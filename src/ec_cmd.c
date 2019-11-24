///
///  @file    ec_cmd.c
///  @brief   Execute EC command.
///
///  @author  Nowwith Treble Software
///
///  @bug     No known bugs.
///
///  @copyright  tbd
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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
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

    FILE *fp;
    struct ifile *ifile = &ifiles[istream];
    struct ofile *ofile = &ofiles[ostream];
    
    if ((fp = ifile->fp) != NULL)
    {
        fclose(fp);

        ifile->fp = NULL;
    }

    ifile->eof = true;
    ifile->cr  = false;

    if ((fp = ofile->fp) != NULL)
    {
        fclose(fp);

        ofile->fp = NULL;
    }

    if (ofile->temp != NULL)
    {
        assert(ofile->name != NULL);

        if (ofile->backup)
        {
            sprintf(scratch, "%s~", ofile->name);

            if (rename(ofile->name, scratch) != 0)
            {
                fatal_err(errno, E_SYS, NULL);
            }
//            printf("renamed %s to %s\r\n", ofile->name, scratch);
        }
        else
        {
            if (remove(ofile->name) != 0)
            {
                fatal_err(errno, E_SYS, NULL);
            }
        }
        
        if (rename(ofile->temp, ofile->name) != 0)
        {
            fatal_err(errno, E_SYS, NULL);
        }
//        printf("renamed %s to %s\r\n", ofile->temp, ofile->name);

        dealloc(&ofile->temp);
    }
    fflush(stdout);

    dealloc(&ofile->name);

    ofile->backup = false;
}
