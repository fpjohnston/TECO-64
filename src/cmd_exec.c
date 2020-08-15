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


#undef toupper

/// @def    toupper(c)
/// @brief  Converts character from lower case to upper case.

#define toupper(c) ((c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c) //lint !e652

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

static const struct cmd_table *find_cmd(struct cmd *cmd);

static void finish_cmd(struct cmd *cmd, union cmd_opts opts);

static const struct cmd_table *scan_ef(struct cmd *cmd, const char *cmds,
                                       const struct cmd_table *table,
                                       uint count, int error);

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
///  @brief    Verify that we're not prematurely at end of command; if we are,
///            then throw an exception.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void check_end(void)
{
    if (command->pos == command->len)
    {
        if (check_macro())
        {
            throw(E_UTM);           // Unterminated macro
        }
        else
        {
            throw(E_UTC);           // Unterminated command
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
///  @brief    Find table entry for command.
///
///  @returns  Table entry, or NULL if we're done with command.
///
////////////////////////////////////////////////////////////////////////////////

static const struct cmd_table *find_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c = toupper(cmd->c1);

    if (nparens != 0 && f.e1.xoper && exec_xoper(c))
    {
        if (c != '{' && c != '}')
        {
            check_args(cmd);
        }

        return NULL;
    }
    else if (c == 'E')
    {
        static const char *e_cmds = "1234ABCDEFGHIJKLMNOPQRSTUVWXY_";

        return scan_ef(cmd, e_cmds, e_table, e_count, E_IEC);
    }
    else if (c == 'F')
    {
        static const char *f_cmds = "'123<>BCDKLNRSU_|";

        return scan_ef(cmd, f_cmds, f_table, f_count, E_IFC);
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

    return &cmd_table[c];
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

    // See if we have an n argument. If not, then check to see if the command
    // was preceded by a minus sign, which is equivalent to an argument of -1.

    if (pop_expr(&cmd->n_arg))          // Do we have an n argument?
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

    // Scan for text arguments and other post-command characters.

    scan_tail(cmd, opts);
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

    *cmd = null_cmd;

    // Start parsing the command string. We will stay in this loop as long as
    // we are only finding simple commands that affect an expression, such as
    // operands or operators that build up the m and n numeric arguments. Once
    // we find a command that affects more than just an expression (opening a
    // file, for instance), then we return to the caller to have the command
    // executed.

    while (command->pos < command->len)
    {
        int c = command->buf[command->pos++];

        cmd->c1 = (char)c;
        cmd->c2 = NUL;
        cmd->c3 = NUL;

        c = toupper(c);

        const struct cmd_table *entry = find_cmd(cmd);

        if (entry == NULL || entry->exec == NULL)
        {
            continue;
        }

        assert(entry->opts != NULL);

        union cmd_opts opts = *entry->opts;

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

    if (loop_depth != 0)
    {
        throw(E_UTL);                   // Unterminated loop
    }
    else if (if_depth != 0)
    {
        throw(E_UTQ);                   // Unterminated conditional
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
///  @brief    Scan 2nd character for E or F command.
///
///  @returns  Table entry.
///
////////////////////////////////////////////////////////////////////////////////

static const struct cmd_table *scan_ef(struct cmd *cmd, const char *cmds,
                                       const struct cmd_table *table,
                                       uint count, int error)
{
    assert(cmd != NULL);
    assert(cmds != NULL);
    assert(table != NULL);

    check_end();

    int c = command->buf[command->pos++];

    const char *p = strchr(cmds, toupper(c));

    if (p == NULL)
    {
        throw(error, c);                // Illegal E character
    }

    cmd->c2 = (char)c;

    uint idx = (uint)(p - cmds);

    assert(idx < count);

    return &table[idx];
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

    if (cmd->c1 == '=')                 // Might have =, ==, or ===
    {
        if (command->pos < command->len && command->buf[command->pos] == '=')
        {
            ++command->pos;

            cmd->c2 = cmd->c1;
            
            if (command->pos < command->len && command->buf[command->pos] == '=')
            {
                ++command->pos;

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
        check_end();

        cmd->c2 = command->buf[command->pos++];

        return;
    }
    else if (opts.w)                    // Is W possible (for P)?
    {
        if (command->pos != command->len)
        {
            int c = command->buf[command->pos];
            
            if (toupper(c) == 'W')
            {
                cmd->w = true;
                ++command->pos;
            }
        }

        return;
    }

    // If command doesn't allow any text arguments, then just return, but throw
    // an exception if we've seen an at-sign.

    if (!opts.t1)
    {
        if (cmd->atsign)
        {
            throw(E_MOD);               // Invalid modifier
        }

        return;
    }

    // Here to check for text arguments. The standard delimiter is ESCape,
    // except for CTRL/A and ! commands. If an at-sign was specified, then
    // the (non-whitespace) delimiter follows the command.

    if (cmd->c1 == CTRL_A || cmd->c1 == '!')
    {
        cmd->delim = cmd->c1;           // Use special delimiter
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
        while (command->pos < command->len)
        {
            int c = command->buf[command->pos];

            if (!isspace(c) || c == '\t')
            {
                break;
            }

            ++command->pos;
        }

        check_end();                    // Must have at least 1 more character

        // The next character has to be the delimiter.

        cmd->delim = command->buf[command->pos++];
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
            while (command->pos < command->len)
            {
                int c = command->buf[command->pos];

                if (!isspace(c) || c == '\t')
                {
                    break;
                }

                ++command->pos;
            }

            check_end();

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
    assert(text != NULL);

    text->len = 0;
    text->buf = command->buf + command->pos;

    // Scan text string, looking for the specified delimiter (usually ESCape).

    while (command->pos < command->len)
    {
        if (command->buf[command->pos++] == delim)
        {
            return;
        }

        ++text->len;
    }

    // Here if we reached end of command string before we saw a delimiter.

    check_end();
}
