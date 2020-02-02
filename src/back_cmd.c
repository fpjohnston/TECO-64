///
///  @file    back_cmd.c
///  @brief   Execute \ (backslash) command.
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
#include "exec.h"
#include "textbuf.h"


///  @var    MAX_DIGITS
///  @brief  Maximum length of digit string. Note that this is big enough to
///          hold a 64-bit octal number.

#define MAX_DIGITS      22


///
///  @brief    Scan \ (backslash) command: read digit string.
///
///  @returns  nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_back(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->n_set)                     // n\`?
    {
        char string[MAX_DIGITS];
        const char *format = "%d";

        if (v.radix == 8)
        {
            format = "%o";
        }
        else if (v.radix == 16)
        {
            format = "%x";
        }

        uint nbytes = (uint)snprintf(string, sizeof(string), format, cmd->n_arg);

        assert(nbytes < sizeof(string));

        exec_insert(string, nbytes);

        last_len = nbytes;
    }
    else
    {
        bool minus = false;
        int pos = 0;
        uint digits = 0;
        int n = 0;
        int c = getchar_tbuf(pos++);

        if (c == '+')
        {
            c = getchar_tbuf(pos++);
        }
        else if (c == '-')
        {
            c = getchar_tbuf(pos++);
            minus = true;
        }

        while (c != -1)
        {
            const char *dstr = "0123456789abcdef";
            const char *dchar = strchr(dstr, tolower(c));

            if (dchar == NULL)
            {
                break;
            }

            int digit = dchar - dstr;

            if ((v.radix == 8 && digit >= 8) || (v.radix == 10 && digit >= 10))
            {
                break;
            }

            ++digits;

            n *= v.radix;
            n += digit;

            c = getchar_tbuf(pos++);
        }

        if (digits != 0)
        {
            if (minus)
            {
                n = -n;
            }

            setpos_tbuf(t.dot + pos - 1);
        }

        push_expr(n, EXPR_VALUE);
    }
}
