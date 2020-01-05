///
///  @file    brace_cmd.c
///  @brief   Execute braced expressions.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


///
///  @brief    Execute braced expression.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_brace(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!f.e1.brace)
    {
        scan_bad(cmd);                  // Issue error and return to main loop
    }

    enum expr_type type = EXPR_NONE;
    int c;
    int c1 = NUL;
    int c2 = NUL;
    bool space = false;

    // Find the one or two characters inside the braced expression

    while ((c = fetch_buf(NOCMD_START)) != '}')
    {
        if (isspace(c))
        {
            space = true;

            continue;
        }
        else if (c1 == NUL)
        {
            space = false;            

            c1 = toupper(c);
        }
        else if (c2 == NUL && !space)
        {
            c2 = toupper(c);
        }
        else
        {
            print_err(E_BRC);           // Invalid braced expression
        }
    }    

    // Here when we've reached the right brace and have the expression

    switch (c1)
    {
        case 'G':
            if (c2 == 'E')
            {
                type = EXPR_GE;
            }
            else if (c2 == 'T')
            {
                type = EXPR_GT;
            }
            break;

        case 'E':
            if (c2 == 'Q')
            {
                type = EXPR_EQ;
            }
            break;

        case 'L':
            if (c2 == 'E')
            {
                type = EXPR_LE;
            }
            else if (c2 == 'T')
            {
                type = EXPR_LT;
            }
            break;

        case 'N':
            if (c2 == 'E')
            {
                type = EXPR_NE;
            }
            break;

        case '^':
            type = EXPR_XOR;
            break;

        case '%':
            type = EXPR_REM;
            break;

        case '<':
            if (c2 == NUL)
            {
                type = EXPR_LT;
            }
            else if (c2 == '=')
            {
                type = EXPR_LE;
            }
            else if (c2 == '>')
            {
                type = EXPR_NE;
            }
            else if (c2 == '<')
            {
                type = EXPR_LEFT;
            }
            break;
            
        case '>':
            if (c2 == NUL)
            {
                type = EXPR_GT;
            }
            else if (c2 == '=')
            {
                type = EXPR_GE;
            }
            else if (c2 == '>')
            {
                type = EXPR_RIGHT;
            }
            break;

        case '=':
            if (c2 == '=')
            {
                type = EXPR_EQ;
            }
            break;

        default:
            break;
    }

    if (type == EXPR_NONE)
    {
        print_err(E_BRC);               // Invalid braced expression
    }

    push_expr(2, type);
}
