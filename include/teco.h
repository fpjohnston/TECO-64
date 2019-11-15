///
///  @file    teco.h
///  @brief   Main header file for TECO text editor.
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

#if     !defined(_SETJMP_H)
#include <setjmp.h>
#endif

#if     !defined(_STDBOOL_H)
#include <stdbool.h>
#endif

#define countof(array) (sizeof(array) / sizeof(array[0]))

#define WAIT        true                // Wait for terminal input
#define CMDBUFSIZ   1000                // Command buffer size

enum expr_type
{
    EXPR_NONE,
    EXPR_OPERATOR,
    EXPR_OPERAND
};

enum mod_flag
{
    MOD_A = 1,                          // @ allowed
    MOD_C = 2,                          // : allowed
    MOD_D = 4,                          // :: allowed
    MOD_M = 8,                          // m argument allowed
    MOD_N = 16                          // n argument allowed
};

struct qreg
{
    char *start;                // Start of Q-register
    char *end;                  // End of Q-register
    int value;                  // Value of Q-register
};

struct tstr
{
    const char *str;            // Start of string
    uint count;                 // No. of characters
};

// Define the command block structure.

struct cmd
{
    char name;                  // Command name
    char sub;                   // Sub command (or NUL)
    uint level;                 // Nesting level
    struct tstr arg1;           // 1st argument
    struct tstr arg2;           // 2nd argument
    struct cmd *next;           // Next command (or NULL)
    struct cmd *prev;           // Previous command (or NULL)
    void (*exec)(struct cmd *cmd); // Execution function
};

extern struct cmd cmd;          // TODO: temporary

// Global variables

extern int teco_version;

extern jmp_buf jump_main;

extern jmp_buf jump_command;

extern const char *prompt;

extern int last_c;

extern int radix;

extern int ctrl_x;

extern int m_arg;

extern bool trace_mode;

extern uint rows;

extern uint cols;

extern int form_feed;

// Global functions

extern void check_errno(int ecode);

extern void check_mod(int c);

extern void check_zero(int ecode);

extern void echo_chr(int c);

extern void exec_cmd(void);

extern int get_flag(int flag);

extern void init_env(void);

extern void init_qreg(void);

extern void init_term(void);

extern int getc_term(bool nowait);

extern void print_prompt(void);

extern void print_term(const char *str);

extern void put_bell(void);

extern void putc_term(int c);

extern void puts_term(const char *str, unsigned int nbytes);

extern void read_cmd(void);

extern int read_term(void);

extern int teco_env(int n);

extern void write_term(const char *buf, unsigned int nbytes);

extern struct qreg *get_qreg(int qname, bool qlocal);

extern int get_delim(int delim);

extern void get_arg(int delim, struct tstr *tstr);

extern void get_cmd(int delim, int ncmds, struct cmd *cmd);

// Functions that skip to the next command

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

// Functions that access the expression stack

extern bool empty_expr(void);

extern int get_n_arg(void);

extern void init_expr(void);

extern const char *next_cmd(void);

extern bool operand_expr(void);

extern void push_expr(int c, enum expr_type type);

extern int scan_cmd(int delim);

extern void type_cmd(const char *p, int len);

// Command buffer functions

extern char *clone_cmd(int delim);

extern char *copy_cmd(void);

extern int count_cmd(void);

extern int delete_cmd(void);

extern void echo_cmd(int pos);

extern bool empty_cmd(void);

extern int fetch_cmd(void);

extern void init_cmd(void);

extern int last_cmd(void);

extern bool match_cmd(const char *str);

extern void reset_cmd(void);

extern int start_cmd(void);

extern void store_cmd(int c);

extern void unfetch_cmd(int c);
