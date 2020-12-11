///
///  @file    goto_cmd.c
///  @brief   Execute goto commands.
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

    if (cmd->n_set)                     // Pass through m and n arguments
    {
        if (cmd->m_set)
        {
            push_x(cmd->m_arg, X_OPERAND);
        }

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

    snprintf(tag, sizeof(tag), "%.*s", (int)cmd->text1.len,
             cmd->text1.data);

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

static void find_tag(struct cmd *cmd, const char *tag)
{
    assert(cmd != NULL);
    assert(tag != NULL);                // Error if no tag string

    int tag_pos = -1;                   // Position of tag in command string

    // The following creates a tag using string building characters, but then
    // sets up a local copy so that we can free up the string that was allocated
    // by build_string(). This is to avoid memory leaks in the event of errors.

    char *tag1 = NULL;                  // Dynamically-allocated tag name
    uint len = build_string(&tag1, tag, (uint)strlen(tag));
    char tag2[len + 1];                 // Local copy of tag name

    strcpy(tag2, tag1);

    free_mem(&tag1);

    cbuf->pos = 0;                      // Start at beginning of command

    // Scan entire command string to verify that we have
    // one and only one instance of the specified tag.

    while (cbuf->pos < cbuf->len)
    {
        if (!skip_cmd(cmd, "!"))
        {
            break;                      // No more commands, so quit
        }

        assert(cmd->c1 == '!');

        // If tracing is enabled, print tag (unless it's a comment)

        if (f.trace.enable && cmd->c2 != '!')
        {
            tprint("!%.*s!", cmd->text1.len, cmd->text1.data);
        }

        if (cmd->text1.len == len && !memcmp(cmd->text1.data, tag2, (ulong)len))
        {
            if (tag_pos != -1)          // Found tag. Have we seen it already?
            {
                throw(E_DUP, tag2);     // Duplicate tag
            }

            tag_pos = (int)cbuf->pos;   // Remember tag for later
        }
    }

    if (tag_pos == -1)                  // Did we find the tag?
    {
        throw(E_TAG, tag2);             // Missing tag
    }

    cbuf->pos = (uint)tag_pos;          // Execute goto
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

        push_x(0, X_NOT);

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
