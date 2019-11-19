///
///  @file    stubs.c
///  @brief   Temporary stub functions for TECO.
///
///  @author  Nowwith Treble Software
///
///  @bug     No known bugs.
///
///  @copyright  tbd
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

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "exec.h"


void exec_C(struct cmd *cmd)
{
    printf("C command\r\n");

    return;
}


void exec_D(struct cmd *cmd)
{
    printf("D command\r\n");

    return;
}


void exec_G(struct cmd *cmd)
{
    printf("G command\r\n");

    return;
}


void exec_J(struct cmd *cmd)
{
    printf("J command\r\n");

    return;
}


void exec_K(struct cmd *cmd)
{
    printf("K command\r\n");

    return;
}


void exec_L(struct cmd *cmd)
{
    printf("L command\r\n");

    return;
}


void exec_M(struct cmd *cmd)
{
    printf("M command\r\n");

    return;
}


void exec_Q(struct cmd *cmd)
{
    printf("Q command\r\n");

    return;
}


void exec_R(struct cmd *cmd)
{
    printf("R command\r\n");

    return;
}


void exec_T(struct cmd *cmd)
{
    printf("T command\r\n");

    return;
}


void exec_U(struct cmd *cmd)
{
    printf("U command\r\n");

    return;
}


void exec_V(struct cmd *cmd)
{
    printf("V command\r\n");

    return;
}


void exec_W(struct cmd *cmd)
{
    printf("W command\r\n");

    return;
}


void exec_X(struct cmd *cmd)
{
    printf("X command\r\n");

    return;
}


void exec_apos(struct cmd *cmd)
{
    printf("' command\r\n");

    return;
}


void exec_backslash(struct cmd *cmd)
{
    printf("\\ command\r\n");

    return;
}


void exec_ctrl_c(struct cmd *cmd)
{
    printf("ctrl_c command\r\n");

    return;
}


void exec_ctrl_caret(struct cmd *cmd)
{
    printf("ctrl_^ command\r\n");

    return;
}


void exec_ctrl_l(struct cmd *cmd)
{
    printf("ctrl_l command\r\n");

    return;
}


void exec_ctrl_p(struct cmd *cmd)
{
    // TODO: use for debugging?

    printf("ctrl_p command\r\n");

    return;
}


void exec_ctrl_q(struct cmd *cmd)
{
    printf("ctrl_q command\r\n");

    return;
}


void exec_ctrl_u(struct cmd *cmd)
{
    printf("ctrl_u command\r\n");

    return;
}


void exec_ctrl_v(struct cmd *cmd)
{
    printf("ctrl_v command\r\n");

    return;
}


void exec_ctrl_w(struct cmd *cmd)
{
    printf("ctrl_w command\r\n");

    return;
}


void exec_ctrl_z(struct cmd *cmd)
{
    printf("ctrl_z command\r\n");

    return;
}


void exec_lbracket(struct cmd *cmd)
{
    printf("[ command\r\n");

    return;
}


void exec_langle(struct cmd *cmd)
{
    printf("< command\r\n");

    return;
}


void exec_pct(struct cmd *cmd)
{
    printf("%% command\r\n");

    return;
}


void exec_question(struct cmd *cmd)
{
    printf("? command\r\n");

    return;
}


void exec_rangle(struct cmd *cmd)
{
    printf("> command\r\n");

    return;
}


void exec_rbracket(struct cmd *cmd)
{
    printf("] command\r\n");

    return;
}


void exec_semi(struct cmd *cmd)
{
    printf("; command\r\n");

    return;
}


void exec_ubar(struct cmd *cmd)
{
    printf("_ command\r\n");

    return;
}


void exec_vbar(struct cmd *cmd)
{
    printf("| command\r\n");

    return;
}



void exec_nul(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->expr.len > 0)
    {
        return;
    }
    else
    {
        return;
    }
}


void exec_ctrl_ubar(struct cmd *cmd)
{
    assert(cmd != NULL);

    push_expr(US, EXPR_OPERATOR);

    cmd->state = CMD_EXPR;

    return;
}


void exec_operator(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->c1 == ')' && !operand_expr()) // Is there an operand available?
    {
        print_err(E_NAP);               // No argument before )
    }

    push_expr(cmd->c1, EXPR_OPERATOR);

    cmd->state = CMD_EXPR;

    return;
}


void exec_B(struct cmd *cmd)
{
    assert(cmd != NULL);

    printf("B command\r\n");

    return;
}


void exec_H(struct cmd *cmd)
{
    assert(cmd != NULL);

    printf("H command\r\n");

    return;
}


void exec_Z(struct cmd *cmd)
{
    assert(cmd != NULL);

    printf("Z command\r\n");

    return;
}


void exec_ctrl_n(struct cmd *cmd)
{
    assert(cmd != NULL);

    printf("ctrl_n command\r\n");

    return;
}


void exec_ctrl_s(struct cmd *cmd)
{
    assert(cmd != NULL);

    printf("ctrl_s command\r\n");

    return;
}


void exec_ctrl_y(struct cmd *cmd)
{
    assert(cmd != NULL);

    printf("ctrl_y command\r\n");

    return;
}


void exec_dot(struct cmd *cmd)
{
    assert(cmd != NULL);

    printf(". command\r\n");

    return;
}



void exec_FB(struct cmd *cmd)
{
    assert(cmd != NULL);

    cmd->opt_colon  = true;
    cmd->opt_atsign = true;

    scan_cmd(cmd);

    return;
}


void exec_F_apos(struct cmd *cmd)
{
    assert(cmd != NULL);

    printf("F%% command\r\n");

    return;
}


void exec_F_langle(struct cmd *cmd)
{
    assert(cmd != NULL);

    printf("F< command\r\n");

    return;
}


void exec_F_rangle(struct cmd *cmd)
{
    assert(cmd != NULL);

    printf("F> command\r\n");

    return;
}


void exec_F_vbar(struct cmd *cmd)
{
    assert(cmd != NULL);

    printf("F| command\r\n");

    return;
}

