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


// Local functions

static void exec_escape(bool double_escape);


///
///  @brief    Execute command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_cmd(void)
{
    // Loop for all characters in command string.

    bool double_escape = false;

    for (;;)
    {
        struct cmd cmd;

        if (next_buf() == NULL)         // If end of command string,
        {
            break;                      //  then back to main loop
        }

        // TODO: add check for trace mode

        f.ei.exec = true;               // Command execution has started

        exec_func *exec = next_cmd(&cmd); // Find and scan next command

        scan_tail(&cmd);                // Scan stuff after command character

        // Assume any tags that start with a space are really comments,
        // and skip them.

        if (cmd.c1 == '!' && cmd.text1.len != 0 && cmd.text1.buf[0] == ' ')
        {
            continue;
        }

        scan_pass2(&cmd);               // Scan expression for real
        log_cmd(&cmd);                  // Log command

        // If only thing on expression stack is a minus sign, then say we have
        // an n argument equal to -1. Otherwise check for m and n arguments.

        if (cmd.expr.len == 1 && estack.obj[0].type == EXPR_MINUS)
        {
            estack.level = 0;

            cmd.n_set = true;
            cmd.n_arg = -1;
        }
        else if (pop_expr(&cmd.n_arg))
        {
            cmd.n_set = true;

            if (pop_expr(&cmd.m_arg))
            {
                cmd.m_set = true;
            }
        }

        if (!teco_debug || !test_indirect())
        {
            if (cmd.c1 == ESC)
            {
                exec_escape(double_escape);

                if (double_escape)
                {
                    return;
                }

                double_escape = true;
            }
            else if (exec != NULL)
            {
                (*exec)(&cmd);          // Now finally execute command
            }
        }

        if (cmd.c1 != ESC)
        {
            double_escape = false;
        }

        f.ei.exec = false;              // Suspend this to check CTRL/C

        if (f.ei.ctrl_c)                // If CTRL/C typed, return to main loop
        {
            f.ei.ctrl_c = false;

            print_err(E_XAB);           // Execution aborted
        }
    }
}


///
///  @brief    Process ESCape which terminates a command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_escape(bool double_escape)
{
    //  If we're in strict mode, and a command ended while we were in
    //  the middle of a conditional statement, then issue an error.

    if (f.ei.strict && test_if() && double_escape)
    {
        print_err(E_UTQ);               // Unterminated quote command
    }

    init_expr();
}


///
///  @brief    Get next command.
///
///  @returns  Pointer to function to execute command.
///
////////////////////////////////////////////////////////////////////////////////

exec_func *next_cmd(struct cmd *cmd)
{
    ///  @var    null_cmd
    ///  @brief  Initial command block values.

    static const struct cmd null_cmd =
    {
        .m_set      = false,
        .n_set      = false,
        .h_set      = false,
        .w_set      = false,
        .colon_set  = false,
        .dcolon_set = false,
        .atsign_set = false,
        .c1         = NUL,
        .c2         = NUL,
        .c3         = NUL,
        .m_arg      = 0,
        .n_arg      = 0,
        .delim      = ESC,
        .qname      = NUL,
        .qlocal     = false,
        .expr       = { .buf = NULL, .len = 0 },
        .text1      = { .buf = NULL, .len = 0 },
        .text2      = { .buf = NULL, .len = 0 },
    };

    assert(cmd != NULL);

    *cmd = null_cmd;

    reset_scan(SCAN_PASS1);

    // Loop for all characters in command.

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

        if (cmd->expr.len == 0)
        {
            cmd->expr.buf = next_buf();
        }

        int c = fetch_buf();            // Get next command string character

        if (c == NUL)
        {
            assert(c != NUL);
        }

        if (c == EOF)
        {
            if (f.ei.strict && test_if())
            {
                print_err(E_UTQ);       // Unterminated quote
            }

            print_err(E_UTC);           // Unterminated command
        }

        if (c == LF || c == CR)
        {
            continue;
        }

        // TODO: add check for trace mode

        if ((uint)c >= cmd_count)
        {
            printc_err(E_ILL, c);       // Illegal character
        }

        if ((cmd->c1 = (char)c) == ESC)
        {
            return NULL;
        }

        exec_func *exec = scan_pass1(cmd);

        if (scan.state == SCAN_PASS1)   // Still scanning expression?
        {
            // If we haven't started scanning an expression, and we just read a
            // whitespace character, then just skip it. Note that if we see a
            // TAB, we'll switch state to SCAN_PASS2, so there's no concern
            // that the code below will cause TABs to be ignored.

            if (cmd->expr.len == 0 || !isspace(c))
            {
                cmd->expr.len = (uint)(next_buf() - cmd->expr.buf);
            }
        }
        else if (scan.state == SCAN_PASS2) // Done scanning expression?
        {
            return exec;
        }
    }
}
