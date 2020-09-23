///
///  @file    oper_cmd.c
///  @brief   Execute operator commands.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"


///
///  @brief    Execute @ command modifier.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_atsign(struct cmd *unused1)
{
}


///
///  @brief    Execute : or :: command modifiers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_colon(struct cmd *unused1)
{
}


///
///  @brief    Process "," (comma argument separator).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_comma(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->m_set)                     // Already seen m argument?
    {
        throw(E_ARG);                   // Invalid arguments
    }

    if (!pop_expr(&cmd->m_arg))         // Any m argument specified?
    {
        if (f.e2.comma)                 // No -- should we issue error?
        {
            throw(E_NAC);               // No argument before comma
        }

        return;
    }

    // If we've seen a comma, then what was on the expression stack was an "m"
    // argument, not an "n" argument (numeric arguments can take the form m,n).

    cmd->m_set = true;
}


///
///  @brief    Execute ctrl/underscore command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_ubar(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    push_expr(TYPE_OPER, cmd->c1);
}


///
///  @brief    Execute ctrl/^ (caret or uparrow) command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_up(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    check_args(cmd);

    int c = fetch_cbuf();

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
    assert(cmd != NULL);                // Error if no command block

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
    assert(cmd != NULL);                // Error if no command block

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
    assert(cmd != NULL);                // Error if no command block

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
