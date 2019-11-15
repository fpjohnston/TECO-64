///
///  @file    digit_cmd.c
///  @brief   Process digits in a command string.
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
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "errors.h"
#include "exec.h"

// Local functions

static bool valid_radix(int c);


///
///  @brief    Process digits in a command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_digit(void)
{
    long sum = 0;
    int c = last_c;

    // Here when we have a digit. Check to see that it's valid for the current
    // radix, and then loop until we run out of valid digits, at which point we
    // have to return the last character to the command buffer.

    while (valid_radix(c))
    {
        const char *digits = "0123456789ABCDEF";
        const char *digit = strchr(digits, toupper(c));

        assert(digit != NULL);

        long n = digit - digits;

        sum *= radix;
        sum += n;

        if ((c = fetch_cmd()) == EOF)
        {
            break;
        }
    }

    unfetch_cmd(c);

    push_expr(sum, EXPR_OPERAND);
}


///
///  @brief    Determine if character is valid in our current radix.
///
///  @returns  true if valid, false if not (if invalid octal digit, E_ILN).
///
////////////////////////////////////////////////////////////////////////////////

static bool valid_radix(int c)
{
    if (radix == 16 && isxdigit(c))
    {
        return true;                    // Valid hexadecimal digit
    }
    else if (isdigit(c))
    {
        if (radix == 10 || c <= '7')
        {
            return true;                // Valid decimal or octal digit
        }

        print_err(E_ILN);               // Illegal octal digit
    }

    return false;                       // Not a digit in current radix
}
