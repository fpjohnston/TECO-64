///
///  @file    ec_cmd.c
///  @brief   Execute EC command.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
///
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files (the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "textbuf.h"
#include "exec.h"


///
///  @brief    Execute EC command: copy input to output and close file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EC(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->n_set)                     // nEC?
    {
        if (cmd->n_arg > 0)
        {
            (void)setsize_tbuf((uint)cmd->n_arg);
        }

        return;
    }

    FILE *fp;
    struct ifile *ifile = &ifiles[istream];
    struct ofile *ofile = &ofiles[ostream];

    // Note that we call size_edit() for every call to next_page() because the
    // size of the edit buffer can change every time we read in another page.

    if ((fp = ofile->fp) != NULL)
    {
        while (next_page(B, getsize_tbuf(), v.ff, (bool)true))
        {
            ;
        }

        fclose(fp);

        ofile->fp = NULL;
    }

    rename_output(ofile);              // Handle any required file renaming

    free_mem(&ofile->temp);
    free_mem(&ofile->name);
    
    if ((fp = ifile->fp) != NULL)
    {
        fclose(fp);

        ifile->fp = NULL;
    }

    ifile->eof = true;
    ifile->cr  = false;
}


///
///  @brief    Scan nEC command: reserved for TECO-10, so error.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_EC(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (operand_expr())                 // Was it nEC?
    {
        cmd->n_set = true;
        cmd->n_arg = get_n_arg();
    }

    scan_state = SCAN_DONE;
}
