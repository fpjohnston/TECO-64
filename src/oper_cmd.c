///
///  @file    oper_cmd.c
///  @brief   Execute operator commands.
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

#include "teco.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"


///
///  @brief    Execute @ command modifier.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_atsign(struct cmd *cmd)
{
    assert(cmd != NULL);

    cmd->atsign = true;
}


///
///  @brief    Execute : or :: command modifiers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_colon(struct cmd *cmd)
{
    assert(cmd != NULL);

    cmd->colon = true;

    int c = fetch_cbuf(NOSTART);

    if (c == ':')
    {
        cmd->dcolon = true;
    }
    else
    {
        unfetch_cbuf(c);
    }
}


///
///  @brief    Execute ctrl/underscore command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctl_ubar(struct cmd *cmd)
{
    assert(cmd != NULL);

    push_expr(TYPE_OPER, cmd->c1);
}


///
///  @brief    Execute ctrl/^ (caret or uparrow) command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctl_up(struct cmd *cmd)
{
    assert(cmd != NULL);

    check_args(cmd);

    int c = fetch_cbuf(NOSTART);

    push_expr(c, EXPR_VALUE);
}


///
///  @brief    Execute left parenthesis.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_lparen(struct cmd *cmd)
{
    assert(cmd != NULL);

    ++nparens;

    push_expr(TYPE_GROUP, cmd->c1);
}


///
///  @brief    Execute general operator. This is called for the following:
///
///            +  addition
///            -  subtraction
///            *  multiplication
///            /  division
///            #  logical OR
///            &  logical AND
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_oper(struct cmd *cmd)
{
    assert(cmd != NULL);

    check_args(cmd);

    push_expr(TYPE_OPER, cmd->c1);
}


///
///  @brief    Execute right parenthesis.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_rparen(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (nparens == 0)                   // Can't have ) without (
    {
        throw(E_MLP);                   // Missing left parenthesis
    }
    else if (!check_expr())             // Is there an operand available?
    {
        throw(E_NAP);                   // No argument before )
    }
    else
    {
        --nparens;
    }

    push_expr(TYPE_GROUP, cmd->c1);
}
