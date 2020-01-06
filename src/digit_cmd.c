///
///  @file    digit_cmd.c
///  @brief   Execute digit commands.
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
#include <string.h>

#include "teco.h"
#include "errors.h"
#include "exec.h"


///
///  @brief    Scan digits in a command string. These can be decimal or octal,
///            depending on the current radix.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_digit(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c = cmd->c1;

    if (v.radix == 8 && c > '7')        // If base 8 and digit is 8 or 9
    {
        print_err(E_ILN);               // Illegal number
    }

    if (!scan.digits)                   // If first digit,
    {
        scan.sum = 0;                   //  then start at zero
    }

    scan.digits = true;
    
    scan.sum *= (int)v.radix;           // Shift over existing digits
    scan.sum += c - '0';                // And add in the new one
}
