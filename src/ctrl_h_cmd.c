///
///  @file    ctrl_h_cmd.c
///  @brief   Execute ^H (CTRL/H) command.
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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "teco.h"
#include "exec.h"


///
///  @brief    Execute ^H (CTRL/H) command. This returns the current time
///            as seconds since midnight divided by 2. The division is necessary
///            so that the result is no more than 16 bits.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#define MINUTES_PER_HOUR        60
#define SECONDS_PER_MINUTE      60

void exec_ctrl_h(struct cmd *cmd)
{
    assert(cmd != NULL);

    time_t t = time(NULL);
    struct tm tm;

    (void)localtime_r(&t, &tm);

    int teco_time = tm.tm_hour * MINUTES_PER_HOUR + tm.tm_min;

    teco_time *= SECONDS_PER_MINUTE;
    teco_time += tm.tm_sec;

    push_expr(teco_time / 2, EXPR_OPERAND);
}

