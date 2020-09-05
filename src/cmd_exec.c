///
///  @file    cmd_exec.c
///  @brief   Execute command string.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "qreg.h"
#include "term.h"


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
    .text1  = { .data = NULL, .len = 0 },
    .text2  = { .data = NULL, .len = 0 },
};


// Local functions

static const struct cmd_table *find_cmd(struct cmd *cmd, bool skip);

static void end_cmd(struct cmd *cmd, const union cmd_opts opts);

static const struct cmd_table *scan_ef(struct cmd *cmd,
                                       const struct cmd_table *table,
                                       uint count, int error);

static void scan_mod(struct cmd *cmd, const union cmd_opts opts);

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
    assert(cmd != NULL);                // Error if no command block

    if (f.e2.args && (cmd->h || cmd->ctrl_y))
    {
        throw(E_ARG);                   // Improper arguments
    }
}


///
///  @brief    Check for modifiers and text strings after command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void end_cmd(struct cmd *cmd, const union cmd_opts opts)
{
    assert(cmd != NULL);                // Error if no command block

    // See if we have an n argument. If not, then check to see if the command
    // was preceded by a minus sign, which is equivalent to an argument of -1.

    cmd->n_set = pop_expr(&cmd->n_arg);

    if (cmd->n_set == false && unary_expr())
    {
        cmd->n_set = true;
        cmd->n_arg = -1;
    }

    // If we have an m argument, verify that it is valid for this command, and
    // that it is followed by an n argument.

    if (cmd->m_set)
    {
        if (f.e2.m_arg && !opts.m)
        {
            throw(E_IMA);               // Illegal m argument
        }
        else if (!cmd->n_set)
        {
            throw(E_MNA);               // Missing n argument
        }
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
    assert(cmd != NULL);                // Error if no command block

    throw(E_ILL, cmd->c1);              // Illegal character
}


///
///  @brief    Execute command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_cmd(struct cmd *macro)
{
    struct cmd cmd = null_cmd;

    // If we were called from a macro, then copy any numeric arguments.

    if (macro != NULL)
    {
        if (macro->n_set)
        {
            push_expr(macro->n_arg, EXPR_VALUE);
        }

        cmd.m_set = macro->m_set;
        cmd.m_arg = macro->m_arg;
    }    

    // Loop for all commands in command string.

    while (cbuf->len != 0)
    {
        exec_func *exec = next_cmd(&cmd, NULL);

        if (exec == NULL)
        {
            break;
        }

        (*exec)(&cmd);

        cmd = null_cmd;

        // Check for commands that allow numeric arguments to be passed
        // through to subsequent commands. This includes [ and ], but
        // also !, because it's sometimes useful to interpose comments
        // between two commands.

        if (strchr("![]", cmd.c1) != NULL)
        {
            cmd.n_set = pop_expr(&cmd.n_arg);
            cmd.m_set = pop_expr(&cmd.m_arg);

            if (cmd.n_set)
            {
                push_expr(cmd.n_arg, EXPR_VALUE);
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
///  @brief    Execute ESCape command. We're called here only for ESCapes
///            between commands, or at the end of command strings, not for
///            ESCapes used to delimit text arguments after commands.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_escape(struct cmd *unused1)
{
    // Skip past any whitespace after the ESCape.

    while (!empty_cbuf())
    {
        int c = peek_cbuf();

        if (!isspace(c) || c == TAB)    // Whitespace?
        {
            break;                      // No, so we're done skipping chrs.
        }

        (void)fetch_cbuf();
    }

    // If we've read all characters in command string, then reset for next time.

    if (empty_cbuf())
    {
        cbuf->pos = cbuf->len = 0;
    }
}


///
///  @brief    Find table entry for command.
///
///  @returns  Table entry, or NULL if we're done with command.
///
////////////////////////////////////////////////////////////////////////////////

static const struct cmd_table *find_cmd(struct cmd *cmd, bool skip)
{
    assert(cmd != NULL);                // Error if no command block

    int c = cmd->c1;

    if (nparens != 0 && f.e1.xoper && exec_xoper(c, skip))
    {
        if (c != '{' && c != '}')
        {
            check_args(cmd);
        }

        return NULL;
    }
    else if (c == 'E' || c == 'e')
    {
        return scan_ef(cmd, e_table, e_max, E_IEC);
    }
    else if (c == 'F' || c == 'f')
    {
        return scan_ef(cmd, f_table, f_max, E_IFC);
    }
    else if (c < 0 || c >= (int)cmd_max)
    {
        throw(E_ILL, c);                // Illegal character
    }
    else if (c == '^' || c == '\x1E')
    {
        check_args(cmd);

        if (c == '\x1E' || (c = fetch_cbuf()) == '^')
        {
            c = fetch_cbuf();

            if (!skip)
            {
                push_expr(c, EXPR_VALUE);
            }

            return NULL;
        }

        c -= 'A' - 1;

        if (c <= NUL || c >= SPACE)
        {
            throw(E_IUC, c);            // Illegal character following ^
        }

        cmd->c1 = (char)c;
    }

    return &cmd_table[c];
}


///
///  @brief    Scan command string for next command. Since many commands are
///            used only to create expressions (such as numeric arguments) for
///            other commands, we will continue looping here until we have a
///            complete command.
///
///            The skip parameter determines whether we just ignore commands
///            until we find one that matches one of the characters in the
///            string, at which time we return to the caller. This is used for
///            commands that affect program flow, such as ", F>, or O.
///
///  @returns  Command function to execute, or NULL if at end of command string.
///
////////////////////////////////////////////////////////////////////////////////

exec_func *next_cmd(struct cmd *cmd, const char *skip)
{
    assert(cmd != NULL);                // Error if no command block

    while (!empty_cbuf())
    {
        int c = fetch_cbuf();

        cmd->c1        = (char)c;
        cmd->c2        = NUL;
        cmd->c3        = NUL;
        cmd->qname     = NUL;
        cmd->qlocal    = false;
        cmd->text1.len = 0;
        cmd->text2.len = 0;

        // Skip commands such as LF or SPACE that do nothing, as well as
        // commands that were already processed in find_cmd().

        const struct cmd_table *entry = find_cmd(cmd, skip != NULL);

        if (entry == NULL || entry->exec == NULL)
        {
            continue;
        }

        assert(entry->opts != NULL);

        // Scan for text arguments and other post-command characters.

        scan_tail(cmd, *entry->opts);
        scan_mod(cmd, *entry->opts);    // Scan for @ and : modifiers

        if (skip != NULL && strchr(skip, c) == NULL)
        {

#if     defined(TECO_TRACE)

            if (entry->exec != exec_bang)
            {
                tprintf("*** skipping %s() at %u", entry->name, cbuf->pos);
            }

#endif

            if (c != '@' && c != ':')
            {
                cmd->atsign = cmd->colon = cmd->dcolon = false;
            }

            continue;                   // Ignore command if need to skip
        }

        if (entry->opts->bits != 0)     // Simple command?
        {
            end_cmd(cmd, *entry->opts); // Make sure everything is copacetic

#if     defined(TECO_TRACE)

            if (entry->exec != exec_bang)
            {
                tprintf("+++ executing %s() at %u", entry->name, cbuf->pos);
            }

#endif

            return entry->exec;         // No, so let caller execute it
        }

#if     defined(TECO_TRACE)

        if (entry->exec != exec_bang)
        {
            tprintf("--- executing %s() at %u", entry->name, cbuf->pos);
        }

#endif

        (*entry->exec)(cmd);            // Execute simple command
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

        if (f.e2.args && estack.base != estack.level)
        {
            throw(E_ARG);               // Improper arguments
        }
    }

    return NULL;
}


///
///  @brief    Scan 2nd character for E or F command.
///
///  @returns  Table entry.
///
////////////////////////////////////////////////////////////////////////////////

static const struct cmd_table *scan_ef(struct cmd *cmd,
                                       const struct cmd_table *table,
                                       uint count, int error)
{
    assert(cmd != NULL);                // Error if no command block
    assert(table != NULL);              // Error if no command table pointer

    int c = fetch_cbuf();

    if (c < 0 || (uint)c > count || table[c].exec == NULL)
    {
        throw(error, c);                // Illegal E or F character
    }

    cmd->c2 = (char)c;

    return &table[c];
}


///
///  @brief    Scan for @, :, or :: modifiers, and make sure that they're valid
///            for the command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void scan_mod(struct cmd *cmd, const union cmd_opts opts)
{
    assert(cmd != NULL);
    
    if (cmd->c1 == '@')
    {
        if (cmd->atsign && f.e2.atsign)
        {
            throw(E_MOD);               // Invalid modifier
        }

        cmd->atsign = true;
    }
    else if (cmd->c1 == ':')
    {
        if (cmd->colon & f.e2.colon)
        {
            throw(E_MOD);               // Invalid modifier
        }

        cmd->colon = true;

        if (peek_cbuf() == ':')         // Double colon?
        {
            (void)fetch_cbuf();         // Yes, count it

            if (cmd->dcolon && f.e2.colon)
            {
                throw(E_MOD);           // Invalid modifier
            }

            cmd->dcolon = true;         // And flag it
        }
    }
    else
    {
        if ((cmd->atsign && f.e2.atsign && !opts.a) ||
            (cmd->colon && f.e2.colon && !opts.c) ||
            (cmd->dcolon && f.e2.colon && !opts.d))
        {
            throw(E_MOD);               // Illegal @, :, or ::
        }
    }
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
    assert(cmd != NULL);                // Error if no command block

    if (opts.q)                         // Get Q-register if required
    {
        get_qname(cmd);
    }

    if (cmd->c1 == '=')                 // Might have =, ==, or ===
    {
        if (!empty_cbuf() && peek_cbuf() == '=')
        {
            (void)fetch_cbuf();

            cmd->c2 = cmd->c1;
            
            if (!empty_cbuf() && peek_cbuf() == '=')
            {
                (void)fetch_cbuf();

                cmd->c3 = cmd->c1;
            }
        }

        if (!cmd->atsign)               // Done unless at-sign seen
        {
            return;
        }
    }
    else if (cmd->c1 == '"')            // " requires additional character
    {
        cmd->c2 = (char)fetch_cbuf();

        return;
    }
    else if (opts.w)                    // Is W possible (for P)?
    {
        if (!empty_cbuf())
        {
            int c = peek_cbuf();
            
            if (c == 'W' || c == 'w')
            {
                (void)fetch_cbuf();

                cmd->w = true;
            }
        }

        return;
    }

    // If command doesn't allow any text arguments, then just return.

    if (!opts.t1)
    {
        return;
    }

    // Here to check for text arguments. The standard delimiter is ESCape,
    // except for CTRL/A and ! commands. If an at-sign was specified, then
    // the (non-whitespace) delimiter follows the command.

    if (cmd->c1 == CTRL_A)
    {
        cmd->delim = CTRL_A;            // ^A normally ends with ^A
    }
    else if (cmd->c1 == '!')
    {
        // If feature enabled, !! starts a comment that ends with LF.

        if (f.e1.bang && !empty_cbuf() && peek_cbuf() == '!')
        {
            (void)fetch_cbuf();

            cmd->delim = LF;            // Tag goes to end of line
        }
        else
        {
            cmd->delim = '!';           // ! normally ends with !
        }
    }
    else
    {
        cmd->delim = ESC;               // Standard delimiter
    }

    // If the user specified the at-sign modifier, then skip any whitespace
    // between the command and the delimiter. This does not include tabs,
    // since those are TECO commands.

    if (cmd->atsign)                    // @ modifier?
    {
        while (!empty_cbuf())
        {
            int c = peek_cbuf();

            if (!isspace(c) || c == '\t')
            {
                break;
            }

            (void)fetch_cbuf();
        }

        // The next character has to be the delimiter.

        cmd->delim = (char)fetch_cbuf();
    }

    int delim = cmd->delim;             // Temporary copy of delimiter

    // Now get the text strings. If f.e1.text is enabled and the delimiter is
    // '{', then the text strings may be of the form {xxx}. This allows for
    // commands such as @S {foo} or @FS {foo} {baz}.

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
            while (!empty_cbuf())
            {
                int c = peek_cbuf();

                if (!isspace(c) || c == '\t')
                {
                    break;
                }

                (void)fetch_cbuf();
            }

            scan_text(delim, &cmd->text2);

            delim = '}';
        }

        scan_text(delim, &cmd->text2);
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
    assert(text != NULL);               // Error if no text string buffer

    text->len  = 0;
    text->data = cbuf->data + cbuf->pos;

    // Scan text string, looking for the specified delimiter (usually ESCape).

    for (;;)
    {
        if (fetch_cbuf() == delim)
        {
            return;
        }

        ++text->len;
    }
}
