///
///  @file    ez_cmd.c
///  @brief   Execute EZ command.
///
///  @copyright 2021 Franklin P. Johnston / Nowwith Treble Software
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
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "editbuf.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "file.h"


#define EZ_SIZE         (KB * 4)        ///< Allocate memory in 4 KB chunks

tstring ez = { .data = NULL, .len = 0 }; ///< Output from EZ command


///
///  @brief    Execute "EZ" command: execute system command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EZ(struct cmd *cmd)
{
    assert(cmd != NULL);

    char syscmd[PATH_MAX];              // System command, and input buffer
    FILE *fp;                           //< File descriptor for pipe
    size_t size;                        //< Total no. of bytes read

    if (cmd->text1.len == 0)            // Any command string?
    {
        return;                         // No
    }

    tstring buf = build_string(cmd->text1.data, cmd->text1.len);

    int nbytes = snprintf(syscmd, sizeof(syscmd), "%s 2>&1", buf.data);

    assert(nbytes > 0);

    if (nbytes >= (int)sizeof(syscmd))
    {
        throw(E_CMD);                   // System command is too long
    }

    if ((fp = popen(syscmd, "r")) == NULL)
    {
        if (cmd->colon)
        {
            push_x(FAILURE, X_OPERAND);

            return;
        }

        throw(E_ERR, syscmd);           // General error
    }

    ez.len = EZ_SIZE;
    ez.data = alloc_mem(ez.len);

    uint_t pos = 0;

    while ((size = fread(ez.data + pos, 1uL, (size_t)EZ_SIZE, fp)) > 0)
    {
        if (size + pos == ez.len)
        {
            ez.data = expand_mem(ez.data, ez.len, EZ_SIZE);
            ez.len += EZ_SIZE;
        }

        pos += (uint_t)size;
    }

    if (ferror(fp) || pclose(fp) == -1)
    {
        if (cmd->colon)
        {
            push_x(FAILURE, X_OPERAND);

            return;
        }

        throw(E_ERR, syscmd);           // General error
    }

    ez.len = pos;                       // Length = total bytes read

    if (cmd->colon)
    {
        push_x(SUCCESS, X_OPERAND);
    }

    return;
}

