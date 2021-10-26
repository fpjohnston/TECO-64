///
///  @file    fk_cmd.c
///  @brief   Execute FK command.
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
#include "ascii.h"
#include "editbuf.h"
#include "eflags.h"
#include "estack.h"
#include "exec.h"
#include "search.h"


///
///  @brief    Execute FK command: forward search and delete.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_FK(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len != 0)
    {
        build_search(cmd->text1.data, cmd->text1.len);
    }

    struct search s;

    s.type       = SEARCH_S;
    s.search     = search_forward;
    s.count      = cmd->n_arg;
    s.text_start = 0;
    s.text_end   = t.Z - t.dot;

    if (search_loop(&s))
    {
        delete_ebuf(-s.text_pos);

        search_success(cmd);
    }
    else
    {
        search_failure(cmd, f.ed.keepdot);
    }
}


///
///  @brief    Scan FK command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_FK(struct cmd *cmd)
{
    assert(cmd != NULL);

    default_n(cmd, (int_t)1);           // FKtext` => 1FKtext`
    reject_neg_m(cmd->m_set, cmd->m_arg);
    reject_dcolon(cmd->dcolon);
    scan_texts(cmd, 1, ESC);

    return false;
}
