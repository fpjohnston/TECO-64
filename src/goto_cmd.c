///
///  @file    goto_cmd.c
///  @brief   Execute goto commands.
///
///  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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
#include "cbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "file.h"
#include "term.h"


// Local functions

static void find_tag(struct cmd *cmd, const char *tag);

static void find_taglist(struct cmd *cmd, const char *taglist);

static void verify_tag(const char *tag);


///
///  @brief    Execute "!" command: comment/tag. This function doesn't actually
///            do anything, but it exists to ensure that the command is properly
///            parsed, so that tags can be found with the O and nO commands.
///
///            Note that we pass through any numeric arguments, much like the [
///            and ] commands. This allows comments to be inserted between two
///            commands, the second of which uses the arguments resulting from
///            the first, such as the following:
///
///            :ERfoo$                     ! Open input file !
///               "U :@^A/?No file/ EX '   ! Print message and exit if error !
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_bang(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->n_set)                     // Pass through n argument
    {
        push_x(cmd->n_arg, X_OPERAND);
    }
}


///
///  @brief    Execute "O" command: goto and computed goto.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_O(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len == 0)            // Is there a tag?
    {
        throw(E_NOT);                   // O command has no tag
    }

    char tag[cmd->text1.len + 1];

    snprintf(tag, sizeof(tag), "%.*s", (int)cmd->text1.len, cmd->text1.data);

    verify_tag(tag);

    // Here when we have a valid tag or taglist, consisting only
    // of printable characters (including spaces).

    if (cmd->n_set)                     // Computed GOTO
    {
        find_taglist(cmd, tag);
    }
    else                                // Simple GOTO
    {
        if (strchr(tag, ',') != NULL)
        {
            throw(E_BAT, tag);
        }

        find_tag(cmd, tag);
    }
}


///
///  @brief    Find a specific tag, checking for possible duplicates.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void find_tag(struct cmd *cmd, const char *orig_tag)
{
    assert(cmd != NULL);
    assert(orig_tag != NULL);           // Error if no tag string

    // The following creates a tag using string building characters, but then
    // sets up a local copy so that we can free up the string that was allocated
    // by build_string(). This is to avoid memory leaks in the event of errors.

    tstring string = build_string(orig_tag, (uint_t)strlen(orig_tag));
    char tag[string.len + 1];           ///< Local copy of tag name

    strcpy(tag, string.data);

    struct
    {
        uint_t loop_start;              ///< Start of current loop (or 0)
        uint_t loop_end;                ///< End of current loop (or 0)
        uint loop_depth;                ///< Current loop depth
        uint if_depth;                  ///< Current if/else depth
        uint_t tag_pos;                 ///< Position of tag
        uint tag_loop;                  ///< Loop depth for tag
        uint tag_if;                    ///< If depth for tag
    } state =
    {
        .loop_start   = 0,
        .loop_end     = (uint_t)EOF,
        .loop_depth   = 0,
        .if_depth     = 0,
        .tag_loop     = 0,
        .tag_if       = 0,
    };


    state.loop_start = getloop_start();

    cbuf->pos = 0;                      // Start at beginning of command

    // Scan entire command string to verify that we have
    // one and only one instance of the specified tag.

    while (cbuf->pos < cbuf->len)
    {
        if (!skip_cmd(cmd, "!\"'<>"))
        {
            break;                      // No more commands, so quit
        }
        else if (cmd->c1 == '"')
        {
            ++state.if_depth;
        }
        else if (cmd->c1 == '\'')
        {
            --state.if_depth;
        }
        else if (cmd->c1 == '<')        // Start of a new loop?
        {
            ++state.loop_depth;
        }
        else if (cmd->c1 == '>')        // End of a loop?
        {
            --state.loop_depth;

            if (state.loop_start != (uint_t)EOF
                && cbuf->pos >= state.loop_start
                && state.loop_end == (uint_t)EOF)
            {
                state.loop_end = cbuf->pos;
            }
        }
        else if (cmd->c1 != '!' || cmd->c2 == '!')
        {
            continue;
        }

        if (cmd->text1.len != string.len
            || memcmp(cmd->text1.data, tag, (size_t)string.len))
        {
            continue;                   // Tag didn't match
        }

        // Tag matched. Make sure we're not branching into the middle of a loop
        // (unless it's within the loop the O command is in).

        if (state.loop_depth != 0)
        {
            if (state.loop_start == (uint_t)EOF || cbuf->pos < state.loop_start
                || (state.loop_end != (uint_t)EOF && cbuf->pos > state.loop_end))
            {
                throw(E_BAT, tag);      // Bad tag
            }
        }

        if (state.if_depth != 0)
        {
            throw(E_BAT, tag);          // Bad tag
        }

        if (state.tag_pos != 0)         // Found tag. Have we seen it already?
        {
            throw(E_DUP, tag);          // Duplicate tag
        }

        // We found the tag, so print it if tracing.

        if (f.trace.enable)
        {
            tprint("!%.*s!", (int)cmd->text1.len, cmd->text1.data);
        }

        state.tag_pos = cbuf->pos;      // Remember tag for later
        state.tag_loop = state.loop_depth;
        state.tag_if = state.if_depth;
    }

    if (state.tag_pos == (uint_t)EOF)   // Did we find the tag?
    {
        throw(E_TAG, tag);              // Missing tag
    }

    init_x();                           // Reinitialize expression stack

    setloop_depth(state.tag_loop);

    setif_depth(state.tag_if);

    cbuf->pos = state.tag_pos;          // Execute goto
}


///
///  @brief    Search a taglist for a specific tag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void find_taglist(struct cmd *cmd, const char *taglist)
{
    assert(cmd != NULL);
    assert(taglist != NULL);

    // Here to handle a computed GOTO.

    const char *start = taglist, *end;
    int ntags = 0;

    // Find all tags, looking for the one matching the n argument.

    while (++ntags <= cmd->n_arg)
    {
        start += strspn(start, " ");    // Skip leading whitespace
        end = start + strcspn(start, " ,"); // Find end of tag

        char tag[(end - start) + 1];

        snprintf(tag, sizeof(tag), "%.*s", (int)(end - start), start);

        if (ntags == cmd->n_arg)
        {
            find_tag(cmd, tag);

            break;
        }

        start = end + strspn(end, " "); // Skip trailing whitespace

        if (*start == ',')
        {
            ++start;
        }
        else if (*start == NUL)         // This catches embedded spaces
        {
            break;
        }
        else
        {
            throw(E_BAT, tag);          // Bad tag
        }
    }
}


///
///  @brief    Scan ! command with format "m,n@X/text1/" (with special
///            delimiters).
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_bang(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_colon(cmd);

    if (f.e1.xoper && nparens != 0)     // Check for ! operator
    {
        reject_m(cmd);
        reject_n(cmd);
        reject_colon(cmd);
        reject_atsign(cmd);

        push_x((int_t)0, X_NOT);

        return true;
    }

    require_n(cmd);

    // If feature enabled, !! starts a comment that ends with LF
    // (but note that the LF is not counted as part of the command).

    int c;

    if (f.e1.bang && (c = peek_cbuf()) == '!')
    {
        next_cbuf();
        trace_cbuf(c);

        scan_texts(cmd, 1, LF);

        --cmd->text1.len;               // Back off the LF
        cmd->c2 = '!';                  // And flag it as a comment
    }
    else
    {
        scan_texts(cmd, 1, '!');
    }

    return false;
}


///
///  @brief    Verify that tag (or tag list) contains no control characters.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void verify_tag(const char *tag)
{
    assert(tag != NULL);

    int c;
    const char *p = tag;

    while ((c = *p++) != NUL)
    {
        if (iscntrl(c))
        {
            throw(E_BAT, tag);          // Bad tag
        }
    }
}
