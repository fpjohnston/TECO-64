///
///  @file    number_cmd.c
///  @brief   Scan numeric digit commands.
///
///  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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

#include "teco.h"
#include "cmdbuf.h"
#include "editbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "estack.h"
#include "exec.h"


///  @var    MAX_DIGITS
///  @brief  Maximum length of digit string. Note that this is big enough to
///          hold a 64-bit octal number.

#define MAX_DIGITS      22


///
///  @brief    Execute \ command: read digit string.
///
///  @returns  nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_back(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_M, NO_COLON, NO_DCOLON, NO_ATSIGN);

    if (cmd->n_set)                     // n\`?
    {
        char string[MAX_DIGITS + 1];
        const char *format = "%d";

        if (f.radix == 8)
        {
            format = "%o";
        }
        else if (f.radix == 16)
        {
            format = "%x";
        }

        int nbytes = snprintf(string, sizeof(string), format, cmd->n_arg);

        // Error if string was truncated

        assert(nbytes < (int)sizeof(string));

        exec_insert(string, (uint_t)(uint)nbytes);

        last_len = (uint_t)(uint)nbytes;
    }
    else
    {
        bool minus = false;
        int_t pos = 0;
        uint ndigits = 0;
        int_t n = 0;
        int c = read_edit(pos++);

        if (c == '+')
        {
            c = read_edit(pos++);
        }
        else if (c == '-')
        {
            c = read_edit(pos++);
            minus = true;
        }

        while (c != EOF)
        {
            if (isdigit(c))
            {
                c -= '0';              // Remove ASCII bias
            }
            else if (isxdigit(c))
            {
                c = 10 + toupper(c) - 'A'; // Change [A,F] to [10,15].
            }
            else
            {
                break;
            }

            if ((f.radix == 8 && c >= 8) || (f.radix == 10 && c >= 10))
            {
                break;
            }

            ++ndigits;

            n *= f.radix;
            n += c;

            c = read_edit(pos++);
        }

        if (ndigits != 0)
        {
            if (minus)
            {
                n = -n;
            }

            set_dot(t->dot + pos - 1);
        }

        store_val(n);
    }
}


///
///  @brief    Scan a number in a command string, which can be decimal or octal,
///            depending on the current radix.
///
///            If f.e1.radix is set, and the number was specified within
///            parentheses, then we allow the radix to be specified per
///            C conventions:
///
///            If it starts with 1-9, it's a decimal number.
///
///            Else if it starts with 0x or 0X, it's a hexadecimal number.
///
///            Else it's an octal number (which starts with 0).
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_number(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    int c = cmd->c1;
    int_t radix;
    int_t n = 0;

    if (!f.e1.radix || !check_parens()) // Auto-detect radix?
    {
        radix = f.radix;                // No - use default
    }
    else if (c != '0')                  // First digit 0?
    {
        radix = 10;                     // No, must be base 10
    }
    else if (toupper(peek_cbuf()) == 'X') // Was it 0x or 0X for hex number?
    {
        next_cbuf();                    // Discard the x or X

        c = require_cbuf();             // Get the first digit for base 16

        if (!isxdigit(c))               // Hexadecimal digit?
        {
            throw(E_ILN);               // Invalid number
        }
        else if (isalpha(c))             // [A-F]?
        {
            c = '9' + 1 + toupper(c) - 'A'; // Convert to [10,15]
        }

        radix = 16;
    }
    else                                // Must be base 8
    {
        radix = 8;
    }

    //  Note that we just got a digit. We will process all consecutive digits,
    //  but will stop if we see anything else. So if the digit flag is set when
    //  we get here, it means that we saw a digit after other characters, such
    //  whitespace (e.g., "12 34") or perhaps colons or at-signs (e.g., 12:34).
    //  We will allow this since classic TECO does, but if numbers should be
    //  in "canonical" form, we will issue an error.

#if     !defined(NOSTRICT)

    if (f.e0.digit)
    {
        if (f.e2.number)
        {
            throw(E_ILN);               // Invalid number
        }
        else if (query_x(&n))           // Recover previous number
        {
            n *= radix;                 // And shift it over
        }
        else                            // If nothing there,
        {
            n = 0;                      //  just do the best we can
        }
    }

#endif

    n += c - '0';                       // Convert ASCII digit to binary and save

    while ((c = peek_cbuf()) != EOF)
    {
        if (isdigit(c))                 // Octal, decimal, or hexadecimal digit
        {
            if (radix == 8 && c >= '8') // Valid octal digit?
            {
                throw(E_ILN);           // Invalid octal number
            }
        }
        else if (radix == 16 && isxdigit(c)) // [A,F]?
        {
            c = '9' + 1 + toupper(c) - 'A';  // Convert to [10,15]
        }
        else
        {
            break;                      // No more valid digits for radix
        }

        next_cbuf();                    // Accept the next digit

        n *= radix;                     // Shift over existing digits
        n += c - '0';                   // Convert ASCII digit to binary and save
    }

    store_val(n);

#if     !defined(NOSTRICT)

    f.e0.digit = true;                  // Set digit flag (AFTER calling store_val)

#endif

    return true;
}
