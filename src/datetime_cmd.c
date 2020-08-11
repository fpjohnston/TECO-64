///
///  @file    datetime_cmd.c
///  @brief   Execute date and time commands.
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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "teco.h"
#include "eflags.h"
#include "estack.h"
#include "exec.h"


#define MINUTES_PER_HOUR        60      ///< Minutes per hour

#define SECONDS_PER_MINUTE      60      ///< Seconds per minute


///
///  @brief    Scan ^B (CTRL/B) command. We return the current date encoded
///            in the following way: ((year - 1900) * 16 + month) * 32 + day.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_B(struct cmd *cmd)
{
    assert(cmd != NULL);

    time_t t = time(NULL);
    struct tm tm;

    (void)localtime_r(&t, &tm);

    int teco_date = ((tm.tm_year) * 16 + tm.tm_mon+1) * 32 + tm.tm_mday;

    push_expr(teco_date, EXPR_VALUE);
}


///
///  @brief    Scan ^H (CTRL/H) command. This returns the current time as
///            milliseconds since midnight.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_H(struct cmd *cmd)
{
    assert(cmd != NULL);

    time_t t = time(NULL);
    struct tm tm;

    (void)localtime_r(&t, &tm);

    int teco_time = tm.tm_hour * MINUTES_PER_HOUR + tm.tm_min;

    teco_time *= SECONDS_PER_MINUTE;
    teco_time += tm.tm_sec;

    if (f.e1.msec)                      // Return time in milliseconds?
    {
        struct timeval tv = { .tv_usec = 0 };

        (void)gettimeofday(&tv, NULL);

        teco_time *= 1000;
        teco_time += (int)(tv.tv_usec / 1000);
    }
    else                                // RT-11, RSX-11, and VMS format
    {
        teco_time /= 2;                 // (seconds since midnight) / 2
    }

    push_expr(teco_time, EXPR_VALUE);
}

