///
///  @file    if_cmd.c
///  @brief   Execute conditional commands.
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
#include "cmdbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "term.h"


#define NO_ELSE     (bool)false         ///< Don't execute | command
#define ELSE_OK     (bool)true          ///< Execute | command if found

// Local functions

static void skip_if(bool else_ok);


///
///  @brief    Execute | command: 'else' clause of conditional statement. We get
///            here when we encounter a vertical bar while executing commands
///            after a quote command, and therefore need to skip past the 'else'
///            clause.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_else(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    if (ctrl.depth == 0)
    {
        throw(E_MSC);                   // Missing start of conditional
    }

    skip_if(NO_ELSE);                   // Skip to end of conditional

    reset_x();                          // Reset expression stack
}


///
///  @brief    Execute ' command: end conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_endif(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    if (ctrl.depth-- == 0)
    {
        throw(E_MSC);                   // Missing start of conditional
    }

    reset_x();                          // Reset expression stack
}


///
///  @brief    Execute F| command: flow to else clause of conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_else(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    if (ctrl.depth == 0)
    {
        ++ctrl.depth;
    }

    skip_if(ELSE_OK);                   // Skip to 'else'

    reset_x();                          // Reset expression stack
}


///
///  @brief    Execute F' command: flow to end of conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_endif(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    if (ctrl.depth == 0)
    {
        ++ctrl.depth;
    }

    skip_if(NO_ELSE);                   // Skip to end of conditional

    reset_x();                          // Reset expression stack
}


///
///  @brief    Execute " (quote) command: if/then/else statement.
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

void exec_if(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_M, NO_COLON, NO_ATSIGN);

    if (!cmd->n_set)                    // Did we see an argument?
    {
        throw(E_NAQ);                   // No argument before "
    }

    int c = (int)cmd->n_arg;

    ++ctrl.depth;                       // Increase conditional depth

    reset_x();                          // Reset expression stack

    switch (cmd->c2)
    {
        case 'A':                       // Test for alphabetic
        case 'a':                       // Test for alphabetic
            if (isalpha(c))
            {
                return;
            }
            break;

        case 'C':                       // Test for symbol constituent
        case 'c':                       // Test for symbol constituent
            if (isalnum(c) || c == '.' || c == '_' || c == '$')
            {
                return;
            }
            break;

        case 'D':                       // Test for numeric
        case 'd':                       // Test for numeric
            if (isdigit(c))
            {
                return;
            }
            break;

        case '=':                       // Test for equal to zero
        case 'E':                       // Test for equal to zero
        case 'e':                       // Test for equal to zero
        case 'F':                       // Test for false
        case 'f':                       // Test for false
        case 'U':                       // Test for unsuccessful
        case 'u':                       // Test for unsuccessful
            if (c == 0)
            {
                return;
            }
            break;

        case '>':                       // Test for greater than zero
        case 'G':                       // Test for greater than zero
        case 'g':                       // Test for greater than zero
            if (c > 0)
            {
                return;
            }
            break;

        case '<':                       // Test for less than zero
        case 'L':                       // Test for less than zero
        case 'l':                       // Test for less than zero
        case 'S':                       // Test for successful
        case 's':                       // Test for successful
        case 'T':                       // Test for true
        case 't':                       // Test for true
            if (c < 0)
            {
                return;
            }
            break;

        case 'N':                       // Test for not equal to zero
        case 'n':                       // Test for not equal to zero
            if (c != 0)
            {
                return;
            }
            break;

        case 'R':                       // Test for alphanumeric
        case 'r':                       // Test for alphanumeric
            if (isalnum(c))
            {
                return;
            }
            break;

        case 'V':                       // Test for lower case
        case 'v':                       // Test for lower case
            if (islower(c))
            {
                return;
            }
            break;

        case 'W':                       // Test for upper case
        case 'w':                       // Test for upper case
            if (isupper(c))
            {
                return;
            }
            break;

        default:
            // Note: reset_if() will be called during error processing,
            //       so we don't need to call it here.

            throw(E_IQC);               // Invalid quote character
    }

    // Here if the test was unsuccessful

    skip_if(ELSE_OK);
}


///
///  @brief    Scan " (quote) command.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_if(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_M, NO_COLON, NO_DCOLON, NO_ATSIGN);

    int c = require_cbuf();             // Get test condition

    cmd->c2 = (char)c;

    return false;
}


///
///  @brief    Skip to end of conditional statement, or to 'else' statement,
///            checking for conditional statements nested within the current
///            statement. We also have to check for any loops that start or
///            end while we are skipping commands, as shown in the table
///            below (where "Q means any conditional):
///
///            "Q '            OK
///            "Q | '          OK
///            "Q < > '        OK
///            "Q | < > '      OK
///            "Q < > | < > '  OK
///            "Q < ' >        ERROR
///            "Q < | > '      ERROR
///            "Q | < ' >      ERROR
///            "Q > '          ERROR
///            "Q | > '        ERROR
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void skip_if(bool else_ok)
{
    assert(ctrl.depth > 0);

    struct cmd cmd;                     // Scrap command block for skip_cmd()
    uint level = 0;
    uint start = ctrl.depth;

    while (skip_cmd(&cmd, "<>\"'|"))
    {
        switch (cmd.c1)
        {
            case '<':                   // Start of loop
                ++level;

                break;

            case '>':                   // End of loop
                if (level-- == 0)
                {
                    throw(E_BNI);       // Right angle bracket not in iteration
                }

                break;

            case '"':                   // 'if' command
                ++ctrl.depth;

                break;

            case '\'':                  // 'endif' command
                if (ctrl.depth-- == start)
                {
                    if (level != 0)     // Can't jump into a loop
                    {
                        throw(E_MRA);   // Missing right angle bracket
                    }

                    if (f.trace)
                    {
                        echo_in(cmd.c1);
                    }

                    return;
                }

                break;

            case '|':                   // 'else' command
                if (ctrl.depth == start)
                {
                    if (else_ok)
                    {
                        if (f.trace)
                        {
                            echo_in(cmd.c1);
                        }

                        return;
                    }
                }

                break;

            default:
                break;
        }
    }

    throw(E_MAP);                       // Missing apostrophe
}
