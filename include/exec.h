///
///  @file    exec.h
///  @brief   Header file for parsing and executing TECO commands.
///
///  *** Automatically generated file. DO NOT MODIFY. ***
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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

// Comment out the following to relax syntax checking and improve speed.

#define TECO_STRICT             ///< Enable syntax checking macros

#if     defined(TECO_STRICT)

///  @def    reject_atsign
///  @brief  Error if at sign and command doesn't allow it.

#define reject_atsign(cmd) if (f.e2.atsign && cmd->atsign) throw(E_ATS)

///  @def    reject_colon
///  @brief  Error if colon and command doesn't allow it.

#define reject_colon(cmd)  if (f.e2.colon && cmd->colon) throw(E_COL)

///  @def    reject_dcolon
///  @brief  Error if dcolon and command doesn't allow it.

#define reject_dcolon(cmd) if (f.e2.colon && cmd->dcolon) throw(E_COL)

///  @def    reject_m
///  @brief  Error if m argument and command doesn't allow it.

#define reject_m(cmd)  if (f.e2.m_arg && cmd->m_set) throw(E_IMA)

///  @def    reject_neg_m
///  @brief  Error if m argument is negative.

#define reject_neg_m(cmd)  if (cmd->m_set && cmd->m_arg < 0) throw(E_NCA)

///  @def    reject_neg_n
///  @brief  Error if n argument is negative.

#define reject_neg_n(cmd)  if (cmd->n_set && cmd->n_arg < 0) throw(E_NCA)

///  @def    reject_n
///  @brief  Error if n argument and command doesn't allow it.

#define reject_n(cmd)  if (f.e2.n_arg && cmd->n_set) throw(E_INA)

///  @def    require_n
///  @brief  Error if m argument not followed by n argument.

#define require_n(cmd)  if (cmd->m_set && !cmd->n_set) throw(E_NON)

#else

#define reject_atsign(cmd)
#define reject_colon(cmd)
#define reject_dcolon(cmd)
#define reject_m(cmd)
#define reject_neg_m(cmd)
#define reject_neg_n(cmd)
#define reject_n(cmd)
#define require_n(cmd)

#endif


///  @struct cmd
///  @brief  Command block structure.

struct cmd
{
    char c1;                        ///< 1st command character
    char c2;                        ///< 2nd command character
    char c3;                        ///< 3rd command character
    char qname;                     ///< Q-register name
    bool qlocal;                    ///< Q-register is local
    int qindex;                     ///< Q-register index (if not -1)
    bool m_set;                     ///< m argument is valid
    int_t m_arg;                    ///< m argument
    bool n_set;                     ///< n argument is valid
    int_t n_arg;                    ///< n argument
    bool h;                         ///< H found
    bool ctrl_y;                    ///< CTRL/Y found
    bool colon;                     ///< : found
    bool dcolon;                    ///< :: found
    bool atsign;                    ///< @ found
    char delim;                     ///< Delimiter for @ modifier
    struct tstring text1;           ///< 1st text string
    struct tstring text2;           ///< 2nd text string
};

// Global variables

extern char eg_command[];

extern char *eg_result;

extern uint nparens;

extern const struct cmd null_cmd;

// Functions that scan commands

extern bool parse_1(struct cmd *cmd);

extern bool parse_2(struct cmd *cmd);

extern bool parse_M(struct cmd *cmd);

extern bool parse_M1(struct cmd *cmd);

extern bool parse_Mc(struct cmd *cmd);

extern bool parse_Mc1(struct cmd *cmd);

extern bool parse_Mc2(struct cmd *cmd);

extern bool parse_Mcq(struct cmd *cmd);

extern bool parse_Mcq1(struct cmd *cmd);

extern bool parse_N1(struct cmd *cmd);

extern bool parse_X(struct cmd *cmd);

extern bool parse_c(struct cmd *cmd);

extern bool parse_c1(struct cmd *cmd);

extern bool parse_cq1(struct cmd *cmd);

extern bool parse_d1(struct cmd *cmd);

extern bool parse_escape(struct cmd *cmd);

extern bool parse_flag1(struct cmd *cmd);

extern bool parse_flag2(struct cmd *cmd);

extern bool parse_m2(struct cmd *cmd);

extern bool parse_mc(struct cmd *cmd);

extern bool parse_mc1(struct cmd *cmd);

extern bool parse_mc2(struct cmd *cmd);

extern bool parse_mcq(struct cmd *cmd);

extern bool parse_md1(struct cmd *cmd);

extern bool parse_md2(struct cmd *cmd);

extern bool parse_mq(struct cmd *cmd);

extern bool parse_n(struct cmd *cmd);

extern bool parse_nc(struct cmd *cmd);

extern bool parse_nc1(struct cmd *cmd);

extern bool parse_ncq(struct cmd *cmd);

extern bool parse_oper(struct cmd *cmd);

extern bool parse_q1(struct cmd *cmd);

extern bool scan_A(struct cmd *cmd);

extern bool scan_B(struct cmd *cmd);

extern bool scan_EJ(struct cmd *cmd);

extern bool scan_F0(struct cmd *cmd);

extern bool scan_FH(struct cmd *cmd);

extern bool scan_FZ(struct cmd *cmd);

extern bool scan_H(struct cmd *cmd);

extern bool scan_P(struct cmd *cmd);

extern bool scan_Q(struct cmd *cmd);

extern bool scan_W(struct cmd *cmd);

extern bool scan_Z(struct cmd *cmd);

extern bool scan_atsign(struct cmd *cmd);

extern bool scan_bad(struct cmd *cmd);

extern bool scan_bang(struct cmd *cmd);

extern bool scan_colon(struct cmd *cmd);

extern bool scan_comma(struct cmd *cmd);

extern bool scan_ctrl_B(struct cmd *cmd);

extern bool scan_ctrl_H(struct cmd *cmd);

extern bool scan_ctrl_P(struct cmd *cmd);

extern bool scan_ctrl_Q(struct cmd *cmd);

extern bool scan_ctrl_S(struct cmd *cmd);

extern bool scan_ctrl_Y(struct cmd *cmd);

extern bool scan_ctrl_Z(struct cmd *cmd);

extern bool scan_ctrl_ubar(struct cmd *cmd);

extern bool scan_ctrl_up(struct cmd *cmd);

extern bool scan_div(struct cmd *cmd);

extern bool scan_dot(struct cmd *cmd);

extern bool scan_equals(struct cmd *cmd);

extern bool scan_gt(struct cmd *cmd);

extern bool scan_lparen(struct cmd *cmd);

extern bool scan_lt(struct cmd *cmd);

extern bool scan_nop(struct cmd *cmd);

extern bool scan_number(struct cmd *cmd);

extern bool scan_oper(struct cmd *cmd);

extern bool scan_pct(struct cmd *cmd);

extern bool scan_quote(struct cmd *cmd);

extern bool scan_rparen(struct cmd *cmd);

extern bool scan_tilde(struct cmd *cmd);


// Functions that execute commands

extern void exec_A(struct cmd *cmd);

extern void exec_C(struct cmd *cmd);

extern void exec_D(struct cmd *cmd);

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

extern void exec_EK(struct cmd *cmd);

extern void exec_EL(struct cmd *cmd);

extern void exec_EN(struct cmd *cmd);

extern void exec_EO(struct cmd *cmd);

extern void exec_EP(struct cmd *cmd);

extern void exec_EQ(struct cmd *cmd);

extern void exec_ER(struct cmd *cmd);

extern void exec_ES(struct cmd *cmd);

extern void exec_ET(struct cmd *cmd);

extern void exec_EU(struct cmd *cmd);

extern void exec_EV(struct cmd *cmd);

extern void exec_EW(struct cmd *cmd);

extern void exec_EX(struct cmd *cmd);

extern void exec_EY(struct cmd *cmd);

extern void exec_E_pct(struct cmd *cmd);

extern void exec_E_ubar(struct cmd *cmd);

extern void exec_F1(struct cmd *cmd);

extern void exec_F2(struct cmd *cmd);

extern void exec_F3(struct cmd *cmd);

extern void exec_FB(struct cmd *cmd);

extern void exec_FC(struct cmd *cmd);

extern void exec_FD(struct cmd *cmd);

extern void exec_FK(struct cmd *cmd);

extern void exec_FL(struct cmd *cmd);

extern void exec_FM(struct cmd *cmd);

extern void exec_FN(struct cmd *cmd);

extern void exec_FQ(struct cmd *cmd);

extern void exec_FR(struct cmd *cmd);

extern void exec_FS(struct cmd *cmd);

extern void exec_FU(struct cmd *cmd);

extern void exec_F_apos(struct cmd *cmd);

extern void exec_F_gt(struct cmd *cmd);

extern void exec_F_lt(struct cmd *cmd);

extern void exec_F_ubar(struct cmd *cmd);

extern void exec_F_vbar(struct cmd *cmd);

extern void exec_G(struct cmd *cmd);

extern void exec_I(struct cmd *cmd);

extern void exec_J(struct cmd *cmd);

extern void exec_K(struct cmd *cmd);

extern void exec_L(struct cmd *cmd);

extern void exec_M(struct cmd *cmd);

extern void exec_N(struct cmd *cmd);

extern void exec_O(struct cmd *cmd);

extern void exec_P(struct cmd *cmd);

extern void exec_R(struct cmd *cmd);

extern void exec_S(struct cmd *cmd);

extern void exec_T(struct cmd *cmd);

extern void exec_U(struct cmd *cmd);

extern void exec_V(struct cmd *cmd);

extern void exec_W(struct cmd *cmd);

extern void exec_X(struct cmd *cmd);

extern void exec_Y(struct cmd *cmd);

extern void exec_apos(struct cmd *cmd);

extern void exec_bang(struct cmd *cmd);

extern void exec_bslash(struct cmd *cmd);

extern void exec_ctrl_A(struct cmd *cmd);

extern void exec_ctrl_C(struct cmd *cmd);

extern void exec_ctrl_D(struct cmd *cmd);

extern void exec_ctrl_E(struct cmd *cmd);

extern void exec_ctrl_I(struct cmd *cmd);

extern void exec_ctrl_O(struct cmd *cmd);

extern void exec_ctrl_R(struct cmd *cmd);

extern void exec_ctrl_T(struct cmd *cmd);

extern void exec_ctrl_U(struct cmd *cmd);

extern void exec_ctrl_V(struct cmd *cmd);

extern void exec_ctrl_W(struct cmd *cmd);

extern void exec_ctrl_X(struct cmd *cmd);

extern void exec_equals(struct cmd *cmd);

extern void exec_escape(struct cmd *cmd);

extern void exec_gt(struct cmd *cmd);

extern void exec_lbracket(struct cmd *cmd);

extern void exec_lt(struct cmd *cmd);

extern void exec_nop(struct cmd *cmd);

extern void exec_pct(struct cmd *cmd);

extern void exec_quote(struct cmd *cmd);

extern void exec_rbracket(struct cmd *cmd);

extern void exec_semi(struct cmd *cmd);

extern void exec_trace(struct cmd *cmd);

extern void exec_ubar(struct cmd *cmd);

extern void exec_vbar(struct cmd *cmd);


// Helper functions for executing commands

extern bool append(bool n_set, int_t n_arg, bool colon_set);

extern bool append_line(void);

extern int check_EI(void);

extern bool check_semi(void);

extern void close_files(void);

extern void exec_cmd(struct cmd *cmd);

extern void exec_macro(struct buffer *macro, struct cmd *cmd);

extern void exit_EG(void);

extern void exec_insert(const char *buf, uint len);

extern int find_eg(char *buf, bool reset);

extern void init_EG(void);

extern void init_EI(void);

extern void init_loop(void);

extern bool next_page(int start, int end, bool ff, bool yank);

extern bool next_yank(void);

extern bool read_EI(void);

extern void reset_if(void);

extern void reset_indirect(void);

extern void reset_loop(void);

extern bool skip_cmd(struct cmd *cmd, const char *skip);

extern void scan_texts(struct cmd *cmd, int ntexts, int delim);

#endif  // !defined(_EXEC_H)
