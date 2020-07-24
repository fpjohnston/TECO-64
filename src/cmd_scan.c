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
#include "qreg.h"


struct scan scan;                   ///< Internal scan variables


// Local functions

static bool check_value(struct cmd *cmd, union cmd_opts opts);

static bool check_xoper(int c);

static union cmd_opts scan_opts(const struct cmd_table *entry);

static void scan_tail(struct cmd *cmd, union cmd_opts opts);

static void scan_text(int delim, struct tstring *tstring);


///
///  @brief    Check to see if we've already processed H or CTRL/Y.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void check_args(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->h || cmd->ctrl_y)
    {
        throw(E_ARG);                   // Improper arguments
    }
}


///
///  @brief    Check command to see if it can return a value (and therefore be
///            part of an expression).
///
///  @returns  true if a value possible, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool check_value(struct cmd *cmd, union cmd_opts opts)
{
    assert(cmd != NULL);

    if (opts.x)
    {
        return false;
    }
    else if (opts.f && cmd->n_set)
    {
        return false;
    }
    else if (opts.v && !cmd->colon)
    {
        return false;
    }
    else if (cmd->c1 == CTRL_T && cmd->n_set)
    {
        return false;
    }

    return true;
}


///
///  @brief    Check to see if command is an extended operator.
///
///  @returns  true if extended operator found, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool check_xoper(int c)
{
    if (scan.nbraces == 0 && (c != '{' || !f.e1.brace))
    {
        return false;                   // No extended operator possible
    }

    switch (c)
    {
        case '>':                       // Check for >, >=, and >>
            c = fetch_cbuf(NOCMD_START);

            if (c == '=')
            {
                c = EXPR_GE;
            }
            else if (c == '>')
            {
                c = EXPR_RSHIFT;
            }
            else
            {
                unfetch_cbuf(c);

                c = EXPR_GT;
            }

            break;

        case '<':                       // Check for <, <=, <>, and <<
            c = fetch_cbuf(NOCMD_START);

            if (c == '=')
            {
                c = EXPR_LE;
            }
            else if (c == '>')
            {
                c = EXPR_NE;
            }
            else if (c == '<')
            {
                c = EXPR_LSHIFT;
            }
            else
            {
                unfetch_cbuf(c);

                c = EXPR_LT;
            }

            break;

        case '=':                       // Check for ==
            c = fetch_cbuf(NOCMD_START);

            if (c != '=')
            {
                throw(E_ARG);
            }

            break;

        case '/':                       // Check for //
            c = fetch_cbuf(NOCMD_START);

            if (c != '/')
            {
                unfetch_cbuf(c);

                return false;
            }

            c = EXPR_REM;

            break;

        case '!':
        case '~':
            break;

        case '{':
            ++scan.nbraces;

            break;

        case '}':
            if (scan.nbraces == 0)      // Can't have } without {
            {
                throw(E_MLP);           // Missing left brace
            }
            else if (!check_expr())     // Is there an operand available?
            {
                throw(E_NAP);           // No argument before }
            }

            --scan.nbraces;

            break;

        default:
            return false;
    }

    push_expr(TYPE_OPER, c);

    return true;
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
    scan.nbraces     = 0;
}


///
///  @brief    Scan command string for next command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_cmd(struct cmd *cmd, int c)
{
    assert(cmd != NULL);

    // Here to start parsing a command string, one character at a time. Note
    // that although some commands may contain only a single character, most of
    // them comprise multiple characters, so we have to keep looping until we
    // have found everything we need. As we continue, we store information in
    // the command block defined by 'cmd', for use when we're ready to actually
    // execute the command. This includes such things as m and n arguments,
    // modifiers such as : and @, and any text strings following the command.

    cmd->c1 = (char)c;
    cmd->c2 = NUL;
    cmd->c3 = NUL;

    c = toupper(c);

    const struct cmd_table *entry;

    if (check_xoper(c))                 // Check for extended operators
    {
        if (c != '{' && c != '}')
        {
            check_args(cmd);
        }

        return;
    }
    else if (c == 'E')
    {
        entry = exec_E(cmd);
    }
    else if (c == 'F')
    {
        entry = exec_F(cmd);
    }
    else if (c < 0 || c >= (int)cmd_count)
    {
        throw(E_ILL, c);                // Illegal character
    }
    else
    {
        if (c == '^')
        {
            check_args(cmd);

            if ((c = fetch_cbuf(NOCMD_START)) == '^')
            {
                c = fetch_cbuf(NOCMD_START);

                push_expr(c, EXPR_VALUE);

                return;
            }

            c = (toupper(c) - 'A') + 1;

            if (c <= NUL || c >= SPACE)
            {
                throw(E_IUC, c);        // Illegal character following ^
            }

            cmd->c1 = (char)c;
        }
        else
        {
            c = toupper(c);
        }

        entry = &cmd_table[c];
    }

    union cmd_opts opts = scan_opts(entry);

    if (cmd->colon && !opts.c && cmd->c1 != '@')
    {
        throw(E_ARG);                   // Improper arguments
    }
    else if (cmd->dcolon && !opts.d && cmd->c1 != '@')
    {
        throw(E_ARG);                   // Improper arguments
    }
    else if (cmd->atsign && !opts.a && cmd->c1 != ':')
    {
        throw(E_ARG);                   // Improper arguments
    }

    if (opts.n)
    {
        if (pop_expr(&cmd->n_arg))
        {
            cmd->n_set = true;
        }
        else if (estack.level == 1 && estack.obj[0].type == EXPR_MINUS)
        {
            --estack.level;

            cmd->n_set = true;
            cmd->n_arg = -1;
        }
    }
    else if (check_expr())
    {
        if (f.e2.n_arg && opts.bits != 0)
        {
            throw(E_UNA);               // Unused n argument
        }
    }

    if (cmd->m_set)
    {
        if (!opts.m && f.e2.m_arg && opts.bits != 0)
        {
            throw(E_UMA);               // Unused m argument
        }

        cmd->m_set = false;             // Just dump the argument
    }

    bool expr = check_value(cmd, opts);

    if (!expr)
    {
        if (scan.nparens || scan.nbraces)
        {
            throw(E_MRP);               // Missing right parenthesis/brace
        }

        if (estack.level != 0)
        {
            throw(E_ARG);               // Improper arguments
        }
    }

    scan_tail(cmd, opts);

    if (!f.e0.dryrun || expr)
    {
        assert(entry->exec != NULL);

        (*entry->exec)(cmd);
    }

    cmd->m_set     = false;
    cmd->n_set     = false;
    cmd->qname     = NUL;
    cmd->qlocal    = false;
    cmd->text1.len = 0;
    cmd->text2.len = 0;

    if (!expr && !check_expr())
    {
        cmd->h      = false;
        cmd->ctrl_y = false;

        if (cmd->c1 != '[' && cmd->c1 != ']')
        {
            cmd->colon  = false;
            cmd->dcolon = false;
            cmd->atsign = false;
        }
    }
}


///
///  @brief    Get the options for the current command.
///
///  @returns  Command options.
///
////////////////////////////////////////////////////////////////////////////////

static union cmd_opts scan_opts(const struct cmd_table *entry)
{
    assert(entry != NULL);

    union cmd_opts opts;

    if (entry->opts == NULL)            // If no option block for command,
    {
        opts.bits = 0;                  //  then all options are disabled
    }
    else
    {
        opts = *entry->opts;

        // m argument implies n argument, as does a flag command.

        if (opts.m || opts.f)
        {
            opts.n = 1;
        }

        // Terminal command can accept m and n arguments, but ignores them.

        if (opts.x)
        {
            opts.m = 1;
            opts.n = 1;
        }

        // :: implies :, as does a colon-modified command that returns a value.

        if (opts.d || opts.v)
        {
            opts.c = 1;
        }

        // Q-register set for G commands are a superset of regular Q-reg. set.

        if (opts.g)
        {
            opts.q = 1;
        }

        // If 2nd text argument is allowed, so is 1st text argument.

        if (opts.t2)
        {
            opts.t1 = 1;
        }
    }

    return opts;
}


///
///  @brief    Scan the rest of the command string. We enter here after scanning
///            any expression, and any prefix modifiers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void scan_tail(struct cmd *cmd, union cmd_opts opts)
{
    assert(cmd != NULL);

    int c;

    cmd->delim = ESC;                   // Assume standard delimiter

    if (cmd->c1 == CTRL_A)
    {
        cmd->delim = cmd->c1;           // Use special delimiter for CTRL/A
    }
    else if (cmd->c1 == '!')
    {
        if (scan.nbraces != 0)
        {
            return;
        }

        cmd->delim = cmd->c1;           // Use special delimiter for !
    }
    else if (cmd->c1 == '=')
    {
        if ((c = fetch_cbuf(NOCMD_START)) != '=')
        {
            unfetch_cbuf(c);
        }
        else if ((c = fetch_cbuf(NOCMD_START)) != '=')
        {
            unfetch_cbuf(c);

            cmd->c2 = '=';
        }
        else
        {
            cmd->c3 = cmd->c2 = '=';
        }

        if (!cmd->atsign)
        {
            return;
        }
    }
    else if (cmd->c1 == '"')            // " requires additional character
    {
        cmd->c2 = (char)fetch_cbuf(NOCMD_START);
    }
    else if (opts.q)                    // Q-register required?
    {
        get_qname(cmd, opts.g ? "*_$" : NULL);
    }
    else if (opts.w)                    // Is W possible?
    {
        c = fetch_cbuf(NOCMD_START);

        if (toupper(c) == 'W')
        {
            cmd->w = true;
        }
        else
        {
            unfetch_cbuf(c);
        }
    }

    // If the user specified the at-sign modifier, then we allow whitespace
    // between the command and the delimiter. Note that whitespace does not
    // include tabs, since those are TECO commands.

    if (cmd->atsign)                    // @ modifier?
    {
        while (isspace(c = fetch_cbuf(NOCMD_START)) && c != '\t')
        {
            ;                           // Skip leading whitespace
        }

        cmd->delim = (char)c;           // Next character is delimiter
    }

    int delim = cmd->delim;

    // Now get the text strings, if they're allowed for this command.
    // Note that if f.e1.text is enabled and the delimiter is '{', then
    // the text strings may be of the form {xxx}. This allows for such
    // commands as @S {foo} or @FS {foo} {baz}.

    if (opts.t1)
    {
        if (f.e1.text && cmd->delim == '{')
        {
            delim = '}';
        }

        scan_text(delim, &cmd->text1);

        if (opts.t2)
        {
            delim = cmd->delim;

            if (f.e1.text && cmd->delim == '{')
            {
                while (isspace(c = fetch_cbuf(NOCMD_START)))
                {
                    ;                   // Skip whitespace
                }

                unfetch_cbuf(c);

                scan_text(delim, &cmd->text2);

                delim = '}';
            }

            scan_text(delim, &cmd->text2);
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

    bool caret = false;                 // Flag for scanned uparrow character

    // Scan text string, looking for the specified delimiter. This is usually
    // ESCape, but may be other characters. If it is ESCape, then we also check
    // to see if we encounter ^[, which we will treat as equivalent to ESCape.
    // Only doing this when the delimiter is ESCape allows the use of ESCape
    // characters in such situations as the use of at-sign modifiers.

    do
    {
        if (delim == ESC)
        {
            if (caret && c == '[')
            {
                --text->len;            // Back off the ^ already counted

                return;
            }

            caret = (c == '^');
        }

        ++text->len;

    } while ((c = fetch_cbuf(NOCMD_START)) != delim);
}
