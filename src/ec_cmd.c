///
///  @file    ec_cmd.c
///  @brief   Execute EC command.
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
#include "exec.h"
#include "file.h"
#include "page.h"


///
///  @brief    Close open input and output files.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void close_files(void)
{
    struct ofile *ofile = &ofiles[ostream];

    if (ofile->fp == NULL)
    {
        if (t.Z != 0)
        {
            throw(E_NFO);               // No output file open
        }
    }
    else
    {
        while (next_page(t.B, t.Z, f.ctrl_e, (bool)true))
        {
            ;
        }

        page_flush(ofile->fp);
    }

    set_page(0);

    exec_EF(NULL);                      // Rename and close the output file

    close_input(istream);
}


///
///  @brief    Execute EC command: copy input to output and close file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EC(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd->m_set);
    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    if (!cmd->n_set)
    {
        close_files();
    }
    else                                // nEC - set size of edit buffer
    {
        uint_t kbytes = (uint_t)cmd->n_arg;

        setsize_ebuf(kbytes * KB);
    }
}
