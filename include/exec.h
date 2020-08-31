///
///  @file    exec.h
///  @brief   Header file for parsing and executing TECO commands.
///
///  @bug     No known bugs.
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

#if     !defined(_EXEC_H)

#define _EXEC_H

#include "teco.h"               //lint !e451 !e537
#include "cmd.h"


// Functions that directly execute commands

extern void exec_A(struct cmd *cmd);

extern void exec_apos(struct cmd *cmd);

extern void exec_atsign(struct cmd *cmd);

extern void exec_B(struct cmd *cmd);

extern void exec_bad(struct cmd *cmd);

extern void exec_bslash(struct cmd *cmd);

extern void exec_bang(struct cmd *cmd);

extern void exec_C(struct cmd *cmd);

extern void exec_colon(struct cmd *cmd);

extern void exec_comma(struct cmd *cmd);

extern void exec_ctrl_A(struct cmd *cmd);

extern void exec_ctrl_C(struct cmd *cmd);

extern void exec_ctrl_D(struct cmd *cmd);

extern void exec_ctrl_B(struct cmd *cmd);

extern void exec_ctrl_E(struct cmd *cmd);

extern void exec_ctrl_H(struct cmd *cmd);

extern void exec_ctrl_I(struct cmd *cmd);

extern void exec_ctrl_N(struct cmd *cmd);

extern void exec_ctrl_O(struct cmd *cmd);

extern void exec_ctrl_Q(struct cmd *cmd);

extern void exec_ctrl_R(struct cmd *cmd);

extern void exec_ctrl_S(struct cmd *cmd);

extern void exec_ctrl_T(struct cmd *cmd);

extern void exec_ctrl_U(struct cmd *cmd);

extern void exec_ctrl_ubar(struct cmd *cmd);

extern void exec_ctrl_up(struct cmd *cmd);

extern void exec_ctrl_V(struct cmd *cmd);

extern void exec_ctrl_W(struct cmd *cmd);

extern void exec_ctrl_X(struct cmd *cmd);

extern void exec_ctrl_Y(struct cmd *cmd);

extern void exec_ctrl_Z(struct cmd *cmd);

extern void exec_D(struct cmd *cmd);

extern void exec_digit(struct cmd *cmd);

extern void exec_dot(struct cmd *cmd);

extern void exec_E1(struct cmd *cmd);

extern void exec_E2(struct cmd *cmd);

extern void exec_E3(struct cmd *cmd);

extern void exec_E4(struct cmd *cmd);

extern void exec_EA(struct cmd *cmd);

extern void exec_EB(struct cmd *cmd);

extern void exec_EC(struct cmd *cmd);

extern void exec_ED(struct cmd *cmd);

extern void exec_EE(struct cmd *cmd);

extern void exec_EF(struct cmd *cmd);

extern void exec_EG(struct cmd *cmd);

extern void exec_EH(struct cmd *cmd);

extern void exec_EI(struct cmd *cmd);

extern void exec_EJ(struct cmd *cmd);

extern void exec_EK(struct cmd *cmd);

extern void exec_EL(struct cmd *cmd);

extern void exec_EM(struct cmd *cmd);

extern void exec_EN(struct cmd *cmd);

extern void exec_EO(struct cmd *cmd);

extern void exec_EP(struct cmd *cmd);

extern void exec_EQ(struct cmd *cmd);

extern void exec_equals(struct cmd *cmd);

extern void exec_ER(struct cmd *cmd);

extern void exec_ES(struct cmd *cmd);

extern void exec_escape(struct cmd *cmd);

extern void exec_ET(struct cmd *cmd);

extern void exec_EU(struct cmd *cmd);

extern void exec_EV(struct cmd *cmd);

extern void exec_EW(struct cmd *cmd);

extern void exec_EX(struct cmd *cmd);

extern void exec_EY(struct cmd *cmd);

extern void exec_E_ubar(struct cmd *cmd);

extern void exec_F1(struct cmd *cmd);

extern void exec_F2(struct cmd *cmd);

extern void exec_F3(struct cmd *cmd);

extern void exec_FB(struct cmd *cmd);

extern void exec_FC(struct cmd *cmd);

extern void exec_FD(struct cmd *cmd);

extern void exec_FK(struct cmd *cmd);

extern void exec_FL(struct cmd *cmd);

extern void exec_FN(struct cmd *cmd);

extern void exec_FR(struct cmd *cmd);

extern void exec_FS(struct cmd *cmd);

extern void exec_FU(struct cmd *cmd);

extern void exec_F_apos(struct cmd *cmd);

extern void exec_F_lt(struct cmd *cmd);

extern void exec_F_gt(struct cmd *cmd);

extern void exec_F_ubar(struct cmd *cmd);

extern void exec_F_vbar(struct cmd *cmd);

extern void exec_G(struct cmd *cmd);

extern void exec_gt(struct cmd *cmd);

extern void exec_H(struct cmd *cmd);

extern void exec_I(struct cmd *cmd);

extern void exec_insert(const char *buf, uint len);

extern void exec_J(struct cmd *cmd);

extern void exec_K(struct cmd *cmd);

extern void exec_L(struct cmd *cmd);

extern void exec_lt(struct cmd *cmd);

extern void exec_lbracket(struct cmd *cmd);

extern void exec_lparen(struct cmd *cmd);

extern void exec_M(struct cmd *cmd);

extern void exec_N(struct cmd *cmd);

extern void exec_O(struct cmd *cmd);

extern void exec_oper(struct cmd *cmd);

extern void exec_P(struct cmd *cmd);

extern void exec_pct(struct cmd *cmd);

extern void exec_Q(struct cmd *cmd);

extern void exec_quote(struct cmd *cmd);

extern void exec_R(struct cmd *cmd);

extern void exec_rbracket(struct cmd *cmd);

extern void exec_rparen(struct cmd *cmd);

extern void exec_S(struct cmd *cmd);

extern void exec_semi(struct cmd *cmd);

extern void exec_T(struct cmd *cmd);

extern void exec_trace(struct cmd *cmd);

extern void exec_U(struct cmd *cmd);

extern void exec_ubar(struct cmd *cmd);

extern void exec_V(struct cmd *cmd);

extern void exec_vbar(struct cmd *cmd);

extern void exec_W(struct cmd *cmd);

extern void exec_X(struct cmd *cmd);

extern void exec_Z(struct cmd *cmd);

extern bool exec_xoper(int c);

extern void exec_Y(struct cmd *cmd);

// Helper functions for executing commands

extern bool append(bool n_set, int n_arg, bool colon_set);

extern bool append_line(void);

extern bool check_semi(void);

extern void exec_cmd(void);

extern void exec_macro(struct buffer *macro);

extern void exit_EG(void);

extern int find_eg(char *buf, bool reset);

extern void init_EG(void);

extern void init_EI(void);

extern void init_loop(void);

extern bool next_page(int start, int end, bool ff, bool yank);

extern bool next_yank(void);

extern int read_indirect(void);

extern void reset_if(void);

extern void reset_indirect(void);

extern void reset_loop(void);

#endif  // !defined(_EXEC_H)
