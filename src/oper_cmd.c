///
///  @file    oper_cmd.c
///  @brief   Scan operator commands.
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
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"


///
///  @brief    Scan + command: addition or unary plus operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_add(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    store_add();

    return true;
}


///
///  @brief    Scan & command: logical AND operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_and(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    store_oper(X_AND);

    return true;
}


///
///  @brief    Scan , (comma) command: separate m and n arguments.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_comma(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    if (cmd->m_set)                     // Already seen comma?
    {
        throw(E_ARG);                   // Invalid arguments
    }

    if (query_x(&cmd->m_arg))           // Any m argument specified?
    {
        // If we've seen a comma, then what was on the expression stack was
        // an "m" argument, not an "n" argument (numeric arguments can take
        // the form m,n).

        cmd->m_set = true;

#if     !defined(NOSTRICT)

        f.e0.digit = false;

#endif

    }
    else
    {
        throw(E_NAC);                   // No argument before comma
    }

    return true;
}


///
///  @brief    Scan CTRL/_ command: 1's complement operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_under(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    exec_oper(X_COMPL);

    return true;
}


///
///  @brief    Scan * command: multiplication operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_mul(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    store_oper(X_MUL);

    return true;
}


///
///  @brief    Scan # command: logical OR operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_or(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    store_oper(X_OR);

    return true;
}


///
///  @brief    Scan - command: subtraction or unary minus operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_sub(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    store_sub();

    return true;
}
