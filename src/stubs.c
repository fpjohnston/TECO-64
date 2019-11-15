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

#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "ascii.h"
#include "exec.h"


void exec_A(void)
{
    printf("A command\r\n");
}


void exec_B(void)
{
    printf("B command\r\n");
}


void exec_C(void)
{
    printf("C command\r\n");
}


void exec_D(void)
{
    printf("D command\r\n");
}


void exec_G(void)
{
    printf("G command\r\n");
}


void exec_H(void)
{
    printf("H command\r\n");
}


void exec_J(void)
{
    printf("J command\r\n");
}


void exec_K(void)
{
    printf("K command\r\n");
}


void exec_L(void)
{
    printf("L command\r\n");
}


void exec_M(void)
{
    printf("M command\r\n");
}


void exec_N(void)
{
    printf("N command\r\n");
}


void exec_O(void)
{
    printf(" command\r\n");
}


void exec_P(void)
{
    printf("P command\r\n");
}


void exec_Q(void)
{
    printf("Q command\r\n");
}


void exec_R(void)
{
    printf("R command\r\n");
}


void exec_T(void)
{
    printf("T command\r\n");
}


void exec_U(void)
{
    printf("U command\r\n");
}


void exec_V(void)
{
    printf("V command\r\n");
}


void exec_W(void)
{
    printf("W command\r\n");
}


void exec_X(void)
{
    printf("X command\r\n");
}


void exec_Y(void)
{
    printf("Y command\r\n");
}


void exec_Z(void)
{
    printf("Z command\r\n");
}


void exec_backslash(void)
{
    printf("\\ command\r\n");
}


void exec_bad(void)
{
    printf("bad command\r\n");
}


void exec_ctrl_c(void)
{
    printf("ctrl_c command\r\n");
}


void exec_ctrl_caret(void)
{
    printf("ctrl_^ command\r\n");
}


void exec_ctrl_i(void)
{
    printf("TAB command\r\n");
}


void exec_ctrl_l(void)
{
    printf("ctrl_l command\r\n");
}


void exec_ctrl_n(void)
{
    printf("ctrl_n command\r\n");
}


void exec_ctrl_p(void)
{
    // TODO: use for debugging?

    printf("ctrl_p command\r\n");
}


void exec_ctrl_q(void)
{
    printf("ctrl_q command\r\n");
}


void exec_ctrl_s(void)
{
    printf("ctrl_s command\r\n");
}


void exec_ctrl_u(void)
{
    printf("ctrl_u command\r\n");
}


void exec_ctrl_ubar(void)
{
    push_expr(US, EXPR_OPERATOR);
}


void exec_ctrl_v(void)
{
    printf("ctrl_v command\r\n");
}


void exec_ctrl_w(void)
{
    printf("ctrl_w command\r\n");
}


void exec_ctrl_y(void)
{
    printf("ctrl_y command\r\n");
}


void exec_ctrl_z(void)
{
    printf("ctrl_z command\r\n");
}


void exec_dot(void)
{
    printf(". command\r\n");
}


void exec_escape(void)
{
}


void exec_lbracket(void)
{
    printf("[ command\r\n");
}


void exec_langle(void)
{
    printf("< command\r\n");
}


void exec_nul(void)
{
    printf(" command\r\n");
}


void exec_nyi(void)
{
    printf(" command\r\n");
}


void exec_operator(void)
{
    push_expr(last_c, EXPR_OPERATOR);
}


void exec_pct(void)
{
    printf("%% command\r\n");
}


void exec_question(void)
{
    printf("? command\r\n");
}


void exec_rangle(void)
{
    printf("> command\r\n");
}


void exec_rbracket(void)
{
    printf("] command\r\n");
}


void exec_semi(void)
{
    printf("; command\r\n");
}


void exec_ubar(void)
{
    printf("_ command\r\n");
}


void exec_vbar(void)
{
    printf("| command\r\n");
}



