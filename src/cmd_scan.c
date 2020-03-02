///
///  @file    cmd_scan.c
///  @brief   Functions to scan TECO command string.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
///
///  Permission is hereby granted, free of charge, to any person obtaining a
///  copy of this software and associated documentation files (the "Software"),
///  to deal in the Software without restriction, including without limitation
///  the rights to use, copy, modify, merge, publish, distribute, sublicense,
///  and/or sell copies of the Software, and to permit persons to whom the
///  Software is furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIA-
///  BILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
///  THE SOFTWARE.
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
#include "estack.h"
#include "exec.h"


struct scan scan;                   ///< Internal scan variables


// Local functions

static exec_func *find_cmd(struct cmd *cmd);

static void scan_tail(struct cmd *cmd);

static void scan_text(int delim, struct tstring *tstring);

static void set_opts(struct cmd *cmd, const char *opts);


///
///  @brief    Find command in command table.
///
///  @returns  Command function found, or NULL if not found.
///
////////////////////////////////////////////////////////////////////////////////

static exec_func *find_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);

    const struct cmd_table *table;
    int c = toupper(cmd->c1);

    if (c == 'E')
    {
        const char *e_cmds = "%123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_";
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
        const char *f_cmds = "'<>BCDKLNRSU_|";
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
                printc_err(E_IUC, cmd->c1); // Illegal character following ^
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
    // This copies the behavior of TECO-32.

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

    return table->exec;
}


///
///  @brief    Reset internal variables for next scan.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_scan(void)
{
    scan.nparens     = 0;
    scan.sum         = 0;
    scan.digits      = false;
    scan.expr        = false;
    scan.brace_opt   = false;
    scan.flag        = false;
    scan.mod         = false;
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
///  @brief    Scan command string for next part of command.
///
///  @returns  Function to execute, or NULL if command is not yet complete.
///
////////////////////////////////////////////////////////////////////////////////

exec_func *scan_cmd(struct cmd *cmd)
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
            cmd->c2 = (char)fetch_cbuf(NOCMD_START);
        } while (isspace(cmd->c2));
    }
    else if (cmd->c1 == '^')
    {
        if ((cmd->c2 = (char)fetch_cbuf(NOCMD_START)) == '^')
        {
            cmd->c3 = (char)fetch_cbuf(NOCMD_START);
        }
    }
    else if (cmd->c1 == '\x1E')
    {
        cmd->c2 = (char)fetch_cbuf(NOCMD_START);
    }

    exec_func *exec = find_cmd(cmd);

    if (f.e3.brace && scan.nbraces && scan.brace_opt)
    {
        scan.expr = true;

        exec = exec_operator;
    }

    if (exec == NULL)
    {
        return NULL;
    }

    // Check to see if command requires a global (or local) Q-register.

    if (scan.q_register)                // Do we need a Q-register?
    {
        int c = fetch_cbuf(NOCMD_START); // Yes

        if (c == '.')                   // Is it a local Q-register?
        {
            cmd->qlocal = true;         // Yes, mark it

            c = fetch_cbuf(NOCMD_START); // Get Q-register name
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
        cmd->c2 = (char)fetch_cbuf(NOCMD_START);
    }

    // CTRL/T, A, and \ (backslash) may or may not be part of an
    // expression, depending on whether they're preceded by a numeric
    // value, and L and W are only part of an expression if a colon
    // modifier is specified.

    if (cmd->c1 == CTRL_T && pop_expr(&cmd->n_arg))
    {
        scan.expr = false;              // n^T types out a value
        push_expr(cmd->n_arg, EXPR_VALUE);
    }
    else if (toupper(cmd->c1) == 'A' && pop_expr(&cmd->n_arg))
    {
        if (cmd->colon_set)
        {
            scan.expr = false;          // n:A appends pages
            push_expr(cmd->n_arg, EXPR_VALUE);
        }
        else
        {
            cmd->n_set = true;
        }
    }
    else if (toupper(cmd->c1) == 'L' && !cmd->colon_set)
    {
        scan.expr = false;              // nL moves lines, n:L counts lines
    }
    else if (toupper(cmd->c1) == 'W' && !cmd->colon_set)
    {
        scan.expr = false;              // nW sets scope stuff
    }
    else if (cmd->c1 == '\\' && pop_expr(&cmd->n_arg))
    {
        scan.expr = false;              // n\ inserts value in buffer
        push_expr(cmd->n_arg, EXPR_VALUE);
    }
    else if (scan.flag && pop_expr(&cmd->n_arg))
    {
        scan.expr = false;
        push_expr(cmd->n_arg, EXPR_VALUE);
    }

#if     0 // TODO: what was this here for?
    if (scan.expr && scan.n_opt && (cmd->m_set || cmd->n_set))
    {
        ;
    }
    else
    {
    }
#endif

    // If character is part of an expression or a modifier, then process
    // it here and tell our caller that we're not done with the command.

    if (scan.expr || exec == exec_mod)
    {
        (*exec)(cmd);

        if (exec != exec_mod)
        {
            cmd->colon_set = scan.mod = false;
        }

        return NULL;
    }

    // Here if we've found the terminal command, so process any following post
    // data (e.g., text strings), and then tell our caller what we found.

    scan_tail(cmd);

    return exec;
}


///
///  @brief    Scan the rest of the command string. We enter here after scanning
///            any expression, and any prefix modifiers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void scan_tail(struct cmd *cmd)
{
    assert(cmd != NULL);

    // Text argument for equals command is only possible with at-sign. This is
    // necessary not to break instances which don't include a text command.

    if (cmd->c1 == '=' && !cmd->atsign_set)
    {
        scan.t1_opt = scan.t2_opt = false;
    }

    if (scan.nparens != 0)
    {
        print_err(E_MRP);               // Missing right parenthesis
    }
    else if (f.e1.strict)
    {
        if ((cmd->colon_set  && !scan.colon_opt ) ||
            (cmd->dcolon_set && !scan.dcolon_opt) ||
            (cmd->atsign_set && !scan.atsign_opt))
        {
            print_err(E_MOD);           // Invalid modifier for command
        }
    }

    if (cmd->c1 == CTRL_A || cmd->c1 == '!')
    {
        cmd->delim = cmd->c1;           // Special delimiter for CTRL/A & tag
    }
    else
    {
        cmd->delim = ESC;
    }

    // The P command can optionally be followed by a W.

    if (scan.w_opt)                     // Optional W following?
    {
        int c = fetch_cbuf(NOCMD_START); // Maybe

        if (toupper(c) == 'W')          // Is it?
        {
            cmd->w_set = true;          // Yes
        }
        else
        {
            unfetch_cbuf(c);             // No. Put character back
        }
    }

    // The @ prefix modifier has already been scanned here, but if the user
    // specified it, the first character after the command is the alternate
    // delimiter to use for the subsequent text string.

    if (cmd->atsign_set)                // @ modifier?
    {
        cmd->delim = (char)fetch_cbuf(NOCMD_START); // Yes, next character is delimiter
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
    text->buf = next_cbuf();

    int c = fetch_cbuf(NOCMD_START);

    if (c == delim)
    {
        return;
    }

    ++text->len;

    while (fetch_cbuf(NOCMD_START) != delim)
    {
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

    scan.expr       = false;
    scan.brace_opt  = false;
    scan.flag       = false;
    scan.m_opt      = false;
    scan.n_opt      = false;
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
            case 'b':
                scan.brace_opt = true;

                break;

            case 'f':
                scan.flag = true;

                break;

            case 'x':
                scan.expr = true;

                break;

            case 'm':
                scan.m_opt = true;
                //lint -fallthrough

            case 'n':
                scan.n_opt = true;

                break;

            case 'a':
                scan.mod = true;

                break;

            case ':':
                if (*opts == ':')       // Double colon?
                {
                    scan.dcolon_opt = true;
                    scan.colon_opt = true;

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
