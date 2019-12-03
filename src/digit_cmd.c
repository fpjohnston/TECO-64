///
///  @file    digit_cmd.c
///  @brief   Process digits in a command string.
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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "errors.h"
#include "exec.h"



///
///  @brief    Scan digits in a command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_digits(struct cmd *cmd)
{
    assert(cmd != NULL);

    long sum = 0;
    int c = cmd->c1;

    // Here when we have a digit. Check to see that it's valid for the current
    // radix, and then loop until we run out of valid digits, at which point we
    // have to return the last character to the command buffer.
    
    while (valid_radix(c))
    {
        const char *digits = "0123456789ABCDEF";
        const char *digit = strchr(digits, toupper(c));

        assert(digit != NULL);

        long n = digit - digits;

        sum *= v.radix;
        sum += n;

        c = fetch_buf();                // Get next digit
    }

    unfetch_buf(c);                     // Return last (non-digit) character

    push_expr((int)sum, EXPR_VALUE);
}


///
///  @brief    Determine if character is valid in our current radix.
///
///  @returns  true if valid, false if not (if invalid octal digit, E_ILN).
///
////////////////////////////////////////////////////////////////////////////////

bool valid_radix(int c)
{
    if (v.radix == 16 && isxdigit(c))
    {
        return true;
    }
    else if (isdigit(c))
    {
        if (v.radix == 10 || c <= '7')
        {
            return true;
        }

        print_err(E_ILN);               // Illegal octal digit
    }

    return false;
}
