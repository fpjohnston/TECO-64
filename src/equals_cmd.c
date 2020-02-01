///
///  @file    equals_cmd.c
///  @brief   Execute "=" command.
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
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


// Local functions

static bool check_format(const char *format);


///
///  @brief    Execute "=", "==", and "===" commands (and with ":" modifiers).
///
///             n=     Display n in decimal, + CRLF
///             n==    Display n in octal, + CRLF
///             n===   Display n in hexadecimal, + CRLF
///             n:=    Display n in decimal, no CRLF
///             n:==   Display n in octal, no CRLF
///             n:===  Display n in hexadecimal, no CRLF
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_equals(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (f.e0.dryrun)
    {
        return;
    }

    if (!cmd->n_set)                    // If no operand, then complain
    {
        print_err(E_NAE);               // No argument before =
    }

    int c;
    const char *mode = "%d";

    if ((c = fetch_buf(NOCMD_START)) != '=')
    {
        unfetch_buf(c);
    }
    else if ((c = fetch_buf(NOCMD_START)) != '=')
    {
        unfetch_buf(c);

        cmd->c2 = '=';

        mode = "%o";
    }
    else
    {
        cmd->c3 = cmd->c2 = '=';

        mode = "%x";
    }

    char user_mode[cmd->text1.len + 1];

    if (cmd->atsign_set && cmd->text1.len != 0)
    {
        memcpy(user_mode, cmd->text1.buf, (ulong)cmd->text1.len);

        user_mode[cmd->text1.len] = NUL;

        if (check_format(user_mode))
        {
            mode = user_mode;
        }
    }

    print_str(mode, cmd->n_arg);

    if (!cmd->colon_set)                // Suppress CRLF?
    {
        print_chr(CRLF);
    }
}


///
///  @brief    Check format string, making sure that one and only one numeric
///            format is specified (we do allow for %%). We do this to ensure
///            that the user does not include something like %s in the format
///            string which would cause unexpected (and potentially fatal)
///            things to happen.
///
///  @returns  true if exactly one numeric format found, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool check_format(const char *format)
{
    assert(format != NULL);

    int c;
    bool is_numeric = false;            // No numeric format seen yet

    while ((c = *format++) != NUL)
    {
        if (c != '%')
        {
            continue;
        }
        else if ((c = *format++) == NUL)
        {
            return false;
        }
        else if (c == '%')              // %% is okay
        {
            continue;
        }
        else if (is_numeric)            // No more than one numeric format
        {
            return false;
        }

        // Here if we have something following % besides %.

        while (!isalpha(c))
        {
            if ((c = *format++) == NUL || !isprint(c))
            {
                return false;
            }
        }

        // Here when we have an alphabetic character.

        if (strchr("hlqjz", c) != NULL)
        {
            if ((c = *format++) == NUL ||
                ((c == 'h' || c == 'l') && (c = *format++) == NUL))
            {
                return false;
            }
        }

        // Here to finally check for a numeric format

        if (strchr("diouxX", c) == NULL)
        {
            return false;
        }

        is_numeric = true;
    }

    return is_numeric;
}
