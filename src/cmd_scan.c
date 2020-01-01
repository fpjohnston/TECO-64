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

static const struct cmd_table *find_cmd(struct cmd *cmd);

static void scan_text(int delim, struct tstring *tstring);

static void set_opts(struct cmd *cmd, const char *opts);


///
///  @brief    Find command in command table.
///
///  @returns  Command entry found, or NULL.
///
////////////////////////////////////////////////////////////////////////////////

static const struct cmd_table *find_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);

    const struct cmd_table *table;
    int c = toupper(cmd->c1);

    if (scan.digits && !scan.space && isxdigit(c))
    {
        c = '0';
        table = &cmd_table[c];          // then treat A-F the same as 0-9.
    }
    else if (c == 'E')
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
    else if (c == 'F')
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
    else if (c == '^')
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
    else if (c == '\x1E')
    {
        push_expr(cmd->c2, EXPR_VALUE);

        return NULL;
    }
    else                                // Everything else
    {
        table = &cmd_table[c];
    }

    assert(table != NULL);

    // The following allows for whitespace between digits in an expression.
    // This copies the behavior of TECO-32. Note that any hexadecimal digits
    // (A-F) have to follow one or more decimal digits, or they will be
    // interpreted as being part of a command.

    if (isspace(c))
    {
        scan.space = true;
    }
    else
    {
        scan.space = false;

        if (!isdigit(c))
        {
            if (scan.digits)
            {
                push_expr(scan.sum, EXPR_VALUE);
            }

            scan.digits = false;
        }
    }
    
    set_opts(cmd, table->opts);

    return table;
}


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
    scan.sum         = 0;
    scan.digits      = false;
    scan.space       = false;
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
///  @brief    Scan digits in a command string. Note that these can be decimal,
///            octal, or hexadecimal digits, depending on the current radix.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_digits(struct cmd *cmd)
{
    static const char *digits = "0123456789ABCDEF";

    assert(cmd != NULL);

    int c = cmd->c1;

    const char *digit = strchr(digits, toupper(c));

    if (digit == NULL ||                // If not a hexadecimal digit,
        (v.radix != 16 && !isdigit(c)) || // or base 8 or 10 and not a digit,
        (v.radix == 8 && c > '7'))      // or base 8 and digit is 8 or 9
    {
        print_err(E_ILN);               // Illegal number
    }

    long n = digit - digits;            // Value of digit in range [0,15]

    if (!scan.digits)                   // If first digit,
    {
        scan.sum = 0;                   //  then start at zero
    }

    scan.digits = true;
    
    scan.sum *= (int)v.radix;           // Shift over existing digits
    scan.sum += (int)n;                 // And add in the new one
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

    int value = 1;                      // 1 = parenthesis, 2 = operator

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
    else
    {
        value = 2;                      // Arithmetic operator
    }

    push_expr(value, cmd->c1);          // Use operator as expression type
}




///
///  @brief    Do first pass of scanning command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

extern struct tstring expr;
extern char *last_chr;

bool cmd_expr;

exec_func *scan_pass1(struct cmd *cmd)
{
    assert(cmd != NULL);

    // Here to start parsing a command string, one character at a time. Note
    // that although some commands may contain only a single character, most of
    // them comprise multiple characters, so we have to keep looping until we
    // have found everything we need. As we continue, we store information in
    // the command block defined by 'cmd', for use when we're ready to actually
    // execute the command. This includes such things as m and n arguments,
    // modifiers such as : and @, and any text strings following the command.

    cmd->c2 = NUL;
    cmd->c3 = NUL;

    if (toupper(cmd->c1) == 'E' || toupper(cmd->c1) == 'F')
    {
        // E and F commands may have whitespace before 2nd character. This
        // seems to be a feature of TECO-32/TECO C (TODO: which one?)

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

    const struct cmd_table *table = find_cmd(cmd);

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

    if (cmd->c1 == '"')                 // " requires additional character
    {
        cmd->c2 = fetch_buf();
    }

    // table will be NULL for ^^x and CTRL/^ commands, both of which just
    // push a value on the stack, requiring no further processing here.

    if (table == NULL)
    {
        return NULL;
    }

    cmd_expr = false;

    if (table->scan != NULL)            // If we have anything to scan,
    {
        (*table->scan)(cmd);            //  then scan it
    }

    // See if we need to set the expression string for the command.

    if (expr.len == 0 && (table->exec != NULL || cmd_expr))
    {
//        expr.len = (uint)(last_chr - expr.buf);

//        cmd->expr = expr;

        scan_tail(cmd);
    }

    return table->exec;
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
///            m  - Command allows m & n arguments       (e.g., m,nET).
///            n  - Command allows n argument            (e.g., nC).
///            :  - Command allows colon modifier        (e.g., :ERfile`).
///            :: - Command allows double colon modifier (e.g., :: Stext`).
///            @  - Command allows atsign form           (e.g., @^A/hello/).
///            q  - Command requires Q-register          (e.g., Mq).
///            W  - Command allows W                     (e.g., PW).
///            1  - Command allows one text string       (e.g., Otag`).
///            2  - Command allows two text strings      (e.g., FNfoo`baz`).
///                 Note: implies 1.
///            &  - Command passes arguments through     (e.g., [A or ]B).
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
            case '&':
                scan.retain = true;

                break;

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

            case 'm':
                scan.m_opt = true;
                //lint -fallthrough

            case 'n':
                scan.n_opt = true;

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
//                assert(isspace(c));     // TODO: only for debugging
                //lint -fallthrough

            case ',':
                break;
        }
    }

    if (!scan.q_register)
    {
        cmd->qname = NUL;
        cmd->qlocal = false;
    }
}
