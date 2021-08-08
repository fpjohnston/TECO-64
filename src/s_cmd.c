///
///  @file    s_cmd.c
///  @brief   Execute S command.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "editbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "search.h"
#include "term.h"


// Local functions

static void exec_search(struct cmd *cmd, bool replace);


///
///  @brief    Execute "S" command: local search.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_S(struct cmd *cmd)
{
    exec_search(cmd, (bool)false);
}


///
///  @brief    Execute "FS" command: local search and replace.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_FS(struct cmd *cmd)
{
    exec_search(cmd, (bool)true);
}


///
///  @brief    Execute search and replace.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_search(struct cmd *cmd, bool replace)
{
    assert(cmd != NULL);

    if (cmd->n_set && cmd->n_arg == 0)  // 0Stext` isn't allowed
    {
        throw(E_ISA);                   // Invalid search argument
    }

    if (!replace && cmd->dcolon)        // ::Stext` => 1,1:Stext`
    {
        cmd->m_arg = cmd->n_arg = 1;
        cmd->m_set = cmd->n_set = true;
    }
    else if (!cmd->n_set)               // Stext` => 1Stext`
    {
        cmd->n_arg = 1;
        cmd->n_set = true;
    }

    if (cmd->text1.len != 0)
    {
        last_search = build_string(cmd->text1.data, cmd->text1.len);
    }
    else if (last_search.len == 0)
    {
        throw(E_SRH, "");               // Nothing to search for
    }

    struct search s;

    s.type = SEARCH_S;

    if (cmd->dcolon)
    {
        s.search     = search_forward;
        s.count      = 1;
        s.text_start = 0;               // Start at current character
        s.text_end   = 0;
    }
    else if (cmd->n_arg < 0)
    {
        s.search     = search_backward;
        s.count      = -cmd->n_arg;
        s.text_start = -1;              // Start at previous character
        s.text_end   = -t.dot;

        if (cmd->m_set && cmd->m_arg != 0)
        {
            int_t nbytes = -cmd->m_arg + 1;

            if (s.text_end < s.text_start + nbytes)
            {
                s.text_end = s.text_start + nbytes;
            }
        }
    }
    else
    {
        s.search     = search_forward;
        s.count      = cmd->n_arg;
        s.text_start = 0;
        s.text_end   = t.Z - t.dot;

        if (cmd->m_set && cmd->m_arg != 0)
        {
            int_t nbytes = cmd->m_arg - 1;

            if (s.text_end > s.text_start + nbytes)
            {
                s.text_end = s.text_start + nbytes;
            }
        }
    }

    if (search_loop(&s))
    {
        if (replace)
        {
            delete_ebuf(-(int_t)last_len);

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
        search_failure(cmd);
    }
}
