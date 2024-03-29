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
#include "cmdbuf.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "term.h"

#include "_cmd_exec.c"              // Include command tables


#if     defined(INLINE)

#undef INLINE
#define INLINE  inline              ///< Enable inline functions

#else

#define INLINE                      ///< Disable inline functions

#endif

///  @var    ctrl
///
///  @brief  Initial command block values.

struct ctrl ctrl =
{
    .depth = 0,
    .level = 0,
    .loop = { { 0 } },
};

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
    .colon  = false,
    .dcolon = false,
    .atsign = false,
    .keep   = false,
    .final  = false,
    .text1  = { .data = NULL, .len = 0 },
    .text2  = { .data = NULL, .len = 0 },
};

uint_t cmd_line;                    ///< Line number in current command/macro


// Local functions

static INLINE void scan_cmd(struct cmd *cmd);

static INLINE const struct cmd_table *scan_special(struct cmd *cmd);

static void scan_text(int delim, tstring *text);


///
///  @brief    Test restrictions for TECO command syntax. Many of these are not
///            errors in classic TECO, and therefore are contingent on bits
///            being set in the E2 flag.
///
///  @returns  Nothing (will throw error if violation found).
///
////////////////////////////////////////////////////////////////////////////////

#if     !defined(NSTRICT)

void confirm_cmd(struct cmd *cmd, ...)
{
    assert(cmd != NULL);

    va_list args;

    va_start(args, cmd);

    int c = va_arg(args, int);
    bool mn_args = (cmd->m_set && cmd->n_set);

    while (c != END_LIST)
    {
        switch (c)
        {
            case NO_ATSIGN: if (f.e2.atsign && cmd->atsign)   throw(E_ATS); break;
            case NO_COLON:  if (f.e2.colon && cmd->colon)     throw(E_COL); break;
            case NO_DCOLON: if (f.e2.colon && cmd->dcolon)    throw(E_COL); break;
            case NO_M:      if (f.e2.m_arg && mn_args)        throw(E_IMA); break;
            case NO_M_ONLY: if (cmd->m_set && !cmd->n_set)    throw(E_NON); break;
            case NO_N:      if (f.e2.n_arg && cmd->n_set)     throw(E_INA); break;
            case NO_NEG_M:  if (cmd->m_set && cmd->m_arg < 0) throw(E_NCA); break;
            case NO_NEG_N:  if (cmd->n_set && cmd->n_arg < 0) throw(E_NCA); break;
            default:                                                        break;
        }

        c = va_arg(args, int);          // Get next item
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

        scan_cmd(cmd);
    }

    if (f.e0.sigint)                    // Did we stop because of a CTRL/C?
    {
        throw(E_XAB);
    }

    // Here to make sure that all conditionals, loops, and parenthetical
    // expressions were complete within the command string just executed.

    if (ctrl.depth != 0)
    {
        throw(E_MAP);                   // Missing apostrophe
    }
    else if (ctrl.level != 0)
    {
        throw(E_MRA);                   // Missing right angle bracket
    }
    else if (check_parens())
    {
        throw(E_MRP);                   // Missing right parenthesis
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

    scan_cmd(cmd);

    return cmd->final;
}


///
///  @brief   Scan command.
///
///  Returns: Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static INLINE void scan_cmd(struct cmd *cmd)
{
    static const struct cmd_table badcmd = { .scan=NULL, .exec=exec_bad };

    assert(cmd != NULL);

    int c = cmd->c1;
    const struct cmd_table *entry;

    if ((uint)c < countof(cmd_table))
    {
        entry = &cmd_table[c];
    }
    else
    {
        entry = &badcmd;
    }

    //  We should have either a scan function, or an exec function, or both. If
    //  we have a scan function, then call it, and return to the caller if we're
    //  done with the current command. Otherwise, check for 2-character commands
    //  (^, E, and F), and repeat the check for a scan function. If we haven't
    //  yet returned, then we must have a terminal character for the command, so
    //  process it.

    if (entry->scan != NULL)
    {
        if ((*entry->scan)(cmd))
        {
            return;
        }
        else if (entry->exec == NULL)
        {
            if ((entry = scan_special(cmd)) == NULL)
            {
                return;
            }
            else if (entry->scan != NULL)
            {
                if ((*entry->scan)(cmd))
                {
                    return;
                }
            }
        }
    }

    if (f.e0.skip)                      // All done if skipping command
    {
        cmd->final = true;              // Say that we saw last chr. in command

        return;
    }

    assert(entry->exec != NULL);

    (*entry->exec)(cmd);                // Execute command

#if     !defined(NSTRICT)

    f.e0.digit = false;

#endif

    //  We normally reset the entire command block after every executed command,
    //  but some commands do not use their numeric arguments and just pass them
    //  to the next command:
    //
    //      [q - Push Q-register.
    //      ]q - Pop Q-register.
    //      '  - End of conditional.
    //      !  - Tag. This allows comments to be used between commands, one of
    //           which generates numeric arguments, and the other which uses
    //           them.
    //
    //  Also, M commands pass through any numeric arguments to the Q-register
    //  being executed, and a double ESCape within a macro can return numeric
    //  arguments which will then be passed to the next command following the M.

    if (cmd->keep && cmd->n_set)        // Are we retaining m & n arguments?
    {
        store_val(cmd->n_arg);          // Put n back on expression stack

        bool m_set = cmd->m_set;        // Save m argument (if any)
        int_t m_arg = cmd->m_arg;

        *cmd = null_cmd;

        cmd->m_set = m_set;             // Restore any m argument
        cmd->m_arg = m_arg;
    }
    else
    {
        *cmd = null_cmd;
    }
}


///
///  @brief   Scan special command. This includes E and F commands, as well
///           as ^x and ^^x commands, and whitespace characters.
///
///  Returns: Pointer to command table, or NULL if need to keep scanning.
///
////////////////////////////////////////////////////////////////////////////////

static INLINE const struct cmd_table *scan_special(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c;

    switch (cmd->c1)
    {
        case '^':                       // ^x and ^^x commands
            c = require_cbuf();

            if (c == '^')
            {
                scan_x(cmd);
                confirm(cmd, NO_M, NO_N, NO_COLON, NO_DCOLON, NO_ATSIGN);

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
                if (entry->scan == NULL)
                {
                    throw(E_ILL, cmd->c1); // Illegal command
                }
                else
                {
                    (void)(*entry->scan)(cmd);

                    return NULL;
                }
            }

            return entry;

        case 'E':                       // E commands
        case 'e':                       // E commands
            c = require_cbuf();

            if ((uint)c > countof(e_table) || (e_table[c].scan == NULL &&
                                               e_table[c].exec == NULL))
            {
                throw(E_IEC, c);        // Invalid E character
            }

            cmd->c2 = (char)c;

            return &e_table[c];

        case 'F':                       // F commands
        case 'f':                       // f commands
            c = require_cbuf();

            if ((uint)c > countof(f_table) || (f_table[c].scan == NULL &&
                                               f_table[c].exec == NULL))
            {
                throw(E_IFC, c);        // Invalid F character
            }

            cmd->c2 = (char)c;

            return &f_table[c];

        default:
            return NULL;
    }
}


///
///  @brief    Scan the text string following the command. Note that the
///            delimiter passed to us is the closing delimiter for the string,
///            which is usually, but not always, the same as the opening
///            delimiter.
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
    uint_t len, tail = 1;

    if (end != NULL)
    {
        len = (uint)(end - start);

        // If we're counting lines, then count any LFs in the text string.

        if (cmd_line != 0)
        {
            const char *p = start;

            for (uint i = 0; i < len; ++i)
            {
                if (*p++ == LF)
                {
                    ++cmd_line;
                }
            }

            if (delim == LF)            // Is LF the delimiter (for !! tags)?
            {
                ++cmd_line;             // Yes, so count that also
            }
        }
    }
    else if (delim == LF)               // Processing a one-line comment?
    {
        end = memchr(start, ESC, (size_t)nbytes);

        if (end != NULL)                // Did we find an ESCape?
        {
            len = (uint)(end - start);
        }
        else                            // No, just consume remainder of line
        {
            len = (uint)nbytes;
        }

        tail = 0;                       // No closing delimiter
    }
    else
    {
        throw(E_BALK);                  // Unexpected end of command or macro
    }

#if     !defined(NTRACE)

    // If tracing, echo remainder of text string, including the closing delimiter.

    if (f.trace)
    {
        const char *p = start;

        for (uint i = 0; i < len + tail; ++i)
        {
            echo_in(*p++);
        }
    }

#endif

    cbuf->pos += len + tail;
    text->len = len;
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
        cmd->atsign = false;

        int c;

        while ((c = peek_cbuf()) != TAB && isspace(c))
        {
            next_cbuf();                // Skip the whitespace character
        }

        c = require_cbuf();             // Get text string delimiter

        //  The n@O/list/ command expects a comma-separated list, so a comma
        //  as a delimiter for the text string isn't allowed.

        if (c == ',' && toupper(cmd->c1) == 'O')
        {
            throw(E_TXT, c);            // Invalid text delimiter
        }
        else if (isgraph(c) || (c >= CTRL_A && c <= CTRL_Z))
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

#if     !defined(NSTRICT)

        f.e0.digit = false;

#endif

        cmd->c1 = (char)c;

        scan_cmd(cmd);

        if (cmd->final)
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

    delete_x();                         // Restore previous expression stack

    return match;
}
