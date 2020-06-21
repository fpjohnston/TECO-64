///
///  @file    f1_cmd.c
///  @brief   Execute F1 command.
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
#include "ascii.h"
#include "eflags.h"
#include "exec.h"
#include "term.h"


///
///  @brief    Execute F1 command: set options for writing files.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F1(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len == 0)
    {
        f.e2.noin   = true;
        f.e2.noout  = true;

        return;
    }

    // Parse the slash-separated list of options.

    char *optlist = alloc_mem(cmd->text1.len + 1);
    char *buf = optlist;
    char *saveptr;
    char *option;

    sprintf(optlist, "%.*s", (int)cmd->text1.len, cmd->text1.buf);

    // Find all options.

    while ((option = strtok_r(buf, "/", &saveptr)) != NULL)
    {
        buf = NULL;

        uint len = (uint)strlen(option);
        char *p = option + len - 1;

        while (len-- > 0 && isspace(*p))
        {
            *p-- = NUL;
        }

        if (len)
        {
            if (!strcasecmp(option, "noin"))
            {
                f.e2.noin = true;
            }
            else if (!strcasecmp(option, "noout"))
            {
                f.e2.noout = true;
            }
            else
            {
                print_str("%%Skipping invalid option \"/%s\"\r\n", option);
            }
        }
    }

    free_mem(&optlist);
}
