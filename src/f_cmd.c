///
///  @file    f_cmd.c
///  @brief   Execute F commands.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
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
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "errors.h"
#include "exec.h"


///
///  @brief    Execute F commands.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

const struct cmd_table *exec_F(struct cmd *cmd)
{
    assert(cmd != NULL);

    const char *f_cmds = "'23<>BCDKLNRSU_|";

    int c = fetch_cbuf(NOSTART);

    const char *f_cmd = strchr(f_cmds, toupper(c));

    if (f_cmd == NULL)
    {
        throw(E_IFC, c);                // Illegal F character
    }

    cmd->c2 = (char)c;

    uint idx = (uint)(f_cmd - f_cmds);

    assert(idx < f_count);

    return &f_table[idx];
}

