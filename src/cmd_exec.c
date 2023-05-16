///
///  @file    cmd_exec.c
///  @brief   Execute command string.
///
///  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "cbuf.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"

#include "_cmd_exec.c"              // Include command tables


#if     defined(INLINE)

#undef INLINE
#define INLINE  inline

#else

#define INLINE

#endif

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
    .text1  = { .data = NULL, .len = 0 },
    .text2  = { .data = NULL, .len = 0 },
};

uint_t cmd_line;                    ///< Line number in current command/macro


// Local functions

static INLINE bool scan_cmd(struct cmd *cmd);

static INLINE const struct cmd_table *scan_special(struct cmd *cmd, int type);

static void scan_text(int delim, tstring *text);


///
///  @brief    Test restrictions for TECO command syntax. Many of these are not
///            errors in classic TECO, and therefore are contingent on bits
///            being set in the E2 flag.
///
///  @returns  Nothing (will throw error if violation found).
///
////////////////////////////////////////////////////////////////////////////////

#if     !defined(NOSTRICT)

void confirm_cmd(struct cmd *cmd, ...)
{
    assert(cmd != NULL);

    va_list args;

    va_start(args, cmd);

    int c = va_arg(args, int);

    while (c != NO_EXIT)
    {
        switch (c)
        {
            case NO_ATSIGN: if (f.e2.atsign && cmd->atsign)   throw(E_ATS); break;
            case NO_COLON:  if (f.e2.colon && cmd->colon)     throw(E_COL); break;
            case NO_DCOLON: if (f.e2.colon && cmd->dcolon)    throw(E_COL); break;
            case NO_M:      if (f.e2.m_arg && cmd->m_set)     throw(E_IMA); break;
            case NO_M_ONLY: if (cmd->m_set && !cmd->n_set)    throw(E_NON); break;
            case NO_N:      if (f.e2.n_arg && cmd->n_set)     throw(E_INA); break;
            case NO_NEG_M:  if (cmd->m_set && cmd->m_arg < 0) throw(E_NCA); break;
            case NO_NEG_N:  if (cmd->n_set && cmd->n_arg < 0) throw(E_NCA); break;
            default:                                                        break;
        }

        c = va_arg(args, int);
    }

    va_end(args);
}

#endif


///
///  @brief    Set default value for n argument if needed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void default_n(struct cmd *cmd, int_t n_default)
{
    assert(cmd != NULL);

    if (!cmd->n_set)
    {
        cmd->n_set = true;
        cmd->n_arg = n_default;
    }
}


///
///  @brief    Execute command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);

    cmd_line = 1;                       // Start command at line 1

    int c;

    // Loop for all commands in command string.

    while (f.e0.exec && (c = fetch_cbuf()) != EOF)
    {
        cmd->c1 = (char)c;

        (void)scan_cmd(cmd);
    }

    if (f.e0.sigint)                    // Did we stop because of a CTRL/C?
    {
        throw(E_XAB);
    }

    // Here to make sure that all conditionals, loops, and parenthetical
    // expressions were complete within the command string just executed.

    if (getif_depth() != 0)
    {
        throw(E_MAP);                   // Missing apostrophe
    }
    else if (getloop_depth() != getloop_base())
    {
        throw(E_MRA);                   // Missing right angle bracket
    }
    else
    {
        confirm_parens();               // Make sure we used all parentheses
    }

    cbuf->pos = cbuf->len = 0;          // Reset for next command string
}


///
///  @brief    Execute command string. This is different from exec_cmd() in that
///            it is typically for strings constructed internally (that is, not
///            user input) for such purposes as mapping keycodes to TECO
///            commands. It constructs a buffer containing the command string
///            and then calls exec_macro(), which in turn calls cmd_exec().
///
///            See exec_key() for examples of use.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_str(const char *str)
{
    assert(str != NULL);
    assert(str[0] != NUL);

    bool exec = f.e0.exec;
    size_t nbytes = strlen(str);
    char text[nbytes + 1];
    tbuffer buf =
    {
        .data = text,
        .size = (uint_t)nbytes,
        .len  = 0,
        .pos  = 0,
    };

    buf.len = buf.size;

    strcpy(text, str);

    // The reason for the next line is that we can be called when we are
    // processing character input, such as an immediate-mode command. This
    // means that the execution flag isn't on, but we need to temporarily force
    // it in order to process a command string initiated by a special key such
    // as Page Up or Page Down.

    f.e0.exec = true;                   // Force execution

    exec_macro(&buf, NULL);

    f.e0.exec = exec;                   // Restore previous state
}


///
///  @brief    Scan command and see if we're finished with it.
///
///  @returns  true if command is complete, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool finish_cmd(struct cmd *cmd, int c)
{
    assert(cmd != NULL);

    cmd->c1 = (char)c;

    return scan_cmd(cmd);
}


///
///  @brief   Scan command.
///
///  Returns: true if we processed the end of the command, else false if we
///           need to continue scanning.
///
////////////////////////////////////////////////////////////////////////////////

static INLINE bool scan_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);

    int type;
    int c = cmd->c1;
    const struct cmd_table *entry;
    const struct cmd_table badcmd = { .scan=NULL, .exec=NULL, .type=c_none };

    if ((uint)c < countof(cmd_table))
    {
        entry = &cmd_table[c];
        type = entry->type;
    }
    else
    {
        entry = &badcmd;
        type = c_none;
    }

    if (entry->exec == NULL)
    {
        // No exec function. See if we have a scan function.

        if (entry->scan != NULL)
        {
            (void)(*entry->scan)(cmd);

            return false;
        }
        else if ((entry = scan_special(cmd, type)) == NULL)
        {
            return false;
        }
    }

    scan_x(cmd);

    if (entry->scan != NULL && (*entry->scan)(cmd))
    {
        return false;
    }
    else if (!f.e0.skip)
    {
        (*entry->exec)(cmd);            // Execute command

        // We normally reset the command block after every command we execute.
        // However, the '[', ']', and '!' commands through m and n arguments
        // unused. Also, a double ESCape in a macro passes through m and n
        // arguments, and 'M' commands allow m and n arguments to be returned
        // to the caller.

        if (entry->type == c_M && cmd->n_set)
        {
            store_val(cmd->n_arg);

            bool m_set = cmd->m_set;
            int_t m_arg = cmd->m_arg;

            *cmd = null_cmd;

            cmd->m_set = m_set;
            cmd->m_arg = m_arg;
        }
        else
        {
            *cmd = null_cmd;
        }
    }

    return true;
}


///
///  @brief   Scan special command. This includes E and F commands, as well
///           as ^x and ^^x commands, and whitespace characters.
///
///  Returns: Pointer to command table, or NULL if need to keep scanning.
///
////////////////////////////////////////////////////////////////////////////////

static INLINE const struct cmd_table *scan_special(struct cmd *cmd, int attr)
{
    assert(cmd != NULL);

    int c;

    switch (attr)
    {
        case c_WHITE:
            return NULL;

        case c_LF:
            if (cmd_line != 0)
            {
                ++cmd_line;
            }

            return NULL;

        case c_UP:                      // ^x and ^^x commands
            c = require_cbuf();

            if (c == '^')
            {
                scan_simple(cmd);

                c = require_cbuf();

                store_val((int_t)c);

                return NULL;
            }

            c = 1 + toupper(c) - 'A';   // Change ^x to equivalent control chr.

            if (c <= NUL || c >= SPACE)
            {
                throw(E_IUC, c);        // Invalid character following ^
            }

            cmd->c1 = (char)c;

            const struct cmd_table *entry = &cmd_table[c];

            if (entry->exec == NULL)
            {
                assert(entry->scan != NULL);

                (void)(*entry->scan)(cmd);

                return NULL;
            }

            return entry;

        case c_E:                       // E commands
            c = require_cbuf();

            if ((uint)c > countof(e_table) || (e_table[c].scan == NULL &&
                                               e_table[c].exec == NULL))
            {
                throw(E_IEC, c);        // Invalid E character
            }

            cmd->c2 = (char)c;

            return &e_table[c];

        case c_F:                       // F commands
            c = require_cbuf();

            if ((uint)c > countof(f_table) || (f_table[c].scan == NULL &&
                                               f_table[c].exec == NULL))
            {
                throw(E_IFC, c);        // Invalid F character
            }

            cmd->c2 = (char)c;

            return &f_table[c];

        case c_none:
        default:
            if (!f.e0.skip)
            {
                throw(E_ILL, cmd->c1); // Illegal command
            }
            else
            {
                return NULL;
            }
    }
}


///
///  @brief    Scan the text string following the command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void scan_text(int delim, tstring *text)
{
    assert(text != NULL);

    text->data = cbuf->data + cbuf->pos;

    size_t nbytes = cbuf->len - cbuf->pos;
    const char *start = text->data;
    const char *end = memchr(start, delim, (size_t)nbytes);
    const char *p;

    if (end == NULL)
    {
        throw(E_BALK);                  // Unexpected end of command or macro
    }

    text->len = (uint_t)(int_t)(end - start);

    // If we're counting lines, then count any LFs in the text string.

    if (cmd_line != 0)
    {
        p = start;

        for (int i = 0; i < (int)(end - start); ++i)
        {
            if (*p++ == LF)
            {
                ++cmd_line;
            }
        }

        if (*p == LF)                   // Is LF the delimiter (for !! tags)?
        {
            ++cmd_line;                 // Yes, so count that also
        }
    }

    // Echo text string or comment if tracing. In order to ensure that this will
    // work when executing an EM command, note that we must echo the LF that
    // terminates a comment; otherwise, what follows would then become part of
    // the comment.

    p = start;

    for (uint i = 0; i < text->len + 1; ++i)
    {
        trace_cbuf(*p++);
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

    // If the user specified the at-sign modifier, then skip any whitespace
    // between the command and the delimiter.

    if (cmd->atsign)                    // @ modifier?
    {
        int c;

        while ((c = peek_cbuf()) != TAB && isspace(c))
        {
            next_cbuf();                // Skip the whitespace character
        }

        c = require_cbuf();             // Get text string delimiter

        if (isgraph(c) || (c >= CTRL_A && c <= CTRL_Z))
        {
            delim = (char)c;
        }
        else
        {
            throw(E_TXT, c);            // Invalid text delimiter
        }
    }

    // If we are allowing paired text delimiters, then the first delimiter
    // cannot be a closing parenthesis, bracket, or brace.

    if (strchr(")>]}", delim) != NULL && f.e1.text)
    {
        throw(E_TXT, delim);            // Invalid text delimiter
    }

    if (strchr("(<[{", delim) == NULL || !f.e1.text)
    {
        scan_text(delim, &cmd->text1);

        if (ntexts == 2)
        {
            scan_text(delim, &cmd->text2);
        }

        return;
    }

    // Here if user wants to delimit text string(s) with paired parentheses,
    // brackets, or braces. This means the text strings may be of the form
    // (xxx), <xxx>, [xxx], or {xxx}, and may include leading or trailing
    // whitespace, allowing commands such as @S {foo}, @FS [foo] [baz],
    // @S<foobaz>, or @^A (foo). Note that if a command allows two text
    // arguments, the second must be delimited by the same character pair
    // as the first.

    const char *end = strchr("()<>[]{}", delim);

    assert(end != NULL);

    // Point to closing parenthesis, bracket, or brace

    ++end;

    scan_text(*end, &cmd->text1);

    if (ntexts != 2)
    {
        return;
    }

    // Skip any whitespace after ')', '>', ']', or '}'

    int c;

    while ((c = peek_cbuf()) != TAB && isspace(c))
    {
        next_cbuf();                    // Skip the whitespace character
    }

    c = require_cbuf();                 // Get text string delimiter

    if (c != delim)                     // Must be same delimiter
    {
        throw(E_TXT, c);                // Invalid text delimiter
    }

    scan_text(*end, &cmd->text2);
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

    bool match = false;                 // Assume failure
    bool trace = f.trace;
    int c;

    new_x();                           // Save current expression stack

    f.e0.skip = true;
    f.trace = false;

    while ((c = fetch_cbuf()) != EOF)
    {
        cmd->c1 = (char)c;

        if (scan_cmd(cmd))
        {
            if (strchr(skip, c) != NULL) // Is this the command we want?
            {
                match = true;

                break;
            }
            else
            {
                *cmd = null_cmd;
            }
        }
    }

    f.trace = trace;
    f.e0.skip = false;

    delete_x();                            // Restore previous expression stack

    return match;
}
