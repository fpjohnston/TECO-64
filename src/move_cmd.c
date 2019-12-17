///
///  @file    move_cmd.c
///  @brief   Execute C, J, L, and R commands.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "textbuf.h"
#include "errors.h"
#include "exec.h"


// Local functions

static void exec_c_r(struct cmd *cmd, int sign);

static void exec_move(struct cmd *cmd, uint pos, bool cond);

    
///
///  @brief    Execute C command: move to relative position forward in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_C(struct cmd *cmd)
{
    exec_c_r(cmd, 1);
}


///
///  @brief    Execute move command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_c_r(struct cmd *cmd, int sign)
{
    assert(cmd != NULL);

    int n = 1;                          // Default move argument

    if (cmd->n_set)
    {
        n = cmd->n_arg;
    }

    n *= sign;

    uint dot = getpos_tbuf();
    uint Z = getsize_tbuf();

    n += (int)dot;                      // Calculate absolute position

    exec_move(cmd, (uint)n, (bool)(n < B || (uint)n > Z));
}


///
///  @brief    Execute J command: move to absolute position in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_J(struct cmd *cmd)
{
    assert(cmd != NULL);

    uint n = B;

    if (cmd->n_set)
    {
        n = (uint)cmd->n_arg;
    }

    uint Z = getsize_tbuf();

    exec_move(cmd, n, (bool)(n > Z));
}


///
///  @brief    Execute L command: move n lines forwards or backwards.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_L(struct cmd *cmd)
{
    assert(cmd != NULL);

    int n = 1;
    
    if (cmd->n_set)
    {
        n = cmd->n_arg;
    }

    uint dot = getpos_tbuf();

    n = getdelta_tbuf(n);

    setpos_tbuf((uint)n + dot);
}


///
///  @brief    Execute move command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_move(struct cmd *cmd, uint pos, bool cond)
{
    assert(cmd != NULL);

    if (cond)
    {
        if (!cmd->colon_set)
        {
            print_err(E_POP);           // Pointer off page
        }

        push_expr(TECO_FAILURE, EXPR_VALUE);

        return;
    }

    setpos_tbuf(pos);

    if (cmd->colon_set)
    {
        push_expr(TECO_SUCCESS, EXPR_VALUE);
    }
}


///
///  @brief    Execute R command: move to relative position backward in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_R(struct cmd *cmd)
{
    exec_c_r(cmd, -1);                  // Reverse of C command
}
