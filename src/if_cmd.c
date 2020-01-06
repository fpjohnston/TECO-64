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
#include "eflags.h"
#include "errors.h"
#include "exec.h"


// TODO: add code to handle termination of loop before end of conditional
//       (e.g., a command string like "< 1 "N > '".

static uint if_depth = 0;           ///< Current depth of conditionals


// Local functions

static void endif(struct cmd *cmd, bool vbar);


///
///  @brief    Flow to end of conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void endif(struct cmd *cmd, bool vbar)
{
    assert(cmd != NULL);

    uint start_depth = if_depth;

    do
    {
        scan.dryrun = true;
        (void)next_cmd(cmd);
        scan.dryrun = false;

        if (cmd->c1 == '"')             // Start of a new conditional?
        {
            ++if_depth;
        }
        else if (cmd->c1 == '\'')       // End of conditional?
        {
            --if_depth;
        }
        else if (vbar && cmd->c1 == '|' && if_depth == start_depth)
        {
            break;
        }

    } while (if_depth >= start_depth);
}


///
///  @brief    Execute ' (apostrophe) command: end conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_apos(struct cmd *unused1)
{
    if (if_depth > 0)
    {
        --if_depth;
    }
}


///
///  @brief    Execute F' command: flow to end of conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_apos(struct cmd *cmd)
{
    endif(cmd, (bool)false);
}


///
///  @brief    Execute F| command: flow to else clause of conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_vbar(struct cmd *cmd)
{
    endif(cmd, (bool)true);
}


///
///  @brief    Execute quote command: if/then/else statement.
///
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

    ++if_depth;

    switch (toupper(cmd->c2))
    {
        case 'A':                       // Test for alphabetic
            if (isalpha(c))
            {
                return;
            }
            break;

        case 'C':                       // Test for symbol constituent
            if (isalnum(c) || c == '.' || c == '_' || (f.e2.dollar && c == '$'))
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
            --if_depth;

            print_err(E_IQC);           // Illegal quote character
    }

    // Here if the test was unsuccessful

    endif(cmd, (bool)true);
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

    endif(cmd, (bool)false);
}


///
///  @brief    Reset conditional statement depth.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_if(void)
{
    if_depth = 0;
}
