///
///  @file    move_cmd.c
///  @brief   Execute C, J, L, and R commands.
///
///  @copyright 2019-2022 Franklin P. Johnston / Nowwith Treble Software
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
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "editbuf.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"


// Local functions

static void exec_c_r(struct cmd *cmd, int sign, int chr);

static void exec_move(struct cmd *cmd, int_t pos, bool pop, int chr);


///
///  @brief    Execute C command: move to relative position forward in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_C(struct cmd *cmd)
{
    exec_c_r(cmd, 1, 'C');
}


///
///  @brief    Execute move command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_c_r(struct cmd *cmd, int sign, int chr)
{
    assert(cmd != NULL);

    int_t n = 1;                        // Default move argument

    if (cmd->n_set)
    {
        n = cmd->n_arg;
    }

    n *= sign;
    n += t->dot;                         // Calculate absolute position

    exec_move(cmd, n, (bool)(n < t->B || n > t->Z), chr);
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

    int_t n = t->B;

    if (cmd->n_set)
    {
        n = cmd->n_arg;
    }

    exec_move(cmd, n, (bool)(n < t->B || n > t->Z), 'J');
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

    int_t n = cmd->colon ? 0 : 1;

    if (cmd->n_set)
    {
        n = cmd->n_arg;
    }

    if (!cmd->colon)
    {
        n = len_edit(n);

        set_dot(n + t->dot);

        return;
    }

    // Here if we have :L or n:L to count lines in buffer.
    //
    //  0:L -> Total no. of lines
    // -1:L -> No. of preceding lines
    //  1:L -> No. of following lines
    //   :L -> Same as 0:L

    if (n < 0)
    {
        push_x(before_dot(), X_OPERAND);
    }
    else if (n > 0)
    {
        push_x(after_dot(), X_OPERAND);
    }
    else
    {
        push_x(before_dot() + after_dot(), X_OPERAND);
    }
}


///
///  @brief    Execute move command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_move(struct cmd *cmd, int_t pos, bool pop, int chr)
{
    assert(cmd != NULL);

    if (pop)
    {
        if (!cmd->colon)
        {
            if (chr == 'C')
            {
                throw(E_POP, "C");      // Pointer off page
            }
            else if (chr == 'J')
            {
                throw(E_POP, "J");      // Pointer off page
            }
            else                        // Must be R
            {
                throw(E_POP, "R");      // Pointer off page
            }
        }

        push_x(FAILURE, X_OPERAND);

        return;
    }

    set_dot(pos);

    if (cmd->colon)
    {
        push_x(SUCCESS, X_OPERAND);
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
    exec_c_r(cmd, -1, 'R');             // Reverse of C command
}


///
///  @brief    Scan C, R, or L command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_C(struct cmd *cmd)
{
    assert(cmd != NULL);

    default_n(cmd, (int_t)1);           // C => 1C, R => 1R, L => 1L
    reject_m(cmd->m_set);
    reject_dcolon(cmd->dcolon);
    reject_atsign(cmd->atsign);

    return false;
}


///
///  @brief    Scan J command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_J(struct cmd *cmd)
{
    assert(cmd != NULL);

    default_n(cmd, (int_t)0);           // J => 0J
    reject_m(cmd->m_set);
    reject_dcolon(cmd->dcolon);
    reject_atsign(cmd->atsign);

    return false;
}
