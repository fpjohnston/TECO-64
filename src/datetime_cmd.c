///
///  @file    datetime_cmd.c
///  @brief   Scan date and time commands.
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include <time.h>
#include <sys/time.h>

#include "teco.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"


#define MINUTES_PER_HOUR        60      ///< Minutes per hour

#define SECONDS_PER_MINUTE      60      ///< Seconds per minute


///
///  @brief    Scan "^B" (CTRL/B): return current date encoded as follows:
///
///            ((year - 1900) * 16 + month) * 32 + day
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_B(struct cmd *cmd)
{
    assert(cmd != NULL);

    check_n_arg(cmd);
    check_colon(cmd);
    check_atsign(cmd);

    time_t t = time(NULL);
    struct tm tm;

    (void)localtime_r(&t, &tm);

    int n = ((tm.tm_year) * 16 + tm.tm_mon + 1) * 32 + tm.tm_mday;

    push_expr(n, EXPR_VALUE);

    return true;
}


///
///  @brief    Scan "^H" (CTRL/H): return current time as milliseconds since
///            midnight.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_H(struct cmd *cmd)
{
    assert(cmd != NULL);

    check_n_arg(cmd);
    check_colon(cmd);
    check_atsign(cmd);

    time_t t = time(NULL);
    struct tm tm;
    struct timeval tv = { .tv_usec = 0 };

    (void)localtime_r(&t, &tm);
    (void)gettimeofday(&tv, NULL);

    int n = tm.tm_hour * MINUTES_PER_HOUR + tm.tm_min;

    n *= SECONDS_PER_MINUTE;
    n += tm.tm_sec;
    n *= 1000;
    n += (int)(tv.tv_usec / 1000);

    push_expr(n, EXPR_VALUE);

    return true;
}
