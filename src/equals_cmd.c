///
///  @file    equals_cmd.c
///  @brief   Execute "=" command.
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

#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


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

void exec_equals(void)
{
    check_mod(MOD_C | MOD_D | MOD_N);

    if (empty_expr())
    {
        print_err(E_NAE);               // No argument before =
    }

    int n_arg = get_n_arg();
    int c;

    if ((c = fetch_cmd()) == EOF || c != '=')
    {
        printf("%d", n_arg);

        if (c != EOF)
        {
            unfetch_cmd(c);
        }
    }
    else if ((c = fetch_cmd()) == EOF || c != '=')
    {
        printf("%o", n_arg);

        if (c != EOF)
        {
            unfetch_cmd(c);
        }
    }
    else
    {
        printf("%x", n_arg);
    }

    (void)fflush(stdout);

    if (f.ei.colon)                     // Colon modifier?
    {
        f.ei.colon = false;             // Yes, clear flag
    }
    else
    {
        putc_term(CRLF);
    }
}
