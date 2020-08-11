///
///  @file    eq_cmd.c
///  @brief   Execute EQ command.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include <unistd.h>
#include <sys/stat.h>

#include "teco.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "file.h"
#include "qreg.h"


///
///  @brief    Execute EQ command: read file into Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EQ(struct cmd *cmd)
{
    assert(cmd != NULL);

    const char *buf = cmd->text1.buf;
    uint len = cmd->text1.len;
    const int stream = IFILE_QREGISTER;

    if (len == 0)                       // If no file name, then done
    {
        return;
    }

    assert(buf != NULL);

    struct ifile *ifile = open_input(buf, len, stream, cmd->colon);

    // Note: open_input() only returns NULL for colon-modified command.

    if (ifile == NULL)
    {
        push_expr(0, EXPR_VALUE);

        return;
    }

    struct stat file_stat;

    if (stat(ifile->name, &file_stat))
    {
        throw(E_SYS, ifile->name);      // Unexpected system error
    }

    uint size = (uint)file_stat.st_size;
    struct buffer text =
    {
        .len  = size,
        .pos  = 0,
        .size = size,
    };

    text.buf = alloc_mem(size);

    if (fread(text.buf, 1uL, (ulong)size, ifile->fp) != size)
    {
        throw(E_SYS, ifile->name);      // Unexpected system error
    }

    store_qtext(cmd->qname, cmd->qlocal, &text);

    close_input(stream);

    if (cmd->colon)
    {
        push_expr(-1, EXPR_VALUE);
    }
}
