///
///  @file    ubar_cmd.c
///  @brief   Execute _ (underscore) and F_ commands.
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
#include <stdio.h>

#include "teco.h"
#include "ascii.h"
#include "editbuf.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "search.h"


// Local functions

static void exec_search(struct cmd *cmd, bool replace);


///
///  @brief    Execute F_ command: search and replace with yank protection.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_under(struct cmd *cmd)
{
    exec_search(cmd, (bool)true);
}


///
///  @brief    Execute _ command: search with yank protection.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_under(struct cmd *cmd)
{
    exec_search(cmd, (bool)false);
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

    if (cmd->n_set && cmd->n_arg == 0)  // 0_text` isn't allowed
    {
        throw(E_ISA);                   // Invalid search argument
    }

    if (!cmd->n_set)                    // _text` => 1_text`
    {
        cmd->n_arg = 1;
        cmd->n_set = true;
    }

    if (cmd->text1.len != 0)
    {
        build_search(cmd->text1.data, cmd->text1.len);
    }

    struct search s;

    if (cmd->n_arg < 0)
    {
        s.type       = SEARCH_U;
        s.search     = search_backward;
        s.count      = -cmd->n_arg;
        s.text_start = -1;              // Start at previous character
        s.text_end   = -t->dot;
    }
    else
    {
        s.type       = SEARCH_U;
        s.search     = search_forward;
        s.count      = cmd->n_arg;
        s.text_start = 0;               // Start at current character
        s.text_end   = t->Z - t->dot;
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
        search_failure(cmd, f.ed.keepdot);
    }
}


///
///  @brief    Scan F_ command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_F_under(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_DCOLON);

    default_n(cmd, (int_t)1);           // F_ => 1F_
    scan_texts(cmd, 2, ESC);

    return false;
}


///
///  @brief    Scan _ command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_under(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_M, NO_DCOLON);

    default_n(cmd, (int_t)1);           // _ => 1_
    scan_texts(cmd, 1, ESC);

    return false;
}
