///
///  @file    cmd_exec.c
///  @brief   Execute command string.
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

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "qreg.h"

uint nparens;                       ///< Parenthesis nesting count

///  @var    null_cmd
///
///  @brief  Initial command block values.

static const struct cmd null_cmd =
{
    .c1     = NUL,
    .c2     = NUL,
    .c3     = NUL,
    .qname  = NUL,
    .qlocal = false,
    .m_set  = false,
    .m_arg  = 0,
    .n_set  = false,
    .n_arg  = 0,
    .h      = false,
    .ctrl_y = false,
    .w      = false,
    .colon  = false,
    .dcolon = false,
    .atsign = false,
    .delim  = ESC,
    .text1  = { .buf = NULL, .len = 0 },
    .text2  = { .buf = NULL, .len = 0 },
};


// Local functions

static void finish_cmd(struct cmd *cmd, union cmd_opts opts);

static const struct cmd_table *get_entry(struct cmd *cmd);

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
///  @brief    Execute illegal character command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_bad(struct cmd *cmd)
{
    assert(cmd != NULL);

    throw(E_ILL, cmd->c1);              // Illegal character
}


///
///  @brief    Execute command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_cmd(void)
{
    struct cmd cmd;                     // Command block

    // Loop for all commands in command string.

    for (;;)
    {
        exec_func *exec = next_cmd(&cmd);

        if (exec == NULL)
        {
            break;
        }

        if (!f.e0.dryrun || cmd.c1 == '?')
        {
            (*exec)(&cmd);

            if (command->len == 0)
            {
                break;
            }
        }

        if (f.e0.ctrl_c)                // If CTRL/C typed, return to main loop
        {
            f.e0.ctrl_c = false;

            throw(E_XAB);               // Execution aborted
        }
    }
}


///
///  @brief    Execute ESCape command. Note that we're called here only for
///            escape characters between commands, or at the end of command
///            strings, not for escapes used to delimit commands (e.g., a
///            command such as "^Ahello, world!<ESC>").
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_escape(struct cmd *unused1)
{
    int c;

    while (command->pos < command->len)
    {
        c = fetch_cbuf(NOSTART);

        if (!isspace(c) || c == TAB)
        {
            unfetch_cbuf(c);

            break;
        }
    }

    if (command->pos == command->len)
    {
        command->pos = command->len = 0;
    }
}


///
///  @brief    Finish non-simple command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void finish_cmd(struct cmd *cmd, union cmd_opts opts)
{
    assert(cmd != NULL);

    // Here when we've seen a command that is more than part of an expression.

    // If command allows one numeric argument, check to see if we have one.
    // If not, then check to see if the command was preceded by a minus
    // sign, which is equivalent to an argument of -1. If no argument is
    // allowed, but there is an operand on top of the stack, then issue an
    // error.

    if (opts.n)
    {
        if (pop_expr(&cmd->n_arg))
        {
            cmd->n_set = true;
        }
        else if (estack.level == estack.base + 1 &&
                 estack.obj[0].type == EXPR_MINUS)
        {
            --estack.level;

            cmd->n_set = true;
            cmd->n_arg = -1;
        }
    }
    else if (check_expr() && f.e2.n_arg)
    {
        throw(E_UNA);                   // Unused n argument
    }

    // If we've seen an 'm' argument, but the command doesn't allow it, then
    // issue an error.

    if (!opts.m && cmd->m_set)
    {
        if (f.e2.m_arg)
        {
            throw(E_UMA);               // Unused m argument
        }

        cmd->m_set = false;             // Just dump the argument
    }

    // Scan for text arguments and other post-command characters.

    scan_tail(cmd, opts);
}


///
///  @brief    Get table entry for command.
///
///  @returns  Table entry, or NULL if we're done with command.
///
////////////////////////////////////////////////////////////////////////////////

static const struct cmd_table *get_entry(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c = toupper(cmd->c1);

    if (exec_xoper(c))                  // Check for extended operators
    {
        if (c != '{' && c != '}')
        {
            check_args(cmd);
        }

        return NULL;
    }
    else if (c == 'E')
    {
        return exec_E(cmd);
    }
    else if (c == 'F')
    {
        return exec_F(cmd);
    }
    else if (c < 0 || c >= (int)cmd_count)
    {
        throw(E_ILL, c);                // Illegal character
    }
    else if (c == '^')
    {
        check_args(cmd);

        if ((c = fetch_cbuf(NOSTART)) == '^')
        {
            c = fetch_cbuf(NOSTART);

            push_expr(c, EXPR_VALUE);

            return NULL;
        }

        c -= 'A' - 1;

        if (c <= NUL || c >= SPACE)
        {
            throw(E_IUC, c);            // Illegal character following ^
        }

        cmd->c1 = (char)c;
    }
    else
    {
        c = toupper(c);
    }

    return &cmd_table[c];
}


///
///  @brief    Scan command string for next command.
///
///  @returns  Command to execute, or NULL if at end of command string.
///
////////////////////////////////////////////////////////////////////////////////

exec_func *next_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c;
    bool start = true;

    *cmd = null_cmd;

    // Start parsing the command string. We will stay in this loop as long as
    // we are only finding simple commands that affect an expression, such as
    // operands or operators that build up the m and n numeric arguments. Once
    // we find a command that affects more than just an expression (opening a
    // file, for instance), then we return to the caller to have the command
    // executed.

    while ((c = fetch_cbuf(start)) != EOF)
    {
        start = false;

        cmd->c1 = (char)c;
        cmd->c2 = NUL;
        cmd->c3 = NUL;

        c = toupper(c);

        const struct cmd_table *entry = get_entry(cmd);

        if (entry == NULL || entry->exec == NULL)
        {
            continue;
        }

        union cmd_opts opts = scan_opts(entry);

        if (opts.q)                     // Q-register required?
        {
            get_qname(cmd, c == 'G' ? "*_$" : NULL);
        }

        // If the character is a 'flag' command such as ET, then it returns a
        // value if and only if there isn't an operand preceding it. Also, the
        // 'A' command returns a value only if it is preceded by an operand.
        // And the 'Q' command always returns a value.

        if ((opts.f && !check_expr()) || (c == 'A' && check_expr()) || c == 'Q')
        {
            if (pop_expr(&cmd->n_arg))
            {
                cmd->n_set = true;
            }
        }
        else if (opts.bits != 0)
        {
            finish_cmd(cmd, opts);

#if 0 // TODO: conditional code below is temporary
            printf("command: ");
            print_cmd(cmd);
#endif
            return entry->exec;
        }

        // If we're strictly enforcing syntax for command modifiers, then
        // @ can only be followed by :, and : can only be followed by @.

        if ((f.e2.atsign && cmd->atsign && c != ':') ||
            (f.e2.colon && cmd->colon && c != '@'))
        {
            throw(E_MOD);               // Invalid command modifier
        }

        (*entry->exec)(cmd);            // Execute simple command

        // Reset any Q-register information so the next command doesn't use it.

        cmd->qname  = NUL;
        cmd->qlocal = false;
    }

    // If we're not in a macro, then confirm that parentheses were properly
    // matched, and that there's nothing left on the expression stack.

    if (!check_macro())
    {
        // Here if we've reached the end of the command string.

        if (nparens)
        {
            throw(E_MRP);               // Missing right parenthesis
        }

        if (estack.base != estack.level)
        {
            throw(E_ARG);               // Improper arguments
        }
    }

    return NULL;
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

        // :: implies :

        if (opts.d)
        {
            opts.c = 1;
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

    if (cmd->c1 == CTRL_A || cmd->c1 == '!')
    {
        cmd->delim = cmd->c1;           // Use special delimiter
    }
    else if (cmd->c1 == '=')
    {
        bool start = check_macro() ? START : NOSTART;

        if ((c = fetch_cbuf(start)) != '=')
        {
            unfetch_cbuf(c);
        }
        else if ((c = fetch_cbuf(start)) != '=')
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
        cmd->c2 = (char)fetch_cbuf(NOSTART);
    }
    else if (opts.w)                    // Is W possible?
    {
        bool start = check_macro() ? START : NOSTART;

        c = fetch_cbuf(start);

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
        while (isspace(c = fetch_cbuf(NOSTART)) && c != '\t')
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
                while (isspace(c = fetch_cbuf(NOSTART)))
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

    int c = fetch_cbuf(NOSTART);

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

    } while ((c = fetch_cbuf(NOSTART)) != delim);
}
