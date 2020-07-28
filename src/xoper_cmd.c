///
///  @file    xoper_cmd.c
///  @brief   Execute extended operator commands.
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

#include "teco.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"


///
///  @brief    Execute extended operator (if enabled).
///
///  @returns  true if extended operator found, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool exec_xoper(int c)
{
    if (nparens == 0 || !f.e1.xoper)
    {
        return false;                   // No extended operator possible
    }

    switch (c)
    {
        case '>':                       // Check for >, >=, and >>
            c = fetch_cbuf(NOSTART);

            if (c == '=')
            {
                c = EXPR_GE;
            }
            else if (c == '>')
            {
                c = EXPR_RSHIFT;
            }
            else
            {
                unfetch_cbuf(c);

                c = EXPR_GT;
            }

            break;

        case '<':                       // Check for <, <=, <>, and <<
            c = fetch_cbuf(NOSTART);

            if (c == '=')
            {
                c = EXPR_LE;
            }
            else if (c == '>')
            {
                c = EXPR_NE;
            }
            else if (c == '<')
            {
                c = EXPR_LSHIFT;
            }
            else
            {
                unfetch_cbuf(c);

                c = EXPR_LT;
            }

            break;

        case '=':                       // Check for ==
            c = fetch_cbuf(NOSTART);

            if (c != '=')
            {
                throw(E_ARG);
            }

            break;

        case '/':                       // Check for //
            c = fetch_cbuf(NOSTART);

            if (c != '/')
            {
                unfetch_cbuf(c);

                return false;
            }

            c = EXPR_REM;

            break;

        case '!':
        case '~':
            break;

        default:
            return false;
    }

    push_expr(TYPE_OPER, c);

    return true;
}