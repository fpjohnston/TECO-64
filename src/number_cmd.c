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

#include "teco.h"
#include "cbuf.h"
#include "editbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "term.h"


///  @var    MAX_DIGITS
///  @brief  Maximum length of digit string. Note that this is big enough to
///          hold a 64-bit octal number.

#define MAX_DIGITS      22

///  @var    digits
///  @brief  Convert ASCII character to hex digit.

static const char digits[] =
{
    ['0'] = 0,
    ['1'] = 1,
    ['2'] = 2,
    ['3'] = 3,
    ['4'] = 4,
    ['5'] = 5,
    ['6'] = 6,
    ['7'] = 7,
    ['8'] = 8,
    ['9'] = 9,
    ['A'] = 10,
    ['a'] = 10,
    ['B'] = 11,
    ['b'] = 11,
    ['C'] = 12,
    ['c'] = 12,
    ['D'] = 13,
    ['d'] = 13,
    ['E'] = 14,
    ['e'] = 14,
    ['F'] = 15,
    ['f'] = 15,
};


///
///  @brief    Execute \ command: read digit string.
///
///  @returns  nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_bslash(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd->m_set);
    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    if (cmd->n_set)                     // n\`?
    {
        char string[MAX_DIGITS];
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
            if (!isxdigit(c))
            {
                break;
            }

            int digit = digits[c];

            if ((f.radix == 8 && digit >= 8) || (f.radix == 10 && digit >= 10))
            {
                break;
            }

            ++ndigits;

            n *= f.radix;
            n += digit;

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
///            If f.e1.radix is set, then we allow the radix to be specified in
///            string, per C conventions:
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

    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    int c = cmd->c1;
    int cx;
    int_t radix;
    bool hex = false;                   // Assume hex digits are disallowed

    if (!f.e1.radix || nparens == 0)    // Auto-detect radix?
    {
        radix = f.radix;                // No - use default
    }
    else if (c != '0')                  // First digit 0?
    {
        radix = 10;                     // No, must be base 10
    }
    else if ((cx = peek_cbuf()) != EOF && (cx == 'x' || cx == 'X'))
    {
        next_cbuf();                    // Discard the x or X
        trace_cbuf(c);

        c = require_cbuf();             // Get the first digit for base 16

        if (!isxdigit(c))               // At least one valid hex digit?
        {
            throw(E_ILN);               // Invalid number
        }

        trace_cbuf(c);

        radix = 16;

        hex = true;                     // We can allow hex digits
    }
    else                                // Must be base 8
    {
        radix = 8;
    }

    int_t n = digits[c];                // Store 1st digit

    while ((c = peek_cbuf()) != EOF && c >= '0')
    {
        if (radix == 10)
        {
            if (c > '9')
            {
                break;                  // Not a decimal digit
            }
        }
        else if (radix == 16)
        {
            if (!isxdigit(c))
            {
                break;                  // Not a hex digit
            }
            else if (!isdigit(c) && !hex)
            {
                break;
            }
        }
        else if (c > '7')               // Must be octal
        {
            if (c > '9')                // Non-digit?
            {
                break;                  // Not an octal digit
            }

            throw(E_ILN);               // Invalid octal number
        }

        next_cbuf();                    // Accept the next digit
        trace_cbuf(c);

        n *= radix;                     // Shift over existing digits
        n += digits[c];                 // And add in the new digit
    }

    store_val(n);

    return true;
}
