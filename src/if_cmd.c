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
#include <stdio.h>

#include "teco.h"
#include "cbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "term.h"


#define NO_ELSE     (bool)false         ///< Don't execute | command
#define ELSE_OK     (bool)true          ///< Execute | command if found

#define MAX_IF      32                  ///< Maximum nesting depth

///  @var     quote
///
///  @brief   Conditional command state

static struct
{
    uint depth;
    uint_t start_if[MAX_IF];
    uint_t start_else[MAX_IF];
    uint loop[MAX_IF];
} quote =
{
    .depth = 0,
};

// Local functions

static void endif(struct cmd *cmd, bool vbar);

static void pop_if(void);

static void push_if(void);


///
///  @brief    Flow to end of conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void endif(struct cmd *unused, bool else_ok)
{
    struct cmd cmdblock;
    struct cmd *cmd = &cmdblock;

    assert(cmd != NULL);

    if (quote.depth == 0)
    {
        throw(E_MAP);                   // Missing apostrophe
    }

    const uint start_if = quote.depth;  // Initial conditional depth

    do
    {
        if (!skip_cmd(cmd, "\"'<>|"))
        {
            throw(E_MAP);               // Missing apostrophe
        }

        //  The following code ensures that a loop begun after a " command ends
        //  before the corresponding | or ' command, and that a loop begun after
        //  a | command ends before the corresponding ' command.
        //
        //  Simplified examples ("Q means any conditional):
        //
        //      "Q < > '        OK
        //      "Q | < > '      OK
        //      "Q < > | < > '  OK
        //      "Q < ' >        ERROR
        //      "Q < | > '      ERROR
        //      "Q | < ' >      ERROR
        //      "Q > '          ERROR
        //      "Q | > '        ERROR

        if (cmd->c1 == '<')             // Loop start
        {
            setloop_depth(getloop_depth() + 1);
        }
        else if (cmd->c1 == '>')        // Loop end
        {
            setloop_depth(getloop_depth() - 1);

            if (f.e2.quote && getloop_depth() < quote.loop[quote.depth - 1])
            {
                throw(E_MRA);           // Missing right angle bracket
            }
        }
        else if (cmd->c1 == '"')        // Conditional start
        {
            push_if();
        }
        else if (cmd->c1 == '\'')       // Conditional end
        {
            if (f.trace && quote.depth == start_if)
            {
                echo_in(cmd->c1);
            }

            pop_if();
        }
        else if (cmd->c1 == '|')        // Conditional else
        {
            if (f.e2.quote && getloop_depth() != quote.loop[quote.depth - 1])
            {
                throw(E_MRA);           // Missing right angle bracket
            }

            if (else_ok)
            {
                quote.start_else[quote.depth] = cbuf->pos;

                if (quote.depth == start_if)
                {
                    if (f.trace)
                    {
                        echo_in(cmd->c1);
                    }

                    break;
                }
            }
        }

    } while (quote.depth >= start_if);
}


///
///  @brief    Execute ' command: end conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_apos(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_ATSIGN);

    if (quote.depth == 0)
    {
        throw(E_MSC);                   // Missing start of conditional
    }

    if (f.e2.quote && getloop_depth() != quote.loop[quote.depth - 1])
    {
        throw(E_MRA);                   // Missing right angle bracket
    }

    pop_if();

    init_x();                           // Reinitialize expression stack
}


///
///  @brief    Execute F' command: flow to end of conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_apos(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_ATSIGN);

    endif(cmd, NO_ELSE);                // Skip any else statement.

    init_x();                           // Reinitialize expression stack
}


///
///  @brief    Execute F| command: flow to else clause of conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_vbar(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_ATSIGN);

    endif(cmd, ELSE_OK);

    init_x();                           // Reinitialize expression stack
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

void exec_quote(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!cmd->n_set)                    // Did we see an argument?
    {
        throw(E_NAQ);                   // No argument before "
    }

    int c = (int)cmd->n_arg;

    init_x();                           // Reinitialize expression stack
    push_if();

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

    endif(cmd, ELSE_OK);
}


///
///  @brief    Execute | command: else clause of conditional statement.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_vbar(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_ATSIGN);

    if (quote.depth == 0)
    {
        throw(E_MSC);                   // Missing start of conditional
    }

    if (f.e2.quote)
    {
        if (getloop_depth() != quote.loop[quote.depth])
        {
            throw(E_MRA);               // Missing right angle bracket
        }
    }

    quote.start_else[quote.depth] = cbuf->pos;

    endif(cmd, NO_ELSE);

    init_x();                           // Reinitialize expression stack
}


///
///  @brief    Get conditional depth.
///
///  @returns  Conditional depth.
///
////////////////////////////////////////////////////////////////////////////////

uint getif_depth(void)
{
    return quote.depth;
}


///
///  @brief    End a conditional.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void pop_if(void)
{
    assert(quote.depth > 0);            // Error if not in conditional

    --quote.depth;
}


///
///  @brief    Start a new conditional.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void push_if(void)
{
    if (quote.depth >= MAX_IF)
    {
        throw(E_MAX);                   // Internal program limit reached
    }

    quote.loop[quote.depth] = getloop_depth();
    quote.start_if[quote.depth] = cbuf->pos;
    quote.start_else[quote.depth] = (uint_t)EOF;

    ++quote.depth;
}


///
///  @brief    Reset conditional statement depth.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_if(void)
{
    quote.depth = 0;
}


///
///  @brief    Scan " (quote) command.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_quote(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_M, NO_COLON, NO_ATSIGN);

    int c = require_cbuf();             // Get test condition

    cmd->c2 = (char)c;

    return false;
}


///
///  @brief    Set conditional depth.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void setif_depth(uint depth)
{
    quote.depth = depth;
}


