///
///  @file    back_cmd.c
///  @brief   Execute \ (backslash) command.
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

#include "teco.h"
#include "exec.h"

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

void scan_back(struct cmd *cmd)
{
    assert(cmd != NULL);

    int n;

    if (pop_expr(&n))                   // n\`?
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

        uint nbytes = (uint)snprintf(string, sizeof(string), format, n);

        assert(nbytes < sizeof(string));

        exec_insert(string, nbytes);
        push_expr(n, EXPR_VALUE);       // TODO: is this correct?
    }
    else
    {
        // TODO: read digit string in buffer using current radix.

        n = 0;

        push_expr(n, EXPR_VALUE);
    }
}
