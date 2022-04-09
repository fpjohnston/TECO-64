///
///  @file    fb_cmd.c
///  @brief   Execute FB and FC commands.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "editbuf.h"
#include "eflags.h"
#include "estack.h"
#include "exec.h"
#include "search.h"


// Local functions

static void exec_search(struct cmd *cmd, bool replace);


///
///  @brief    Execute FB command: bounded search.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_FB(struct cmd *cmd)
{
    exec_search(cmd, (bool)false);
}


///
///  @brief    Execute FC command: bounded search and replace.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_FC(struct cmd *cmd)
{
    exec_search(cmd, (bool)true);
}


///
///  @brief    Execute bounded search (and maybe replace).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_search(struct cmd *cmd, bool replace)
{
    assert(cmd != NULL);

    if (cmd->text1.len != 0)
    {
        build_search(cmd->text1.data, cmd->text1.len);
    }

    struct search s;

    s.type  = SEARCH_S;
    s.count = 1;

    if (cmd->m_set)
    {
        bool reverse = (cmd->m_arg > cmd->n_arg);

        s.search     = reverse ? search_backward : search_forward;
        s.text_start = cmd->m_arg - t->dot;
        s.text_end   = cmd->n_arg - t->dot;
    }
    else if (cmd->n_arg <= 0)
    {
        s.search     = search_backward;
        s.text_start = -1;
        s.text_end   = len_edit(cmd->n_arg);
    }
    else
    {
        s.search     = search_forward;
        s.text_start = 0;
        s.text_end   = len_edit(cmd->n_arg);
    }

    if (search_loop(&s))
    {
        if (replace)
        {
            delete_edit(-(int_t)last_len);

            if (cmd->text2.len)
            {
                exec_insert(cmd->text2.data, cmd->text2.len);
            }
        }
        else
        {
            print_flag(f.es);
        }

        search_success(cmd);
     }
    else
    {
        search_failure(cmd, true);
    }
}


///
///  @brief    Scan FB command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_FB(struct cmd *cmd)
{
    assert(cmd != NULL);

    default_n(cmd, (int_t)1);           // FB => 1FB
    reject_neg_m(cmd->m_set, cmd->m_arg);
    reject_dcolon(cmd->dcolon);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Scan FC command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_FC(struct cmd *cmd)
{
    assert(cmd != NULL);

    default_n(cmd, (int_t)1);           // FC => 1FC
    reject_neg_m(cmd->m_set, cmd->m_arg);
    reject_dcolon(cmd->dcolon);
    scan_texts(cmd, 2, ESC);

    return false;
}
