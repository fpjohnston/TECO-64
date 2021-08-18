///
///  @file    oper_cmd.c
///  @brief   Scan operator commands.
///
///  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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

#include "teco.h"
#include "cbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "term.h"


///
///  @brief    Scan "," command: separate m and n arguments.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_comma(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->m_set)                     // Already seen m argument?
    {
        throw(E_ARG);                   // Invalid arguments
    }

    if (isoperand())                    // Any m argument specified?
    {
        // If we've seen a comma, then what was on the expression stack was
        // an "m" argument, not an "n" argument (numeric arguments can take
        // the form m,n).

        cmd->m_set = true;
        cmd->m_arg = pop_x();
    }
    else if (f.e2.comma)                // No m arg. - is that an error?
    {
        throw(E_NAC);                   // No argument before comma
    }

    return true;
}


///
///  @brief    Scan "^_" (CTRL/_) command: 1's complement operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_ubar(struct cmd *cmd)
{
    assert(cmd != NULL);

    push_x(OPER, X_1S_COMP);

    return true;
}


///
///  @brief    Scan "/" command: division operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_div(struct cmd *cmd)
{
    assert(cmd != NULL);

    // Check for double slash remainder operator.

    int c;

    if (f.e1.xoper && nparens != 0 && (c = peek_cbuf()) == '/')
    {
        next_cbuf();
        trace_cbuf(c);

        cmd->c2 = (char)c;

        push_x(OPER, X_REM);
    }
    else
    {
        push_x(OPER, X_DIV);
    }

    return true;
}


///
///  @brief    Scan "(" command: expression grouping.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_lparen(struct cmd *cmd)
{
    assert(cmd != NULL);

    // The following means that a command string such as mmm (nnn) just has a
    // value of nnn; the mmm is discarded. This allows for enclosing Q and other
    // commands in parentheses to guard against being affected (and the command
    // possibly being completely changed) by a preceding value. In the case of
    // a Q command, for example, instead of returning the numeric value in the
    // Q-register, a preceding value would return the ASCII value of the nth
    // character in the Q-register text string.

    if (isoperand())                    // Operand on stack?
    {
        (void)pop_x();                  // Yes, just discard it
    }

    ++nparens;

    push_x(OPER, X_LPAREN);

    return true;
}


///
///  @brief    Scan operator commands. This is called for the following:
///
///            +  addition
///            -  subtraction
///            *  multiplication
///            /  division
///            #  logical OR
///            &  logical AND
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_oper(struct cmd *cmd)
{
    assert(cmd != NULL);

    // The following takes advantage of the fact that the operators have the
    // same value as the equivalent expression types (e.g., X_MUL == '*').

    push_x(OPER, cmd->c1);

    return true;
}


///
///  @brief    Scan ")" command: expression grouping.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_rparen(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (nparens == 0)                   // Can't have ) without (
    {
        throw(E_MLP);                   // Missing left parenthesis
    }
    else if (!isoperand())              // Is there an operand available?
    {
        throw(E_NAP);                   // No argument before )
    }
    else
    {
        --nparens;
    }

    push_x(OPER, X_RPAREN);

    return true;
}


///
///  @brief    Scan "~" command: exclusive OR operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_tilde(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!f.e1.xoper || nparens == 0)
    {
        throw(E_ILL, cmd->c1);          // Illegal command
    }

    push_x(OPER, X_XOR);

    return true;
}
