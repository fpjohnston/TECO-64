///
///  @file    o_cmd.c
///  @brief   Execute O command.
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

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "exec.h"

// TODO: do something better here.

static char tag[128 + 1];               ///< Last tag seen


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
        print_err(E_NTF);               // No tag found
    }

    // Here if we have a tag

    if (!cmd->n_set)                    // Is it Otag` or nOtag1,tag,tag3`?
    {
        sprintf(tag, "%.*s", (int)cmd->text1.len, cmd->text1.buf);

//        prints_err(E_TAG, tag);         // TODO: do something better here
    }

    // Here if the command was nO (computed goto).

    if (cmd->n_arg <= 0)                // Is value non-positive?
    {
        return;
//        print_err(E_NTF);               // No tag found
    }

    char *taglist = alloc_mem(cmd->text1.len + 1);
    char *buf = taglist;
    char *next;
    char *saveptr;
    uint ntags = 0;

    tag[0] = NUL;

    sprintf(taglist, "%.*s", (int)cmd->text1.len, cmd->text1.buf);

    // Find all tags, looking for the one matching the n argument, but
    // validating all regardless.

    while ((next = strtok_r(buf, ",", &saveptr)) != NULL)
    {
        buf = NULL;

        // TODO: validate tag

        if (++ntags == (uint)cmd->n_arg) // Is this the tag we want?
        {
            sprintf(tag, "%s", next);
        }
    }

    free_mem(&taglist);
    
    if (tag[0] == NUL)                  // Did we find the tag in the list?
    {
        print_err(E_NTF);               // No tag found
    }

//    prints_err(E_TAG, tag);             // TODO: do something better here
}

