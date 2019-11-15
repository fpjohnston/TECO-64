///
///  @file    quote_cmd.c
///  @brief   Execute quote command.
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

#include <ctype.h>
#include <stdlib.h>

#include "teco.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


///
///  @brief    Execute quote command (test value).
///
///            "      Start conditional
///            n"<    Text for less than zero
///            n"=    Test for equal to zero
///            n">    Test for greater than zero
///            n"A    Test for alphabetic
///            n"C    Test for symbol constituent
///            n"D    Test for numeric
///            n"E    Test for equal to zero
///            n"F    Test for false
///            n"G    Test for greater than zero
///            n"L    Test for less than zero
///            n"N    Test for not equal to zero
///            n"R    Test for alphanumeric
///            n"S    Test for successful
///            n"T    Test for true
///            n"U    Test for unsuccessful
///            n"V    Test for lower case
///            n"W    Test for upper case
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_quote(void)
{
    check_mod(MOD_N);                   // Allow n"

    if (empty_expr())                   // If nothing on stack,
    {
        print_err(E_NAQ);               // No argument before "
    }

    int c = get_n_arg();                // Character to test
    int test = fetch_cmd();             // Test condition

    init_expr();

    switch (toupper(test))
    {
        case 'A':                       // Test for alphabetic
            if (isalpha(c))
            {
                return;
            }
            break;

        case 'C':                       // Test for symbol constituent
            if (isalnum(c) || c == '.' || c == '_' || c == '$')
            {
                return;
            }
            break;

        case 'D':                       // Test for numeric
            if (isdigit(c))
            {
                return;
            }
            break;

        case '=':                       // Test for equal to zero
        case 'E':                       // Test for equal to zero
        case 'F':                       // Test for false
        case 'U':                       // Test for unsuccessful
            if (c == 0)
            {
                return;
            }
            break;

        case '>':                       // Test for greater than zero
        case 'G':                       // Test for greater than zero
            if (c > 0)
            {
                return;
            }
            break;

        case '<':                       // Test for less than zero
        case 'L':                       // Test for less than zero
        case 'S':                       // Test for successful
        case 'T':                       // Test for true
            if (c < 0)
            {
                return;
            }
            break;

        case 'N':                       // Test for not equal to zero
            if (c != 0)
            {
                return;
            }
            break;

        case 'R':                       // Test for alphanumeric
            if (isalnum(c))
            {
                return;
            }
            break;

        case 'V':                       // Test for lower case
            if (islower(c))
            {
                return;
            }
            break;

        case 'W':                       // Test for upper case
            if (isupper(c))
            {
                return;
            }
            break;

        default:
            print_err(E_IQC);           // Illegal character after "

            return;
    }

#if     0       // TODO: finish this
    if (FlowEE() == EXIT_FAILURE)       // Flow to | or '
    {
        return;
    }
#endif
    
    return;
}

