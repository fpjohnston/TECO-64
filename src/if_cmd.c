///
///  @file    if_cmd.c
///  @brief   Execute conditional commands.
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
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "errors.h"
#include "exec.h"


///
///  @brief    Execute ' (apostrophe) command: end conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_apos(struct cmd *cmd)
{
    assert(cmd != NULL);

    // TODO: add more here
}


///
///  @brief    Execute F' command: flow to end of conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_apos(struct cmd *unused1)
{
    printf("%s() not yet completed\r\n", __func__);

    return;
}


///
///  @brief    Execute F| command: flow to else clause of conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_vbar(struct cmd *unused1)
{
    printf("%s() not yet completed\r\n", __func__);

    return;
}


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

void exec_quote(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!cmd->n_set)                    // Did we see an argument?
    {
        print_err(E_NAQ);               // No argument before "
    }

    int c = cmd->n_arg;                 // Value to test
    int test = cmd->c2;                 // Test condition

    switch (toupper(test))
    {
        case 'A':                       // Test for alphabetic
            if (!isalpha(c))
            {
                return;
            }
            break;

        case 'C':                       // Test for symbol constituent
            if (!isalnum(c) && c != '.' && c != '_' && c != '$')
            {
                return;
            }
            break;

        case 'D':                       // Test for numeric
            if (!isdigit(c))
            {
                return;
            }
            break;

        case '=':                       // Test for equal to zero
        case 'E':                       // Test for equal to zero
        case 'F':                       // Test for false
        case 'U':                       // Test for unsuccessful
            if (!(c == 0))
            {
                return;
            }
            break;

        case '>':                       // Test for greater than zero
        case 'G':                       // Test for greater than zero
            if (!(c > 0))
            {
                return;
            }
            break;

        case '<':                       // Test for less than zero
        case 'L':                       // Test for less than zero
        case 'S':                       // Test for successful
        case 'T':                       // Test for true
            if (!(c < 0))
            {
                return;
            }
            break;

        case 'N':                       // Test for not equal to zero
            if (c == 0)
            {
                return;
            }
            break;

        case 'R':                       // Test for alphanumeric
            if (!isalnum(c))
            {
                return;
            }
            break;

        case 'V':                       // Test for lower case
            if (!islower(c))
            {
                return;
            }
            break;

        case 'W':                       // Test for upper case
            if (!isupper(c))
            {
                return;
            }
            break;

        default:
            print_err(E_IQC);           // Illegal character after "
    }

    // Here if the test was successful

#if     0       // TODO: finish this
    if (FlowEE() == EXIT_FAILURE)       // Flow to | or '
    {
        return;
    }
#endif

}


///
///  @brief    Execute | (vertical bar) command: else clause of conditional
///            statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_vbar(struct cmd *cmd)
{
    assert(cmd != NULL);

    // TODO: add more here
}


///
///  @brief    Scan " (quote) command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_quote(struct cmd *cmd)
{
    assert(cmd != NULL);
    
    if (scan_state != SCAN_PASS2)
    {
        cmd->c2 = (char)fetch_buf();   // Just store 2nd character

        if (strchr("ACDEFGLNRSTUVW<=>", cmd->c2) == NULL)
        {
            print_err(E_IQC);
        }

        scan_state = SCAN_PASS2;
    }
}
