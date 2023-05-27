///
///  @file    esc_cmd.c
///  @brief   Execute ESCape commands.
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
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "cmdbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "estack.h"
#include "errors.h"
#include "exec.h"


// Local functions

static bool double_escape(void);


///
///  @brief    Check to see if we have a double ESCape.
///
///  @returns  true if double escape, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool double_escape(void)
{
    if (cbuf->pos < cbuf->len)          // Any more characters?
    {
        if (cbuf->data[cbuf->pos] == ESC)
        {
            return true;                // We have <ESC><ESC>
        }
        else if (cbuf->pos < cbuf->len - 1
                 && !memcmp(&cbuf->data[cbuf->pos], "^[", (size_t)2))
        {
            return true;                // We have <ESC>^[
        }
    }

    //  Here for a single ESCape, either at the end of the command string,
    //  or followed by a non-ESCape.

    return false;
}


///
///  @brief    Execute ESCape command. We're called here only for ESCapes
///            between commands, or at the end of command strings, not for
///            ESCapes used to delimit text arguments after commands.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_escape(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    // If we're ending this command string, stop processing any more commands.
    // If we're also in a macro, return to caller, preserving numeric arguments.

    if (double_escape())
    {
        cbuf->pos = cbuf->len;          // Move to end of command string

        if (check_macro())
        {
            return;
        }
    }

    cmd->m_set = cmd->n_set = false;
    cmd->m_arg = cmd->n_arg = 0;

    if (check_parens())
    {
        throw(E_MRP);                   // Missing right parenthesis
    }

    init_x();                           // Reinitialize expression stack
}


///
///  @brief    Execute no-op command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_nop(struct cmd *unused)
{
}
