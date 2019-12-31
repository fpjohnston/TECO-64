///
///  @file    cmd_scan.c
///  @brief   Functions to scan TECO command string.
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


struct scan scan;                   ///< Internal scan variables


// Local functions

static const struct cmd_table *scan_cmd(struct cmd *cmd);

static uint scan_digits(int c, const char *p, uint len, bool pass1);

static void scan_text(int delim, struct tstring *tstring);

static void set_opts(struct cmd *cmd, const char *opts);

static bool valid_radix(int c);


///
///  @brief    Reset internal variables for next scan.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_scan(enum scan_state state)
{
    scan.state       = state;
    scan.nparens     = 0;
    scan.comma_set   = false;
    scan.colon_opt   = false;
    scan.dcolon_opt  = false;
    scan.atsign_opt  = false;
    scan.w_opt       = false;
    scan.t1_opt      = false;
    scan.t2_opt      = false;
    scan.q_register  = false;
}


///
///  @brief    We've scanned an illegal character, so return to main loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_bad(struct cmd *cmd)
{
    assert(cmd != NULL);

    printc_err(E_ILL, cmd->c1);
}


///
///  @brief    Scan command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static const struct cmd_table *scan_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);

    const struct cmd_table *table;

    if (toupper(cmd->c1) == 'E')
    {
        const char *e_cmds = "%ABCDEFGHIJKLMNOPQRSTUVWXYZ_";
        const char *e_cmd  = strchr(e_cmds, toupper(cmd->c2));

        if (e_cmd == NULL)
        {
            printc_err(E_IEC, cmd->c2); // Illegal E character
        }

        uint i = (uint)(e_cmd - e_cmds);

        assert(i < cmd_e_count);

        table = &cmd_e_table[i];
    }
    else if (toupper(cmd->c1) == 'F')
    {
        const char *f_cmds = "'<>BCDKNRS_|";
        const char *f_cmd  = strchr(f_cmds, toupper(cmd->c2));

        if (f_cmd == NULL)
        {
            printc_err(E_IFC, cmd->c2); // Illegal F character
        }

        uint i = (uint)(f_cmd - f_cmds);

        assert(i < cmd_f_count);

        table = &cmd_f_table[i];
    }
    else if (cmd->c1 == '^')
    {
        if (cmd->c2 == '^')
        {
            push_expr(cmd->c3, EXPR_VALUE);

            return NULL;
        }
        else
        {
            cmd->c1 = (char)(toupper(cmd->c2) - 'A') + 1;
            cmd->c2 = NUL;

            if (cmd->c1 <= NUL || cmd->c1 >= SPACE)
            {
                printc_err(E_IUC, cmd->c1);  // Illegal character following ^
            }

            table = &cmd_table[(int)cmd->c1];
        }
    }
    else if (cmd->c1 == '\x1E')
    {
        push_expr(cmd->c2, EXPR_VALUE);

        return NULL;
    }
    else                                // Everything else
    {
        table = &cmd_table[toupper(cmd->c1)];
    }

    assert(table != NULL);

    set_opts(cmd, table->opts);

    return table;
}


///
///  @brief    Scan digits in a command string. Note that these can be decimal,
///            octal, or hexadecimal digits, depending on the current radix.
///
///  @returns  No. of digits we scanned.
///
////////////////////////////////////////////////////////////////////////////////

static uint scan_digits(int c, const char *p, uint len, bool pass1)
{
    uint maxlen = len;
    long sum = 0;

    // Here when we have a digit. Check to see that it's valid for the current
    // radix, and then loop until we run out of valid digits.
    
    while (valid_radix(c))
    {
        const char *digits = "0123456789ABCDEF";
        const char *digit = strchr(digits, toupper(c));

        assert(digit != NULL);

        long n = digit - digits;

        sum *= v.radix;
        sum += n;

        if (pass1)
        {
            c = fetch_buf();            // Get next digit
        }
        else
        {
            if (len-- == 0)
            {
                break;
            }

            assert(p != NULL);

            c = *p++;
        }
    }

    if (pass1)
    {
        unfetch_buf(c);                 // Return last (non-digit) character
    }

    push_expr((int)sum, EXPR_VALUE);

    return maxlen - len;
}


///
///  @brief    Scan a command modifier (i.e., @ or :).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_mod(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->c1 == '@')
    {
        if (f.ei.strict && cmd->atsign_set)
        {
            print_err(E_MOD);           // Two @'s are not allowed
        }

        cmd->atsign_set = true;
    }
    else if (cmd->c1 == ':')
    {
        if (f.ei.strict && cmd->dcolon_set)
        {
            print_err(E_MOD);           // More than two :'s are not allowed
        }

        if (cmd->colon_set)
        {
            cmd->colon_set = false;
            cmd->dcolon_set = true;
        }
        else if (!cmd->dcolon_set)
        {
            cmd->colon_set = true;
        }
    }

    scan.state = SCAN_MOD;
}


///
///  @brief    Process operator in expression. This may be any of the binary
///            operators (+, -, *, /, &, #), the 1's complement operator (^_),
///            or a left or right parenthesis.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_operator(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->c1 == '(')
    {
        ++scan.nparens;
    }
    else if (cmd->c1 == ')')
    {
        if (scan.nparens == 0)          // Can't have ) without (
        {
            print_err(E_MLP);           // Missing left parenthesis
        }
        else if (!pop_expr(NULL))       // Is there an operand available?
        {
            print_err(E_NAP);           // No argument before )
        }
        else
        {
            --scan.nparens;
        }
    }

    push_expr(0, cmd->c1);              // Use operator as expression type
}




///
///  @brief    Do first pass of scanning command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

exec_func *scan_pass1(struct cmd *cmd)
{
    assert(cmd != NULL);

    // Here to start parsing a command string,  one character at a time. Note
    // that although some commands may contain only a single character, most of
    // them comprise multiple characters, so we have to keep looping until we
    // have found everything we need. As we continue, we store information in
    // the command block defined by 'cmd', for use when we're ready to actually
    // execute the command. This includes such things as m and n arguments,
    // modifiers such as : and @, and any text strings followed the command.

    cmd->c2 = NUL;
    cmd->c3 = NUL;

    if (isdigit(cmd->c1))
    {
        (void)scan_digits(cmd->c1, NULL, 0, (bool)true);

        if (scan.state == SCAN_PASS1)    // Still scanning expression?
        {
            cmd->expr.len = (uint)(next_buf() - cmd->expr.buf);
        }

        return NULL;
    }
    else if (toupper(cmd->c1) == 'E' || toupper(cmd->c1) == 'F')
    {
        // Ex and Fx commands may have whitespace before the 'x'.

        do
        {
            cmd->c2 = (char)fetch_buf();
        } while (isspace(cmd->c2));
    }
    else if (cmd->c1 == '^')
    {
        if ((cmd->c2 = (char)fetch_buf()) == '^')
        {
            cmd->c3 = (char)fetch_buf();
        }
    }
    else if (cmd->c1 == '\x1E')
    {
        cmd->c2 = (char)fetch_buf();
    }

    const struct cmd_table *table = scan_cmd(cmd);

    // Check to see if command requires a global (or local) Q-register.

    if (scan.q_register)                // Do we need a Q-register?
    {
        int c = fetch_buf();            // Yes

        if (c == '.')                   // Is it a local Q-register?
        {
            cmd->qlocal = true;         // Yes, mark it

            c = fetch_buf();            // Get Q-register name
        }        

        if (!isalnum(c))
        {
            // The following allows use of G* and G_

            if (toupper(cmd->c1) != 'G' || (c != '*' && c != '_'))
            {
                printc_err(E_IQN, c);   // Illegal Q-register name
            }
        }

        cmd->qname = (char)c;           // Save the name
    }

    if (table == NULL)
    {
        return NULL;
    }

    if (table->scan != NULL)            // If we have anything to scan,
    {
        (*table->scan)(cmd);            //  then scan it
    }
    else if (table->exec != NULL)
    {
        scan.state = SCAN_PASS2;
    }

    return table->exec;
}


///
///  @brief    Do second pass of scanning for expression.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_pass2(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->expr.len == 0)             // Do we have an expression?
    {
        return;                         // No, so we're done
    }

    reset_scan(SCAN_PASS2);

    init_expr();                        // Re-initialize expression stack

    if (cmd->c1 == ESC)
    {
        return;
    }

    struct cmd tmp;                     // Temporary command block

    memset(&tmp, 0, sizeof(tmp));

    tmp.expr = cmd->expr;

    const char *p = tmp.expr.buf + tmp.expr.len;

    while (isspace(*--p) && tmp.expr.len > 0)
    {
        --tmp.expr.len;
    }

    p = tmp.expr.buf;

    uint len = tmp.expr.len;

    while (len-- > 0)
    {
        int c = *p++;

        tmp.c1 = (char)c;
        tmp.c2 = NUL;
        tmp.c3 = NUL;

        assert((uint)c < cmd_count);

        if (strchr("EF^\x1E", toupper(c)) != NULL)
        {
            if (len-- == 0)
            {
                print_err(E_UTC);       // Unterminated command
            }

            tmp.c2 = *p++;

            if (c == '^' && tmp.c2 == '^')
            {
                if (len-- == 0)
                {
                    print_err(E_UTC);   // Unterminated command
                }

                tmp.c3 = *p++;
            }
        }
        else if (isdigit(c))
        {
            uint ndigits = scan_digits(c, p, len, (bool)false);

            p   += ndigits - 1;
            len -= ndigits - 1;
            
            if (len > 0)
            {
                continue;
            }
            else
            {
                break;
            }
        }

        const struct cmd_table *table = scan_cmd(&tmp);

        // Check to see if command requires a global (or local) Q-register.

        if (scan.q_register)            // Do we need a Q-register?
        {
            if (len-- == 0)
            {
                print_err(E_UTC);       // Unterminated command
            }

            c = *p++;

            if (c == '.')               // Is it a local Q-register?
            {
                tmp.qlocal = true;      // Yes, mark it

                if (len-- == 0)
                {
                    print_err(E_UTC);   // Unterminated command
                }

               c = *p++;                // Get Q-register name
            }        

            if (!isalnum(c))
            {
                // The following allows use of G* and G_

                if (toupper(tmp.c1) != 'G' || (c != '*' && c != '_'))
                {
                    printc_err(E_IQN, c); // Illegal Q-register name
                }
            }

            tmp.qname = (char)c;        // Save the name
        }

        if (table != NULL && table->scan != NULL)
        {
            (*table->scan)(&tmp);       // Scan it if we can
        }
    }

    cmd->m_set = tmp.m_set;
    cmd->n_set = tmp.n_set;
    cmd->h_set = tmp.h_set;

    cmd->m_arg = tmp.m_arg;
    cmd->n_arg = tmp.n_arg;
}


///
///  @brief    Scan the rest of the command string. We enter here after scanning
///            any expression, and any prefix modifiers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_tail(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (scan.nparens != 0)
    {
        print_err(E_MRP);               // Missing right parenthesis
    }
    else if (f.ei.strict)
    {
        if (   (cmd->colon_set  && !scan.colon_opt )
            || (cmd->dcolon_set && !scan.dcolon_opt)
            || (cmd->atsign_set && !scan.atsign_opt))
        {
            print_err(E_MOD);           // Invalid modifier for command
        }
    }

    if (cmd->c1 == CTRL_A || cmd->c1 == '!')
    {
        cmd->delim = cmd->c1;           // Special delimiter for CTRL/A & tag
    }
    else if (f.ee != NUL)
    {
        cmd->delim = (char)f.ee;
    }
    else if (f.et.accent)
    {
        cmd->delim = '`';
    }
    else
    {
        cmd->delim = ESC;
    }

    // The P command can optionally be followed by a W.

    if (scan.w_opt)                     // Optional W following?
    {
        int c = fetch_buf();            // Maybe

        if (toupper(c) == 'W')          // Is it?
        {
            cmd->w_set = true;          // Yes
        }
        else
        {
            unfetch_buf(c);             // No. Put character back
        }
    }

    // The @ prefix modifier has already been scanned here, but if the user
    // specified it, the first character after the command is the alternate
    // delimiter to use for the subsequent text string.

    if (cmd->atsign_set)                // @ modifier?
    {
        cmd->delim = (char)fetch_buf(); // Yes, next character is delimiter
    }

    // Now get the text strings, if they're allowed for this command.

    if (scan.t1_opt)
    {
        scan_text(cmd->delim, &cmd->text1);

        if (scan.t2_opt)
        {
            scan_text(cmd->delim, &cmd->text2);
        }
    }
}


///
///  @brief    Scan the text string following the command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void scan_text(int delim, struct tstring *text)
{
    assert(text != NULL);

    text->len = 0;
    text->buf = next_buf();

    int c;

    if ((c = fetch_buf()) == EOF)
    {
        print_err(E_UTC);               // Unterminated command
    }

    if (c == delim)
    {
        return;
    }

    ++text->len;

    while ((c = fetch_buf()) != delim)
    {
        if (c == EOF)
        {
            print_err(E_UTC);           // Unterminated command
        }

        ++text->len;
    }
}


///
///  @brief    Set options for each command. These are as follows:
///
///            :  - Command allows colon modifier        (e.g., :ERfile`).
///            :: - Command allows double colon modifier (e.g., :: Stext`).
///            @  - Command allows atsign form           (e.g., @^A/hello/).
///            q  - Command requires Q-register          (e.g., Mq).
///            W  - Command allows W                     (e.g., PW).
///            1  - Command allows one text string       (e.g., Otag`).
///            2  - Command allows two text strings      (e.g., FNfoo`baz`).
///                 Note: implies 1.
///
///            These do not need to be in any particular order, and characters
///            not in the list above will be ignored. This allows the use of
///            spaces to improve readability.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void set_opts(struct cmd *cmd, const char *opts)
{
    assert(cmd != NULL);

    scan.q_register = false;

    if (opts == NULL)
    {
        return;
    }

    int c;

    scan.colon_opt  = false;
    scan.dcolon_opt = false;
    scan.atsign_opt = false;
    scan.q_register = false;
    scan.w_opt      = false;
    scan.t1_opt     = false;
    scan.t2_opt     = false;

    while ((c = *opts++) != NUL)
    {
        switch (c)
        {
            case ':':
                if (*opts == ':')       // Double colon?
                {
                    scan.dcolon_opt = true;

                    ++opts;
                }
                else                    // No, just single colon
                {
                    scan.colon_opt  = true;
                }

                break;

            case '@':
                scan.atsign_opt = true;

                break;

            case 'q':
                scan.q_register = true;

                break;

            case 'W':
                scan.w_opt = true;

                break;

            case '2':
                scan.t2_opt = true;
                //lint -fallthrough

            case '1':
                scan.t1_opt = true;

                break;

            default:
                assert(isspace(c));

                break;
        }
    }

    if (!scan.q_register)
    {
        cmd->qname = NUL;
        cmd->qlocal = false;
    }
}


///
///  @brief    Determine if character is valid in our current radix.
///
///  @returns  true if valid, false if not (if invalid octal digit, E_ILN).
///
////////////////////////////////////////////////////////////////////////////////

static bool valid_radix(int c)
{
    if (v.radix == 16 && isxdigit(c))
    {
        return true;
    }
    else if (isdigit(c))
    {
        if (v.radix == 10 || c <= '7')
        {
            return true;
        }

        print_err(E_ILN);               // Illegal octal digit
    }

    return false;
}
