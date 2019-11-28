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


void exec_C(struct cmd *unused1)
{
    printf("C command\r\n");

    return;
}


void exec_D(struct cmd *unused1)
{
    printf("D command\r\n");

    return;
}


void exec_J(struct cmd *unused1)
{
    printf("J command\r\n");

    return;
}


void exec_K(struct cmd *unused1)
{
    printf("K command\r\n");

    return;
}


void exec_L(struct cmd *unused1)
{
    printf("L command\r\n");

    return;
}


void exec_M(struct cmd *unused1)
{
    printf("M command\r\n");

    return;
}


void exec_R(struct cmd *unused1)
{
    printf("R command\r\n");

    return;
}


void exec_T(struct cmd *unused1)
{
    printf("T command\r\n");

    return;
}


void exec_V(struct cmd *unused1)
{
    printf("V command\r\n");

    return;
}


void exec_W(struct cmd *unused1)
{
    printf("W command\r\n");

    return;
}


void exec_apos(struct cmd *unused1)
{
    printf("' command\r\n");

    return;
}


void exec_backslash(struct cmd *unused1)
{
    printf("\\ command\r\n");

    return;
}


void exec_ctrl_c(struct cmd *unused1)
{
    printf("ctrl_c command\r\n");

    return;
}


void exec_ctrl_caret(struct cmd *unused1)
{
    printf("ctrl_^ command\r\n");

    return;
}


void exec_ctrl_l(struct cmd *unused1)
{
    printf("ctrl_l command\r\n");

    return;
}


void exec_ctrl_p(struct cmd *unused1)
{
    // TODO: use for debugging?

    printf("ctrl_p command\r\n");

    return;
}


void exec_ctrl_q(struct cmd *unused1)
{
    printf("ctrl_q command\r\n");

    return;
}


void exec_ctrl_v(struct cmd *unused1)
{
    printf("ctrl_v command\r\n");

    return;
}


void exec_ctrl_w(struct cmd *unused1)
{
    printf("ctrl_w command\r\n");

    return;
}


void exec_langle(struct cmd *unused1)
{
    printf("< command\r\n");

    return;
}


void exec_question(struct cmd *unused1)
{
    printf("? command\r\n");

    return;
}


void exec_rangle(struct cmd *unused1)
{
    printf("> command\r\n");

    return;
}


void exec_semi(struct cmd *unused1)
{
    printf("; command\r\n");

    return;
}


void exec_ubar(struct cmd *unused1)
{
    printf("_ command\r\n");

    return;
}


void exec_vbar(struct cmd *unused1)
{
    printf("| command\r\n");

    return;
}


void exec_ctrl_ubar(struct cmd *unused1)
{
    push_expr(US, EXPR_OPERATOR);

    return;
}


void exec_B(struct cmd *unused1)
{
    push_expr(0, EXPR_OPERAND);

    return;
}


void exec_Z(struct cmd *unused1)
{
    push_expr(65536u, EXPR_OPERAND);

    return;
}


void exec_ctrl_n(struct cmd *unused1)
{
    printf("ctrl_n command\r\n");

    return;
}


void exec_ctrl_s(struct cmd *unused1)
{
    printf("ctrl_s command\r\n");

    return;
}


void exec_ctrl_y(struct cmd *unused1)
{
    printf("ctrl_y command\r\n");

    return;
}


void exec_dot(struct cmd *unused1)
{
    printf(". command\r\n");

    return;
}


void exec_FB(struct cmd *unused1)
{
    printf("FB command\r\n");

    return;
}


void exec_F_apos(struct cmd *unused1)
{
    printf("F%% command\r\n");

    return;
}


void exec_F_langle(struct cmd *unused1)
{
    printf("F< command\r\n");

    return;
}


void exec_F_rangle(struct cmd *unused1)
{
    printf("F> command\r\n");

    return;
}


void exec_F_vbar(struct cmd *unused1)
{
    printf("F| command\r\n");

    return;
}

