///
///  @file    loop_cmd.c
///  @brief   Execute loop commands.
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
#include <stdio.h>

#include "teco.h"
#include "cmdbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "term.h"


#define INFINITE    (0)             ///< Infinite loop count

// Local functions

static void skip_loop(void);


///
///  @brief    Check to see if we're in a loop.
///
///  @returns  true if we're in a loop, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool check_loop(void)
{
    if (ctrl.level != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


///
///  @brief    Execute F> command: flow to end of loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_greater(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    if (ctrl.level == 0)                // Outside of loop?
    {
        cbuf->pos = cbuf->len;          // Yes, end the command string
    }
    else
    {
        uint i = ctrl.level - 1;

        if (ctrl.loop[i].iter == INFINITE || --ctrl.loop[i].iter > 0)
        {
            cbuf->pos = ctrl.loop[i].pos; // Go back to start of loop
            cmd_line = ctrl.loop[i].line; // Reset line number
        }
        else
        {
            skip_loop();
        }

        reset_x();                      // Reset expression stack
    }
}


///
///  @brief    Execute F< command: flow to start of loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_less(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    if (ctrl.level == 0)                // Outside of loop?
    {
        cbuf->pos = 0;                  // Yes, reset to start of command
    }
    else                                // No, restart the loop
    {
        uint i = ctrl.level - 1;

        cbuf->pos = ctrl.loop[i].pos;
    }

    reset_x();                          // Reset expression stack

    return;
}


///
///  @brief    Execute > command: end of loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_greater(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (ctrl.level == 0)
    {
        throw(E_BNI);                   // Right angle bracket not in iteration
    }

    uint i = ctrl.level - 1;

    if (ctrl.loop[i].iter == INFINITE || --ctrl.loop[i].iter > 0)
    {
        cbuf->pos = ctrl.loop[i].pos;   // Go back to start of loop
        cmd_line = ctrl.loop[i].line;   // Reset line number
    }
    else
    {
        --ctrl.level;                   // Done with loop
    }

    reset_x();                          // Reset expression stack
}


///
///  @brief    Execute < command: start of loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_less(struct cmd *cmd)
{
    assert(cmd != NULL);

    int_t iter = INFINITE;              // Assume infinite loop

    if (cmd->n_set && (iter = cmd->n_arg) <= 0)
    {
        ++ctrl.level;

        skip_loop();                    // Skip loop if count is <= 0
    }
    else
    {
        if (ctrl.level == MAX_LOOPS)
        {
            throw(E_MAX);               // Internal program limit reached
        }

        uint i = ctrl.level++;

        ctrl.loop[i].iter  = iter;
        ctrl.loop[i].pos   = cbuf->pos;
        ctrl.loop[i].line  = cmd_line;
        ctrl.loop[i].depth = ctrl.depth;
    }

    reset_x();                          // Reset expression stack
}


///
///  @brief    Execute ; command: exit loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_semi(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (ctrl.level == 0)
    {
        throw(E_SNI);                   // Semi-colon not in loop
    }

    if (!cmd->n_set)
    {
        throw(E_NAS);                   // No argument before semi-colon
    }

    if (cmd->colon)                     // n:; command
    {
        if (cmd->n_arg >= 0)            // End loop if n < 0
        {
            return;
        }
    }
    else                                // n; command
    {
        if (cmd->n_arg < 0)             // End loop if n >= 0
        {
            return;
        }
    }

    skip_loop();
    reset_x();                          // Reset expression stack
}


///
///  @brief    Exit loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exit_loop(struct cmd *cmd)
{
    assert(cmd != NULL);

    skip_loop();
    reset_x();                          // Reset expression stack
}


///
///  @brief    Scan > command: relational operator.
///
///  @returns  true if extended operator found, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_greater(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    // ">" is a relational operator only if it's in parentheses; otherwise,
    // it's the end of a loop.

    if (!f.e1.xoper || !check_parens())
    {
        return false;
    }

    // The following is necessary to handle the situation where a '>' command
    // is a relational operator (or part of one) and not the end of a loop.

    if (cmd->n_set)
    {
        cmd->n_set = false;

        store_val(cmd->n_arg);
    }

    if (peek_cbuf() == '=')             // >= operator
    {
        next_cbuf();
        store_oper(X_GE);
    }
    else if (peek_cbuf() == '>')        // >> operator
    {
        next_cbuf();
        store_oper(X_RSHIFT);
    }
    else                                // > operator
    {
        store_oper(X_GT);
    }

    return true;
}


///
///  @brief    Scan < command: relational operator or start of loop.
///
///  @returns  true if extended operator found, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_less(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_M, NO_COLON, NO_DCOLON, NO_ATSIGN);

    // "<" is a relational operator only if it's in parentheses; otherwise,
    // it's the start of a loop.

    if (!f.e1.xoper || !check_parens())
    {
        return false;
    }

    // The following is necessary to handle the situation where a '<' command
    // is a relational operator (or part therefore) and not the start of a loop.

    if (cmd->n_set)
    {
        cmd->n_set = false;

        store_val(cmd->n_arg);
    }

    int c = peek_cbuf();

    if (c == '=')                       // <= operator
    {
        next_cbuf();
        store_oper(X_LE);
    }
    else if (c == '>')                  // <> operator
    {
        next_cbuf();
        store_oper(X_NE);
    }
    else if (c == '<')                  // << operator
    {
        next_cbuf();
        store_oper(X_LSHIFT);
    }
    else                                // < operator
    {
        store_oper(X_LT);
    }

    return true;
}


///
///  @brief    Scan ; command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_semi(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_M, NO_DCOLON, NO_ATSIGN);

    return false;
}


///
///  @brief    Flow to end of loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void skip_loop(void)
{
    assert(ctrl.level > 0);

    struct cmd cmd;
    uint level = ctrl.level--;

    while (skip_cmd(&cmd, "<>\"'"))
    {
        switch (cmd.c1)
        {
            case '<':                   // Start of loop
                ++level;

                break;

            case '>':                   // End of loop
                if (--level == ctrl.level)
                {
                    if (f.trace)
                    {
                        echo_in(cmd.c1);
                    }

                    return;
                }

                break;

            case '"':                   // Start of conditional
                ++ctrl.depth;

                break;

            case '\'':                  // End of conditional
                if (ctrl.depth-- == 0)
                {
                    throw(E_MSC);       // Missing start of conditional
                }

                break;

            default:
                break;
        }
    }

    throw(E_MRA);                       // Missing right angle bracket
}
