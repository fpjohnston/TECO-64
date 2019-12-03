///
///  @file    eg_cmd.c
///  @brief   Execute EG command.
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
#include <unistd.h>

#include "teco.h"
#include "exec.h"

char *eg_command;                       ///< EG command to be executed.


///
///  @brief    Execute EG command: execute system command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EG(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->colon_set)
    {
        // TODO: TBD

        return;
    }

    // TODO: issue error E_NFO if there is text in the buffer,
    //       but there is no output file open.

    exec_EC(cmd);

    // EG`, not :EG`, so get ready to exit

    eg_command = alloc_mem(cmd->text1.len + 1);

    sprintf(eg_command, "%.*s", (int)cmd->text1.len, cmd->text1.buf);

    exit(EXIT_SUCCESS);
}


///
///  @brief    Initialize check that will be called to see if we need to execute
///            an EG command before we exit. We do it this way to ensure that
///            we've handled all possible cleanup.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_EG(void)
{
    if (atexit(exit_EG) != 0)
    {
        exit(EXIT_FAILURE);
    }
}

