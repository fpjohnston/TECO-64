///
///  @file    goto_cmd.c
///  @brief   Execute goto commands.
///
///  @copyright 2019-2022 Franklin P. Johnston / Nowwith Treble Software
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

static void find_tag(const char *tag);

static void find_taglist(const char *taglist, int arg);

static bool validate_tag(tstring *tag);


///
///  @brief    Execute ! command: comment/tag. This function doesn't actually
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
}


///
///  @brief    Execute O command: goto and computed goto.
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

    sprintf(tag, "%.*s", (int)cmd->text1.len, cmd->text1.data);

    if (cmd->n_set)                     // Computed GOTO
    {
        find_taglist(tag, (int)cmd->n_arg);
    }
    else if (strchr(tag, ',') != NULL)  // Tag cannot contain a comma
    {
        throw(E_BAT, tag);              // Bad tag
    }
    else                                // Simple GOTO
    {
        find_tag(tag);
    }
}


///
///  @brief    Find a specific tag, checking for possible duplicates.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void find_tag(const char *orig_tag)
{
    assert(orig_tag != NULL);           // Error if no tag string

    // Check for string building characters to create tag

    tstring tag = build_string(orig_tag, (uint_t)strlen(orig_tag));

    if (!validate_tag(&tag))
    {
        throw(E_BAT, tag.data);         // Bad tag
    }

    struct cmd cmd = null_cmd;          // Dummy command block for skip_cmd()
    uint_t loop_start = getloop_start(); // Start of current loop (0 if none)
    uint_t loop_end = (uint_t)EOF;      // End of current loop
    uint loop_depth = 0;                // Initial loop depth
    uint if_depth = 0;                  // Current if/else depth
    uint_t tag_pos = 0;                 // Position of tag
    uint_t tag_line = 0;                // Line number for tag
    uint tag_loop = 0;                  // Loop depth for tag
    uint tag_if = 0;                    // If depth for tag
    uint_t saved_line = cmd_line;       // Save current line number

    cmd_line = 1;                       // Start command at line 1
    cbuf->pos = 0;                      // Start at beginning of command

    // Scan entire command string to verify that we have
    // one and only one instance of the specified tag.

    while (cbuf->pos < cbuf->len && skip_cmd(&cmd, "!\"'<>"))
    {
        switch (cmd.c1)
        {
            case '"':                   // Start of conditional
                ++if_depth;

                break;

            case '\'':                  // End of conditional
                --if_depth;

                break;

            case '<':                   // Start of loop
                ++loop_depth;

                break;

            case '>':                   // End of loop
                --loop_depth;

                // Check for end of loop the O command may be in

                if (loop_start != 0 && loop_end == (uint_t)EOF)
                {
                    loop_end = cbuf->pos;
                }

                break;

            case '!':                   // Start of tag/comment
                if (cmd.c2 != '!'
                    && cmd.text1.len == tag.len
                    && !memcmp(cmd.text1.data, tag.data, (size_t)tag.len))
                {
                    // Error if duplicate tag

                    if (tag_pos != 0)
                    {
                        cmd_line = saved_line;  // Restore original line number

                        throw(E_DUP, tag.data); // Duplicate tag
                    }

                    // Error if jumping into a conditional

                    if (if_depth != 0)
                    {
                        cmd_line = saved_line;  // Restore original line number

                        throw(E_LOC, tag.data); // Invalid location
                    }

                    // Error if jumping into a loop (other than current loop).

                    if (loop_depth != 0
                        && (cbuf->pos < loop_start || cbuf->pos > loop_end))
                    {
                        cmd_line = saved_line;  // Restore original line number

                        throw(E_LOC, tag.data); // Invalid location
                    }

                    // We found the tag, so print it if tracing.

                    if (f.trace.enable)
                    {
                        tprint("!%.*s!", (int)cmd.text1.len, cmd.text1.data);
                    }

                    // Save state for tag in case we decide to use it

                    tag_pos  = cbuf->pos;
                    tag_line = cmd_line;
                    tag_loop = loop_depth + getloop_depth();
                    tag_if   = if_depth;
                }

                break;

            default:
                break;
        }
    }

    if (tag_pos == 0)                   // Did we find the tag?
    {
        cmd_line = saved_line;          // Restore original line number

        throw(E_TAG, tag.data);         // Missing tag
    }

    init_x();                           // Reinitialize expression stack

    setloop_depth(tag_loop);
    setif_depth(tag_if);

    cmd_line = tag_line;                // Use the tag's line number
    cbuf->pos = tag_pos;                // Execute goto
}


///
///  @brief    Search a taglist for a specific tag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void find_taglist(const char *taglist, int arg)
{
    assert(taglist != NULL);

    // Here to handle a computed GOTO.

    const char *next = taglist;
    int ntags = 0;

    // Find all tags, looking for the one matching the n argument.

    while (++ntags <= arg && *next != NUL)
    {
        uint_t len = (uint_t)strcspn(next, ",");
        char tag[len + 1];

        snprintf(tag, sizeof(tag), "%.*s", (int)len, next);

        if (ntags == arg)
        {
            if (len != 0)
            {
                find_tag(tag);
            }

            break;
        }

        next += len;

        if (*next == ',')
        {
            ++next;
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

    reject_colon(cmd->colon);

    if (f.e1.xoper && nparens != 0)     // Check for ! operator
    {
        reject_m(cmd->m_set);
        reject_n(cmd->n_set);
        reject_atsign(cmd->atsign);

        push_x((int_t)0, X_NOT);

        return true;
    }

    require_n(cmd->m_set, cmd->n_set);

    // If feature enabled, !! starts a comment that ends with LF
    // (but note that the LF is not counted as part of the command).

    int c;

    if ((c = peek_cbuf()) == '!')
    {
        if (!f.e1.bang)                 // Is !! enabled?
        {
            throw(E_EXT);
        }

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
///  @brief    Scan O command.
////
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_O(struct cmd *cmd)
{
    assert(cmd != NULL);
    reject_neg_n(cmd->n_set, cmd->n_arg);
    require_n(cmd->m_set, cmd->n_set);
    reject_colon(cmd->colon);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Validate tag, skipping any leading or trailing spaces.
///
///  @returns  true if valid tag, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool validate_tag(tstring *tag)
{
    assert(tag != NULL);

    // Trim leading spaces

    while (*tag->data != NUL && *tag->data == ' ')
    {
        ++tag->data;
        --tag->len;
    }

    // Trim trailing spaces

    while (tag->data[tag->len - 1] == ' ')
    {
        tag->data[--tag->len] = NUL;
    }

    // Verify that tag does not contain any commas (since these are used for
    // computed GOTOs), nor any control characters. Note that this restriction
    // only applies to tags used in O commands, not the tags themselves (which
    // may actually be comments).

    const char *p = tag->data;
    int c;

    while ((c = *p++) != NUL)
    {
        if (iscntrl(c) || c == ',')
        {
            return false;
        }
    }

    return true;
}
