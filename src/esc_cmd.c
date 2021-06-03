///
///  @file    esc_cmd.c
///  @brief   Execute ESCape commands.
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

#include "teco.h"
#include "cbuf.h"
#include "exec.h"


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

    // If no more characters in command string, then reset for next time.

    if (cbuf->pos == cbuf->len)
    {
        cbuf->pos = cbuf->len = 0;
    }
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
