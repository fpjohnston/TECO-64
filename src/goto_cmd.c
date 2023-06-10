///
///  @file    goto_cmd.c
///  @brief   Execute goto commands.
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
#include <stdio.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "cmdbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "estack.h"
#include "exec.h"


// Local functions

static void find_tag(const char *text, uint len);

static void validate_tag(tstring *tag);


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

void exec_tag(struct cmd *cmd)
{
    assert(cmd != NULL);

    cmd->keep = true;                   // Say we need to retain m & n args.
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

    if (cmd->n_set)                     // Computed GOTO
    {
        // Look for the nth tag in the comma-separated list

        char list[cmd->text1.len + 1];
        char *tag = list;
        char *token;

        sprintf(tag, "%.*s", (int)cmd->text1.len, cmd->text1.data);

        while ((token = strsep(&tag, ",")) != NULL)
        {
            if (--cmd->n_arg == 0)
            {
                if (token[0] != NUL)
                {
                    find_tag(token, (uint)strlen(token));
                }

                break;
            }
        }
    }
    else                                // Simple GOTO
    {
        find_tag(cmd->text1.data, cmd->text1.len);
    }
}


///
///  @brief    Find a specific tag, checking for possible duplicates.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void find_tag(const char *text, uint len)
{
    assert(text != NULL);               // Error if no tag name

    // Check for string building characters to create tag

    tstring tag = build_string(text, len);
    struct cmd cmd;                     // Dummy command block for skip_cmd()
    uint old_pos = cbuf->pos;           // Save command buffer position
    uint_t old_line = cmd_line;         // Save current line number
    uint tag_pos = 0;                   // Position of tag
    uint level = 0;                     // Loop command level
    uint depth = 0;                     // Conditional command depth

    validate_tag(&tag);                 // Trim spaces and verify format

    //  Start at the beginning of the command string, and search for the tag.

    cbuf->pos = 0;

    while (skip_cmd(&cmd, "!"))
    {
        switch (cmd.c1)
        {
            case '"':                   // 'if' command
                ++depth;

                break;

            case '\'':                  // 'endif' command
                --depth;

                break;

            case '!':                   // Tag (label or comment)
                if (cmd.c2 != '!' && cmd.text1.len == tag.len
                    && !memcmp(cmd.text1.data, tag.data, (size_t)tag.len))
                {
                    if (tag_pos != 0)
                    {
                        cmd_line = old_line; // Restore line number for throw()

                        throw(E_DUP, tag.data); // Duplicate tag
                    }
                    else
                    {
                        tag_pos = cbuf->pos;
                    }
                }

                break;

            default:
                break;
        }
    }

    //  Issue error if we couldn't find the tag, or if we're in a loop and the
    //  tag was found the start of the loop. The reason for the latter error is
    //  because we cannot easily ensure that we wouldn't be jumping into a loop
    //  that we are not currently inside of. 

    if (tag_pos == 0)
    {
        cmd_line = old_line;            // Restore line number for throw()

        throw(E_TAG, tag.data);         // Missing tag
    }
    else if (ctrl.level != 0 && tag_pos < ctrl.loop[ctrl.level - 1].pos)
    {
        cmd_line = old_line;            // Restore line number for throw()

        throw(E_LOC, tag.data);         // Invalid tag location
    }

    //  Now that we know that we have a tag, and where it is, we can determine
    //  how to proceed. There are basically three possibilities:
    //
    //  - The tag is after the O command, so we don't need to adjust the depth
    //    of the conditional statements, or the line number.
    //  - The tag is before the O command, and:
    //    - we are not in any loop, so we just start at the beginning of the
    //      command string.
    //    - we are in a loop, or nested loop, so we cannot start before the
    //      start of the innermost loop.

    if (old_pos < tag_pos)
    {
        cbuf->pos = old_pos;
    }
    else
    {
        ctrl.depth = depth;

        if (ctrl.level == 0)
        {
            cmd_line = 1;
            cbuf->pos = 0;
        }
        else
        {
            cmd_line = ctrl.loop[ctrl.level - 1].line;
            cbuf->pos = ctrl.loop[ctrl.level - 1].pos;
        }
    }

    //  Scan command string to find the tag again. Note: when scanning for
    //  loops, we distinguish between the loop we may be enclosed by, and
    //  any new loops that we may find preceding our position, in order that
    //  we not jump into the middle of any loops. We also check that we don't
    //  jump into the middle of any loops following our position.

    while (skip_cmd(&cmd, "<>\"'!"))
    {
        switch (cmd.c1)
        {
            case '<':                   // Start of loop
                ++level;

                break;

            case '>':                   // End of loop
                if (level != 0)
                {
                    --level;
                }
                else if (ctrl.level != 0) // Exiting current (nested) loop?
                {
                    --ctrl.level;       // Yes
                }
                else
                {
                    throw(E_BNI);       // Right angle bracket not in iteration
                }

                break;

            case '"':                   // 'if' command
                ++ctrl.depth;

                break;

            case '\'':                  // 'endif' command
                --ctrl.depth;

                break;

            case '!':                   // Start of tag
                if (cbuf->pos == tag_pos && level == 0)
                {
                    // The +2 is for the delimiting exclamation marks.

                    cbuf->pos -= tag.len + 2;

                    reset_x();

                    return;
                }

                break;

            default:
                break;
        }
    }

    //  Here if trying to jump into the middle of a loop (other than ours).

    cmd_line = old_line;                // Restore line number for throw()

    throw(E_LOC, tag.data);             // Invalid tag location
}


///
///  @brief    Scan ! command with format "m,n@X/text1/" (with special
///            delimiters).
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_tag(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (f.e1.xoper && check_parens())   // Is it a logical NOT operator?
    {
        confirm(cmd, NO_M, NO_N, NO_COLON, NO_DCOLON, NO_ATSIGN);

        store_oper(X_NOT);

        return true;
    }

    // Here if we have either a GOTO label or a comment.

    scan_x(cmd);
    confirm(cmd, NO_M_ONLY, NO_COLON, NO_DCOLON, NO_ATSIGN);

    // If feature enabled, !! starts a comment that ends with LF
    // (but note that the LF is not counted as part of the command).

    if (peek_cbuf() == '!')
    {
        next_cbuf();

        if (!f.e1.bang)                 // Is !! enabled?
        {
            throw(E_EXT);
        }

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

    scan_x(cmd);
    confirm(cmd, NO_NEG_N, NO_M_ONLY, NO_COLON, NO_DCOLON);

    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Validate tag, by first trimming any leading or trailing spaces,
///            and then confirming that what remains consists only of graphic
///            characters other than commas or exclamation marks.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void validate_tag(tstring *tag)
{
    assert(tag != NULL);
    assert(tag->data != NULL);

    // Trim leading spaces

    uint len = (uint)strspn(tag->data, " ");

    tag->data += len;
    tag->len -= len;

    // Trim trailing spaces

    while (tag->len != 0 && tag->data[tag->len - 1] == ' ')
    {
        tag->data[--tag->len] = NUL;
    }

    if (tag->len == 0)
    {
        throw(E_NOT);                   // No tag
    }

    const char *p = tag->data;

    //  Verify that tag does not contain any commas (since those are used for
    //  computed GOTOs), nor any exclamation marks (since those are used to
    //  delimit tags), nor any control characters. Note that this restriction
    //  only applies to tags used in O commands, not the tags themselves (which
    //  may actually be comments).

    for (uint i = 0; i < tag->len; ++i)
    {
        int c = *p++;

        if (!isgraph(c) || c == ',' || c == '!')
        {
            throw(E_BAT, tag->data);    // Bad tag
        }
    }
}
