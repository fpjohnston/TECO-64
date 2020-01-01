///
///  @file    goto_cmd.c
///  @brief   Execute goto commands.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
///
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files (the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "errors.h"
#include "exec.h"


// Local functions

static void find_tag(struct cmd *cmd, const char *text, uint len);


///
///  @brief    Execute ! command: comment/tag. This function doesn't actually do
///            anything, but it exists to ensure that the command is properly
///            scanned, so that tags can be found with the O and nO commands.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_bang(struct cmd *unused1)
{

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
        prints_err(E_TAG, "\?\?\?");    // Missing tag
    }

    // Here if we have a tag

    if (!cmd->n_set)                    // Is it Otag` or nOtag1,tag,tag3`?
    {
        find_tag(cmd, cmd->text1.buf, cmd->text1.len);

        return;
    }

    // Here if the command was nO (computed goto).

    if (cmd->n_arg <= 0)                // Is value non-positive?
    {
        return;                         // Just ignore goto
    }

    // Parse the comma-separated list of tags, looking for the one we want.
    // If the nth tag is null, or is out of range, then we do nothing.

    char *taglist = alloc_mem(cmd->text1.len + 1);
    char *buf = taglist;
    char *saveptr;
    char *next;
    uint ntags = 0;

    sprintf(taglist, "%.*s", (int)cmd->text1.len, cmd->text1.buf);

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

            break;
        }
    }

    free_mem(&taglist);
}


///
///  @brief    Find a specific tag, checking for possible duplicates.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void find_tag(struct cmd *cmd, const char *text, uint len)
{
    assert(cmd != NULL);
    assert(text != NULL);    

    char *tag = alloc_mem(len + 1);
    uint nbytes = (uint)sprintf(tag, "%.*s", (int)len, text);

    int tag_pos = -1;                   // No tag found yet

    cmdbuf->pos = 0;                   // Start at beginning of command

    for (;;)
    {
        (void)next_cmd(cmd);

        if (cmdbuf->pos == cmdbuf->len) // End of command string?
        {
            if (tag_pos != -1)          // Did we find the tag?
            {
                break;
            }

            prints_err(E_TAG, tag);     // Missing tag
        }
        else if (cmd->c1 != '!')        // Is this a tag?
        {
            continue;                   // No
        }
        else if (tag_pos != -1)         // Have we seen it before?
        {
            prints_err(E_DUP, tag);     // Duplicate tag
        }
        else if (cmd->text1.len == nbytes &&
                 !memcmp(cmd->text1.buf, tag, (long)nbytes))
        {
            tag_pos = (int)cmdbuf->pos; // Remember tag for later
        }
    }

    free_mem(&tag);

    cmdbuf->pos = (uint)tag_pos;       // Execute goto
}
