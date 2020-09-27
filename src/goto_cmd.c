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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"


// Local functions

static void find_tag(struct cmd *cmd, const char *text, uint len);


///
///  @brief    Execute ! command: comment/tag. This function doesn't actually do
///            anything, but it exists to ensure that the command is properly
///            scanned, so that tags can be found with the O and nO commands.
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
    assert(cmd != NULL);                // Error if no command block

    if (cmd->n_set)                     // Pass through m and n arguments
    {
        if (cmd->m_set)
        {
            push_expr(cmd->m_arg, EXPR_VALUE);
        }

        push_expr(cmd->n_arg, EXPR_VALUE);
    }
}


///
///  @brief    Execute O command: goto and computed goto.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_O(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->text1.len == 0)            // Is there a tag?
    {
        throw(E_NOT);                   // O command has no tag
    }

    // Here if we have a tag

    if (!cmd->n_set)                    // Is it Otag` or nOtag1,tag,tag3`?
    {
        find_tag(cmd, cmd->text1.data, cmd->text1.len);

        return;
    }

    // Here if the command was nO (computed goto).

    if (cmd->n_arg <= 0)                // Is value non-positive?
    {
        throw(E_IOA);                   // Illegal O argument
    }

    // Parse the comma-separated list of tags, looking for the one we want.
    // If the nth tag is null, or is out of range, then we do nothing.

    char taglist[cmd->text1.len + 1];
    char *buf = taglist;
    char *saveptr;
    char *next;
    uint ntags = 0;

    snprintf(taglist, sizeof(taglist), "%.*s", (int)cmd->text1.len,
             cmd->text1.data);

    // Find all tags, looking for the one matching the n argument.

    while ((next = strtok_r(buf, ",", &saveptr)) != NULL)
    {
        buf = NULL;

        if (++ntags == (uint)cmd->n_arg) // Is this the tag we want?
        {
            uint len = (uint)strlen(next);

            if (len != 0)
            {
                find_tag(cmd, next, len);
            }

            return;
        }
    }

    // Here if O argument is out of range of tag list

    throw(E_BOA);                       // O argument is out of range
}


///
///  @brief    Find a specific tag, checking for possible duplicates.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void find_tag(struct cmd *cmd, const char *text, uint len)
{
    assert(cmd != NULL);                // Error if no command block
    assert(text != NULL);               // Error if no tag string

    int tag_pos = -1;                   // Position of tag in command string

    // The following creates a tag using string building characters, but then
    // sets up a local copy so that we can free up the string that was allocated
    // by build_string(). This is to avoid memory leaks.

    char *tag1 = NULL;                  // Dynamically-allocated tag name

    (void)build_string(&tag1, text, len);

    len = (uint)strlen(tag1);

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
