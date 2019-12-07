///
///  @file    cmd_exec.c
///  @brief   Execute command string.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
///
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files (the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


enum scan_state scan_state;             ///< Current expression scanning state

// Local functions

static void exec_expr(struct cmd *cmd);

static void finish_cmd(struct cmd *cmd, const struct cmd_table *table);


///
///  @brief    Execute command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_cmd(void)
{
    struct cmd cmd = null_cmd;

    scan_state = SCAN_NULL;

    // Loop for all characters in command string.

    for (;;)
    {
        // The following is used to note the start of any expression that may
        // precede the actual command. Until we start one, we will keep setting
        // a pointer to our current location in the command string. Once we
        // start an expression, we will increment the count for each character
        // parsed. Once we're done with the expression, we will have a pointer
        // to the start of it, along with the length of the no. of characters.
        //
        // Note that one of the reasons we do it this way is so that we will
        // count will be correct even if the expression contains characters
        // such as spaces which don't affect its evaluation, but which need to
        // be counted anyway.

        if (cmd.expr.len == 0)
        {
            cmd.expr.buf = next_buf();
        }

        if (next_buf() == NULL)         // If end of command string,
        {
            break;                      //  then back to main loop
        }

        int c = fetch_buf();            // Get next command string character

        // Flag that command execution has started. This is placed after we
        // call fetch_buf(), so that any CTRL/C causes return to main loop
        // without an error message.

        // TODO: add check for trace mode

        f.ei.exec = true;

        if ((uint)c >= cmd_count)
        {
            printc_err(E_ILL, c);       // Illegal character
        }

        const struct cmd_table *table = scan_cmd(&cmd, c);

        if (table->scan != NULL)
        {
            (*table->scan)(&cmd);
        }
        else if (table->exec != NULL)
        {
            scan_state = SCAN_DONE;
        }

        // Some commands have a postfix Q-register, which is an alphanumeric name
        // optionally preceded by a . (to flag that it's a local and not a global
        // Q-register.

        if (cmd.q_req)                  // Do we need a Q-register?
        {
            c = fetch_buf();            // Yes

            if (c == '.')               // Is it a local Q-register?
            {
                cmd.qlocal = true;      // Yes, mark it

                c = fetch_buf();        // Get Q-register name
            }        

            if (!isalnum(c))
            {
                // The following allows use of G* and G_

                if (toupper(cmd.c1) != 'G' || (c != '*' && c != '_'))
                {
                    printc_err(E_IQN, c); // Illegal Q-register name
                }
            }

            cmd.qreg = (char)c;         // Save the name
        }

        if (scan_state == SCAN_EXPR)    // Still scanning expression?
        {
            cmd.expr.len = (uint)(next_buf() - cmd.expr.buf);
        }
        else if (scan_state == SCAN_DONE) // Done scanning expression?
        {
            finish_cmd(&cmd, table);

            scan_state = SCAN_NULL;
        }

        f.ei.exec = false;              // Suspend this to check CTRL/C

        if (f.ei.ctrl_c)                // If CTRL/C typed, return to main loop
        {
            f.ei.ctrl_c = false;

            print_err(E_XAB);
        }
    }
}


///
///  @brief    Process ESCape which terminates a command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_escape(struct cmd *cmd)
{
    assert(cmd != NULL);

    init_expr();
}


///
///  @brief    Do a full scan of the expression. All we've done up to now is to
///            check the expression for form. Now we check it for substance.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_expr(struct cmd *cmd)
{
    assert(cmd != NULL);

    cmd->m_set = false;
    cmd->n_set = false;
    cmd->colon_set = false;

    if (cmd->c1 == ESC)
    {
        return;
    }

    const char *p = cmd->expr.buf;

    while (p < cmd->expr.buf + cmd->expr.len)
    {
        struct cmd_table *table = NULL;
        int c = *p++;

        if ((uint)c >= cmd_count)
        {
            printc_err(E_ILL, c);
        }

        cmd->c1 = (char)c;

        if (toupper(c) == 'E')          // E{x} command
        {
            assert(p < cmd->expr.buf + cmd->expr.len);

            cmd->c2 = (char)*p++;

            table = scan_E(cmd);
        }
        else if (toupper(c) == 'F')     // F{x} command
        {
            assert(p < cmd->expr.buf + cmd->expr.len);

            cmd->c2 = (char)*p++;

            table = scan_F(cmd);
        }
        else if (c == '^')              // ^{x} or ^^{x} command
        {
            assert(p < cmd->expr.buf + cmd->expr.len);

            if ((c = *p++) == '^')      // ^^{x} - value of character
            {
                assert(p < cmd->expr.buf + cmd->expr.len);

                push_expr(*p++, EXPR_VALUE);

                continue;
            }
            else                        // ^{x}
            {
                cmd->c1 = (char)c;
                cmd->c1 = (char)scan_caret(cmd);

                table = &cmd_table[(int)cmd->c1];
            }
        }
        else if (c == '\x1E')           // CTRL/^{x} command
        {
            assert(p < cmd->expr.buf + cmd->expr.len);

            push_expr(*p++, EXPR_VALUE);

            continue;
        }
        else if (isdigit(c))
        {
            long sum = 0;

            // Here when we have a digit. Check to see that it's valid for the current
            // radix, and then loop until we run out of valid digits, at which point we
            // have to return the last character to the command buffer.

            --p;

            while (valid_radix(*p))
            {
                const char *digits = "0123456789ABCDEF";
                const char *digit = strchr(digits, toupper(*p));

                assert(digit != NULL);

                long n = digit - digits;

                sum *= v.radix;
                sum += n;

                if (++p >= cmd->expr.buf + cmd->expr.len)
                {
                    break;
                }
            }

            push_expr((int)sum, EXPR_VALUE);

            continue;
        }
        else                            // Everything else
        {
            cmd->c2 = NUL;

            table = &cmd_table[toupper(c)];
        }

        assert(table != NULL);

        if (table->scan != NULL)
        {
            (*table->scan)(cmd);
        }
    }

    if (operand_expr())
    {
        cmd->n_arg = get_n_arg();
        cmd->n_set = true;
    }
}


///
///  @brief    Finish scanning command, after we have done a preliminary scan
///            for an expression, and for any modifiers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void finish_cmd(struct cmd *cmd, const struct cmd_table *table)
{
    assert(cmd != NULL);
    assert(table != NULL);

    (void)fflush(stdout);

    scan_tail(cmd);                     // Finish scanning command

    // Tags that start with !+ are really comments, so discard them.

    if (cmd->c1 == '!')
    {
        if (cmd->text1.len > 0)
        {
            if (cmd->text1.buf[0] == '+')
            {
                return;
            }
        }
    }

    if (cmd->c1 == '\'' || cmd->c1 == '|' || cmd->c1 == '>')
    {
        --cmd->level;
    }

    if (teco_debug && cmd->c1 != ESC)
    {
        print_cmd(cmd);                 // Print command if debugging
    }

    if (cmd->c1 == '"' || cmd->c1 == '|' || cmd->c1 == '<')
    {
        ++cmd->level;
    }

    if (cmd->expr.len != 0)             // Do we have an expression?
    {
        init_expr();                    // Re-initialize the stack

        // Save the stuff we'll need for the actual command

        char c1 = cmd->c1;
        char c2 = cmd->c2;
        char c3 = cmd->c3;
        uint atsign_set = cmd->atsign_set;
        uint colon_set  = cmd->colon_set;
        uint dcolon_set = cmd->dcolon_set;

        exec_expr(cmd);                 // Execute expression for real

        // Restore command characters

        cmd->dcolon_set = dcolon_set;
        cmd->colon_set  = colon_set;
        cmd->atsign_set = atsign_set;
        cmd->c3 = c3;
        cmd->c2 = c2;
        cmd->c1 = c1;
    }

    // If the only thing on the expression stack is a minus sign, then say we
    // have an n argument equal to -1.
    //
    // If we have anything else on the stack after parsing an expression, then
    // that's an error.

    if (cmd->expr.len == 1 && estack.obj[0].type == EXPR_MINUS)
    {
        estack.obj[0].type = EXPR_VALUE;
        estack.obj[0].value = -1;
    }
    else if (estack.level > 0)
    {
        print_err(E_ARG);
    }

    if (table->exec != NULL)
    {
        if (operand_expr())
        {
            cmd->n_set = true;
            cmd->n_arg = get_n_arg();
        }

        if (!teco_debug || cmd->c1 == 'e')
            (*table->exec)(cmd);
    }

    uint level = cmd->level;        

    *cmd = null_cmd;

    cmd->level = level;

    if (operand_expr())                 // Pass through n to next command?
    {
        cmd->n_set = true;
        cmd->n_arg = get_n_arg();
    }

    assert(estack.level == 0);          // Sanity check
}

