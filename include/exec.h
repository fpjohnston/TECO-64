///
///  @file    exec.h
///  @brief   Header file for parsing and executing TECO commands.
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

#if     !defined(_EXEC_H)

#define _EXEC_H

#if     !defined(_TECO_H)

#include "teco.h"

#endif

enum cmd_state
{
CMD_NULL,                       // Scanning for start of command
CMD_EXPR,                       // Scanning expression
CMD_MOD,                        // Scanning modifiers
CMD_DONE                        // Scanning is done
};

// Define the command block structure.

struct cmd
{
enum cmd_state state;           // State of command (for parsing)
uint level;                     // Command level

union
{
uint flag;                  // Command flag

struct
{
    uint opt_m      : 1;    // m argument allowed
    uint opt_n      : 1;    // n argument allowed
    uint opt_bits   : 1;    // m,n set & clear flag bits
    uint opt_colon  : 1;    // : allowed
    uint opt_dcolon : 1;    // :: allowed
    uint opt_atsign : 1;    // @ allowed
    uint opt_w      : 1;    // W allowed (for P)
    uint opt_qreg   : 1;    // Q-register required
    uint opt_t1     : 1;    // 1 text field allowed
    uint opt_t2     : 1;    // 2 text fields allowed
    uint got_m      : 1;    // m argument found
    uint got_n      : 1;    // n argument found
    uint got_colon  : 1;    // : found
    uint got_dcolon : 1;    // :: found
    uint got_atsign : 1;    // @ found
};
};

char c1, c2, c3;                // Command characters (or NUL)
int m, n;                       // m,n arguments
int paren;                      // No. of parentheses counted
char delim;                     // Delimiter for @ modifier
char qreg;                      // Q-register, if any
bool qlocal;                    // Q-register is local (not global)
struct tstr expr;               // Expression string
struct tstr text1;              // 1st text string
struct tstr text2;              // 2nd text string
struct cmd *next;               // Next command (or NULL)
struct cmd *prev;               // Previous command (or NULL)
};

struct cmd_table
{
void (*scan)(struct cmd *cmd);  // Scan function
void (*exec)(struct cmd *cmd);  // Execution function
const char *opts;               // Command modifiers and options
};

// Functions that assist in parsing commands

extern void get_flag(uint *flag, struct cmd *cmd);

extern const struct cmd_table *init_E(struct cmd *cmd);

extern const struct cmd_table *init_F(struct cmd *cmd);

// Functions that execute commands

extern void exec_A(struct cmd *cmd);

extern void exec_B(struct cmd *cmd);

extern void exec_C(struct cmd *cmd);

extern void exec_D(struct cmd *cmd);

extern void exec_G(struct cmd *cmd);

extern void exec_H(struct cmd *cmd);

extern void exec_I(struct cmd *cmd);

extern void exec_J(struct cmd *cmd);

extern void exec_K(struct cmd *cmd);

extern void exec_L(struct cmd *cmd);

extern void exec_M(struct cmd *cmd);

extern void exec_N(struct cmd *cmd);

extern void exec_O(struct cmd *cmd);

extern void exec_P(struct cmd *cmd);

extern void exec_Q(struct cmd *cmd);

extern void exec_R(struct cmd *cmd);

extern void exec_S(struct cmd *cmd);

extern void exec_T(struct cmd *cmd);

extern void exec_U(struct cmd *cmd);

extern void exec_V(struct cmd *cmd);

extern void exec_W(struct cmd *cmd);

extern void exec_X(struct cmd *cmd);

extern void exec_Y(struct cmd *cmd);

extern void exec_Z(struct cmd *cmd);

extern void exec_apos(struct cmd *cmd);

extern void exec_atsign(struct cmd *cmd);

extern void exec_backslash(struct cmd *cmd);

extern void exec_bang(struct cmd *cmd);

extern void exec_colon(struct cmd *cmd);

extern void exec_comma(struct cmd *cmd);

extern void exec_ctrl_a(struct cmd *cmd);

extern void exec_ctrl_b(struct cmd *cmd);

extern void exec_ctrl_c(struct cmd *cmd);

extern void exec_ctrl_caret(struct cmd *cmd);

extern void exec_ctrl_d(struct cmd *cmd);

extern void exec_ctrl_e(struct cmd *cmd);

extern void exec_ctrl_f(struct cmd *cmd);

extern void exec_ctrl_h(struct cmd *cmd);

extern void exec_ctrl_i(struct cmd *cmd);

extern void exec_ctrl_l(struct cmd *cmd);

extern void exec_ctrl_n(struct cmd *cmd);

extern void exec_ctrl_o(struct cmd *cmd);

extern void exec_ctrl_p(struct cmd *cmd);

extern void exec_ctrl_q(struct cmd *cmd);

extern void exec_ctrl_r(struct cmd *cmd);

extern void exec_ctrl_s(struct cmd *cmd);

extern void exec_ctrl_t(struct cmd *cmd);

extern void exec_ctrl_u(struct cmd *cmd);

extern void exec_ctrl_ubar(struct cmd *cmd);

extern void exec_ctrl_v(struct cmd *cmd);

extern void exec_ctrl_w(struct cmd *cmd);

extern void exec_ctrl_x(struct cmd *cmd);

extern void exec_ctrl_y(struct cmd *cmd);

extern void exec_ctrl_z(struct cmd *cmd);

extern int exec_digit(const char *str, int nchrs);

extern void exec_dot(struct cmd *cmd);

extern void exec_equals(struct cmd *cmd);

extern void exec_escape(struct cmd *cmd);

extern void exec_langle(struct cmd *cmd);

extern void exec_lbracket(struct cmd *cmd);

extern void exec_nul(struct cmd *cmd);

extern void exec_nyi(struct cmd *cmd);

extern void exec_operator(struct cmd *cmd);

extern void exec_pct(struct cmd *cmd);

extern void exec_question(struct cmd *cmd);

extern void exec_quote(struct cmd *cmd);

extern void exec_rangle(struct cmd *cmd);

extern void exec_rbracket(struct cmd *cmd);

extern void exec_rparen(struct cmd *cmd);

extern void exec_semi(struct cmd *cmd);

extern void exec_ubar(struct cmd *cmd);

extern void exec_vbar(struct cmd *cmd);

// Functions that handle E commands

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

extern void exec_ER(struct cmd *cmd);

extern void exec_ES(struct cmd *cmd);

extern void exec_ET(struct cmd *cmd);

extern void exec_EU(struct cmd *cmd);

extern void exec_EV(struct cmd *cmd);

extern void exec_EW(struct cmd *cmd);

extern void exec_EX(struct cmd *cmd);

extern void exec_EY(struct cmd *cmd);

extern void exec_EZ(struct cmd *cmd);

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

extern void exec_F_langle(struct cmd *cmd);

extern void exec_F_rangle(struct cmd *cmd);

extern void exec_F_ubar(struct cmd *cmd);

extern void exec_F_vbar(struct cmd *cmd);

// Functions that parse a command string.

extern void scan_bad(struct cmd *cmd);

extern void scan_cmd(struct cmd *cmd);

extern void scan_flag(struct cmd *cmd);

extern void print_cmd(struct cmd *cmd);

extern void scan_done(struct cmd *cmd);

extern void scan_expr(struct cmd *cmd);

extern void scan_mod(struct cmd *cmd);

extern void scan_null(struct cmd *cmd);

// Functions that skip to the next command.

extern void skip_arg1(int c);

extern void skip_arg2(void);

extern void skip_caret(void);

extern void skip_cmd(void);

extern void skip_ctrl_a(void);

extern void skip_ctrl_u(void);

extern void skip_e(void);

extern void skip_esc(void);

extern void skip_f(void);

extern void skip_one(void);

extern void skip_quote(void);

extern void skip_tag(void);

#endif  // _EXEC_H
