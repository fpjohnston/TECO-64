///
///  @file    exec.h
///  @brief   Header file for parsing and executing TECO commands.
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

#if     !defined(_EXEC_H)

#define _EXEC_H

#if     !defined(_TECO_H)

#include "teco.h"

#endif

///  @struct  scan
///  @brief   Internal scanning state

struct scan
{
    bool dryrun;                    ///< "Dry run" scan (no changes)
    uint nparens;                   ///< No. of unmatched left parentheses
    int sum;                        ///< Accumulated sum of digits scanned
    bool digits;                    ///< Accumulated sum is valid
    bool expr;                      ///< Current command is part of expression
    bool mod;                       ///< Command modifier seen
    bool space;                     ///< Last chr. scanned was whitespace
    bool comma_set;                 ///< Comma seen in expression
    bool m_opt;                     ///< m argument allowed
    bool n_opt;                     ///< n argument allowed
    bool colon_opt;                 ///< Colon allowed in command
    bool dcolon_opt;                ///< Double colon allowed in command
    bool atsign_opt;                ///< At sign allowed in command
    bool w_opt;                     ///< W allowed (for P)
    bool q_register;                ///< Q-register required
    bool t1_opt;                    ///< 1st text string allowed in command
    bool t2_opt;                    ///< 2nd text string allowed in command
};

extern struct scan scan;

///  @struct cmd
///  @brief  Command block structure.

struct cmd
{
    char c1;                        ///< 1st command character
    char c2;                        ///< 2nd command character (or NUL)
    char c3;                        ///< 3rd command character (or NUL)
    char qname;                     ///< Q-register name
    bool qlocal;                    ///< If true, Q-register is local
    int m_arg;                      ///< m argument
    int n_arg;                      ///< n argument
    bool m_set;                     ///< m argument found
    bool n_set;                     ///< n argument found
    bool h_set;                     ///< H found
    bool w_set;                     ///< W found
    bool colon_set;                 ///< : found
    bool dcolon_set;                ///< :: found
    bool atsign_set;                ///< @ found
    char delim;                     ///< Delimiter for @ modifier
    struct tstring expr;            ///< Expression string
    struct tstring text1;           ///< 1st text string
    struct tstring text2;           ///< 2nd text string
};

///  @typedef  scan_func
///  @brief   Function to scan command.

typedef void (scan_func)(struct cmd *cmd);

///  @typedef exec_func
///  @brief   Function to execute command.

typedef void (exec_func)(struct cmd *cmd);

///  @struct cmd_table
///  @brief  Format of command tables used to parse and execute commands.

struct cmd_table
{
    scan_func *scan;                ///< Scan function
    exec_func *exec;                ///< Execution function
    const char *opts;               ///< Command modifiers and options
};

extern const struct cmd_table cmd_table[];

extern const struct cmd_table cmd_e_table[];

extern const struct cmd_table cmd_f_table[];

extern const uint cmd_count;

extern const uint cmd_e_count;

extern const uint cmd_f_count;

extern bool cmd_expr;

// Functions that assist in parsing commands

extern bool append(bool n_set, int n_arg, bool colon_set);

extern bool append_line(void);

extern bool next_page(uint start, uint end, bool ff, bool yank);

// Functions that execute commands

extern void exec_C(struct cmd *cmd);

extern void exec_D(struct cmd *cmd);

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

extern void exec_X(struct cmd *cmd);

extern void exec_Y(struct cmd *cmd);

extern void exec_apos(struct cmd *cmd);

extern void exec_bang(struct cmd *cmd);

extern void exec_ctrl_a(struct cmd *cmd);

extern void exec_ctrl_c(struct cmd *cmd);

extern void exec_ctrl_d(struct cmd *cmd);

extern void exec_ctrl_i(struct cmd *cmd);

extern void exec_ctrl_o(struct cmd *cmd);

extern void exec_ctrl_u(struct cmd *cmd);

extern void exec_ctrl_v(struct cmd *cmd);

extern void exec_ctrl_w(struct cmd *cmd);

extern void exec_equals(struct cmd *cmd);

extern void exec_escape(struct cmd *cmd);

extern void exec_insert(const char *buf, uint len);

extern void exec_lt(struct cmd *cmd);

extern void exec_lbracket(struct cmd *cmd);

extern void exec_question(struct cmd *cmd);

extern void exec_quote(struct cmd *cmd);

extern void exec_gt(struct cmd *cmd);

extern void exec_rbracket(struct cmd *cmd);

extern void exec_semi(struct cmd *cmd);

extern void exec_ubar(struct cmd *cmd);

extern void exec_vbar(struct cmd *cmd);

// Functions that handle E commands

extern void exec_EA(struct cmd *cmd);

extern void exec_EB(struct cmd *cmd);

extern void exec_EC(struct cmd *cmd);

extern void exec_EF(struct cmd *cmd);

extern void exec_EG(struct cmd *cmd);

extern void exec_EI(struct cmd *cmd);

extern void exec_EK(struct cmd *cmd);

extern void exec_EL(struct cmd *cmd);

extern void exec_EM(struct cmd *cmd);

extern void exec_EN(struct cmd *cmd);

extern void exec_EP(struct cmd *cmd);

extern void exec_EQ(struct cmd *cmd);

extern void exec_ER(struct cmd *cmd);

extern void exec_EW(struct cmd *cmd);

extern void exec_EX(struct cmd *cmd);

extern void exec_EY(struct cmd *cmd);

extern void exec_E_pct(struct cmd *cmd);

extern void exec_E_ubar(struct cmd *cmd);

// Functions that handle F commands

extern void exec_FB(struct cmd *cmd);

extern void exec_FC(struct cmd *cmd);

extern void exec_FD(struct cmd *cmd);

extern void exec_FK(struct cmd *cmd);

extern void exec_FN(struct cmd *cmd);

extern void exec_FR(struct cmd *cmd);

extern void exec_FS(struct cmd *cmd);

extern void exec_F_apos(struct cmd *cmd);

extern void exec_F_lt(struct cmd *cmd);

extern void exec_F_gt(struct cmd *cmd);

extern void exec_F_ubar(struct cmd *cmd);

extern void exec_F_vbar(struct cmd *cmd);

// Functions that may be part of an expression

extern void scan_A(struct cmd *cmd);

extern void scan_B(struct cmd *cmd);

extern void scan_bad(struct cmd *cmd);

extern void scan_back(struct cmd *cmd);

extern void scan_brace(struct cmd *cmd);

extern void scan_comma(struct cmd *cmd);

extern void scan_ctrl_b(struct cmd *cmd);

extern void scan_ctrl_e(struct cmd *cmd);

extern void scan_ctrl_f(struct cmd *cmd);

extern void scan_ctrl_h(struct cmd *cmd);

extern void scan_ctrl_n(struct cmd *cmd);

extern void scan_ctrl_q(struct cmd *cmd);

extern void scan_ctrl_r(struct cmd *cmd);

extern void scan_ctrl_s(struct cmd *cmd);

extern void scan_ctrl_t(struct cmd *cmd);

extern void scan_ctrl_x(struct cmd *cmd);

extern void scan_ctrl_y(struct cmd *cmd);

extern void scan_ctrl_z(struct cmd *cmd);

extern void scan_digit(struct cmd *cmd);

extern void scan_dot(struct cmd *cmd);

extern void scan_E1(struct cmd *cmd);

extern void scan_ED(struct cmd *cmd);

extern void scan_EE(struct cmd *cmd);

extern void scan_EH(struct cmd *cmd);

extern void scan_EJ(struct cmd *cmd);

extern void scan_EO(struct cmd *cmd);

extern void scan_ES(struct cmd *cmd);

extern void scan_ET(struct cmd *cmd);

extern void scan_EU(struct cmd *cmd);

extern void scan_EV(struct cmd *cmd);

extern void scan_EZ(struct cmd *cmd);

extern void scan_H(struct cmd *cmd);

extern void scan_mod(struct cmd *cmd);

extern void scan_operator(struct cmd *cmd);

extern exec_func *scan_pass1(struct cmd *cmd);

extern void scan_pct(struct cmd *cmd);

extern void scan_Q(struct cmd *cmd);

extern void scan_tail(struct cmd *cmd);

extern void scan_tilde(struct cmd *cmd);

extern void scan_W(struct cmd *cmd);

extern void scan_Z(struct cmd *cmd);

// Miscellaneous functions

extern bool check_indirect(void);

extern void close_indirect(void);

extern void exit_EG(void);

extern void init_EG(void);
 
extern void init_loop(void);

extern void log_cmd(struct cmd *cmd);

extern exec_func *next_cmd(struct cmd *cmd);

extern int open_indirect(bool default_type);

extern int open_output(const struct cmd *cmd, uint stream);

extern void reset_if(void);

extern bool read_indirect(void);

extern void reset_loop(void);

extern void reset_scan(void);

extern bool test_indirect(void);

#endif  // _EXEC_H
