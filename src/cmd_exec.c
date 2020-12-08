///
///  @file    cmd_exec.c
///  @brief   Execute command string.
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"

#include "cbuf.h"
#include "commands.h"


uint nparens;                       ///< Parenthesis nesting count

///  @var    null_cmd
///
///  @brief  Initial command block values.

const struct cmd null_cmd =
{
    .c1     = NUL,
    .c2     = NUL,
    .c3     = NUL,
    .qname  = NUL,
    .qlocal = false,
    .qindex = -1,
    .m_set  = false,
    .m_arg  = 0,
    .n_set  = false,
    .n_arg  = 0,
    .h      = false,
    .ctrl_y = false,
    .colon  = false,
    .dcolon = false,
    .atsign = false,
    .delim  = ESC,
    .text1  = { .data = NULL, .len = 0 },
    .text2  = { .data = NULL, .len = 0 },
};


// Local functions

static inline exec_func *scan_cmd(struct cmd *cmd, int c);

static void scan_text(int delim, struct tstring *text);

///
///  @brief    Execute command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);

    // Loop for all commands in command string.

    int c;

    while ((c = fetch_cbuf()) != EOF)
    {
        exec_func *exec;

        // The specific check for a space is an optimization which was found
        // through testing to make a noticeable difference with some macros.

        if (c == SPACE || (exec = scan_cmd(cmd, c)) == NULL)
        {
            continue;
        }

        (*exec)(cmd);

        if (f.e0.ctrl_c)
        {
            if (f.et.ctrl_c)
            {
                f.et.ctrl_c = false;
            }
            else
            {
                f.e0.ctrl_c = false;

                throw(E_XAB);
            }
        }

        *cmd = null_cmd;
    }

    // Here to make sure that all conditionals, loops, and parenthetical
    // expressions were complete within the command string just executed.

    if (if_depth != 0)
    {
        throw(E_MAP);                   // Missing apostrophe
    }
    else if (loop_depth != 0)
    {
        throw(E_MRA);                   // Missing right angle bracket
    }
    else if (nparens != 0)
    {
        throw(E_MRP);                   // Missing right parenthesis
    }
}


///
///  @brief    Scan for secondary commands (E, F, and ^).
///
///  @returns  Table entry if need to execute command, NULL if done scanning.
///
////////////////////////////////////////////////////////////////////////////////

static inline exec_func *scan_cmd(struct cmd *cmd, int c)
{
    assert(cmd != NULL);

    if ((uint)c >= cmd_max)
    {
        throw(E_ILL, c);                // Illegal command
    }

    cmd->c1 = (char)c;

    const struct cmd_table *entry = &cmd_table[c];

    // Check for secondary commands (E, F, and ^).

    if (entry->scan == NULL && entry->exec == NULL)
    {
        // Note that the order of the conditional tests below are based on
        // the anticipated frequency of the respective commands. That is,
        // we anticipate ^ commands more often than E commands, which in
        // turn are more frequent than F commands.

        if (c == '^')
        {
            if ((c = require_cbuf()) == '^')
            {
                push_x(require_cbuf(), X_OPERAND);

                return NULL;
            }

            c = 1 + toupper(c) - 'A';

            if (c <= NUL || c >= SPACE)
            {
                throw(E_IUC, c);        // Invalid character following ^
            }

            cmd->c1 = (char)c;

            entry = &cmd_table[c];
        }
        else if (c == 'E' || c == 'e')
        {
            c = require_cbuf();

            if ((uint)c > e_max || (e_table[c].scan == NULL &&
                                    e_table[c].exec == NULL))
            {
                throw(E_IEC, c);        // Invalid E character
            }

            cmd->c2 = (char)c;

            entry = &e_table[c];
        }
        else if (c == 'F' || c == 'f')
        {
            c = require_cbuf();

            if ((uint)c > f_max || (f_table[c].scan == NULL &&
                                    f_table[c].exec == NULL))
            {
                throw(E_IFC, c);        // Invalid F character
            }

            cmd->c2 = (char)c;

            entry = &f_table[c];
        }
    }

    // If we will execute a command after scanning it, then see if we
    // have an n argument we can pop off the expression stack. If not,
    // then check to see if we have a unary minus, which we will treat
    // as equivalent to a -1 argument (that is, -P is the same as -1P).

    if (entry->exec != NULL)
    {
        if (isoperand())
        {
            cmd->n_set = true;
            cmd->n_arg = pop_x();
        }
        else if (!cmd->n_set && unary_x())
        {
            cmd->n_set = true;
            cmd->n_arg = -1;
        }
    }

#if     defined(TECO_TRACE)

    if (entry->scan != scan_nop && entry->scan != NULL)
    {
        tprint("--- %s\r\n", entry->scan_name);
    }

#endif

    if (entry->parse != NULL)
    {
        (void)(*entry->parse)(cmd);

        if ((*entry->scan)(cmd))
        {
            return NULL;
        }
    }
    else if (entry->scan != NULL && (*entry->scan)(cmd))
    {
        return NULL;
    }

#if     defined(TECO_TRACE)

    if (entry->exec != NULL)
    {
        tprint("+++ %s\r\n", entry->exec_name);
    }

#endif

    return entry->exec;
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

    text->data = cbuf->data + cbuf->pos;

    uint n = cbuf->len - cbuf->pos;
    char *end = memchr(text->data, delim, (ulong)n);

    if (end == NULL)
    {
        abort_cbuf();
    }

    text->len = (uint)(end - text->data);

    if (f.e0.trace)
    {
        const char *p = text->data;

        for (uint i = 0; i < text->len + 1; ++i)
        {
            echo_in(*p++);
        }
    }

    cbuf->pos += text->len + 1;
}


///
///  @brief    Scan for text strings following command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_texts(struct cmd *cmd, int ntexts, int delim)
{
    assert(cmd != NULL);

    if (cmd->c1 == CTRL_A)
    {
        cmd->delim = CTRL_A;
    }
    else
    {
        cmd->delim = (char)delim;
    }

    // If the user specified the at-sign modifier, then skip any whitespace
    // between the command and the delimiter.

    if (cmd->atsign)                    // @ modifier?
    {
        int c;

        while ((c = require_cbuf()) != TAB && isspace(c))
        {
            ;
        }

        if (!isgraph(c))
        {
            throw(E_ATS);               // Invalid delimiter
        }

        cmd->delim = (char)c;
    }

    if (cmd->delim != '{' || !f.e1.text)
    {
        scan_text(cmd->delim, &cmd->text1);

        if (ntexts == 2)
        {
            scan_text(cmd->delim, &cmd->text2);
        }

        return;
    }

    // Here if user wants to delimit text string(s) with braces. This means
    // the text strings may be of the form {xxx}, and may include whitespace.
    // This allows for commands such as @S {foo} or @FS {foo} {baz}.

    scan_text('}', &cmd->text1);

    if (ntexts != 2)
    {
        return;
    }

    // Skip any whitespace after '}'

    int c;

    while ((c = peek_cbuf()) != EOF && isspace(c) && c != TAB)
    {
        next_cbuf();                    // Skip whitespace character
    }

    c = require_cbuf();                 // Get text string delimiter

    scan_text(c == '{' ? '}' : c, &cmd->text2);
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
///            branch and loop commands such as ", F>, and O.
///
///  @returns  true if found a match, false if we reached end of command string.
///
////////////////////////////////////////////////////////////////////////////////

bool skip_cmd(struct cmd *cmd, const char *skip)
{
    assert(cmd != NULL);
    assert(skip != NULL);

    *cmd = null_cmd;

    // Some of the commands we parse may push operands or operators on the
    // stack. Since we don't need those when we find the command we want,
    // we just reset the expression stack when we return. If an error occurs,
    // the entire stack will be reset elsewhere.

    uint saved_level = x.level;
    bool match = false;                 // Assume failure
    bool saved_exec = f.e0.exec;
    int c;

    f.e0.exec = false;

    while ((c = fetch_cbuf()) != EOF)
    {
        // The specific check for a space is an optimization which was found
        // through testing to make a noticeable difference with some macros.

        if (c == SPACE || scan_cmd(cmd, c) == NULL)
        {
            continue;
        }

        // If this command matches what we're looking for, then exit.

        if (strchr(skip, c) != NULL)
        {
            match = true;

            break;
        }

        *cmd = null_cmd;
    }

    f.e0.exec = saved_exec;
    x.level = saved_level;

    return match;
}
