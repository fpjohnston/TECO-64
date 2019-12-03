///
///  @file    eh_cmd.c
///  @brief   Process TECO EH command.
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "eflags.h"
#include "exec.h"


///
///  @brief    Execute EH command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EH(struct cmd *cmd)
{
    assert(cmd != NULL);

    f.eh.flag = set_flag(cmd, f.eh.flag);

    uint verbose = f.eh.verbose;
    uint command = f.eh.command;

    f.eh.flag = 0;

    f.eh.command = command;
    f.eh.verbose = verbose;
}


///
///  @brief    Scan EH command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_EH(struct cmd *cmd)
{
    assert(cmd != NULL);

    get_flag(cmd, f.eh.flag);
}
