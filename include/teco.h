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

#if     !defined(_TECO_H)

#define _TECO_H

#if     !defined(_SETJMP_H)
#include <setjmp.h>
#endif

#if     !defined(_STDBOOL_H)
#include <stdbool.h>
#endif

#include "cmd.h"

#define countof(array) (sizeof(array) / sizeof(array[0]))

#define WAIT        true                // Wait for terminal input
#define CMDBUFSIZ   1000                // Command buffer size

#define EXPR_SIZE       64

enum expr_type
{
    EXPR_NONE,
    EXPR_OPERATOR,
    EXPR_OPERAND
};

struct expr
{
    long item;
    enum expr_type type;
};

struct expr_stack
{
    struct expr stack[EXPR_SIZE];
    uint len;
};

extern struct expr_stack expr;

struct qreg
{
    char *start;                // Start of Q-register
    char *end;                  // End of Q-register
    int value;                  // Value of Q-register
};

struct tstr
{
    const char *buf;            // Start of string
    uint len;                   // No. of characters
};


//
//  The following bit-encoded flags defined the available options for each
//  command. These are OR'd together in dispatch tables to determine how par-
//  sing and execution should proceed.
//
//  Note: no given command uses more than three or four of these.
//

enum cmd_opts
{
    _A  = (1 << 0),                 // @ modifier allowed
    _C  = (1 << 1),                 // : modifier allowed
    _D  = (1 << 2),                 // :: modifier allowed
    _N  = (1 << 3),                 // n argument allowed
    _MN = (1 << 4) | _N,            // m and n arguments allowed
    _Q  = (1 << 5),                 // Q-register required
    _T1 = (1 << 6),                 // 1 text string allowed
    _T2 = (1 << 7),                 // 2 text strings allowed
    _W  = (1 << 8),                 // W modifier allowed (for P)
};
    

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
    uint level;                     // Nesting level

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

    char c1;                        // 1st command character
    char c2;                        // 2nd command character (or NUL)
    char c3;                        // 3nd command character (or NUL)
    int m;                          // m argument
    int n;                          // n argument
    int paren;                      // No. of parentheses counted
    char delim;                     // Delimiter for @ modifier
    char qreg;                      // Q-register, if any
    char qlocal;                    // Q-register is local (not global)
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
    enum cmd_opts opts;
};
    
// Scanning functions

extern void scan_bad(struct cmd *cmd);

extern void scan_cmd(struct cmd *cmd);

extern void scan_flag(struct cmd *cmd);

extern void scan_null(struct cmd *cmd);

extern void print_cmd(struct cmd *cmd);

extern void scan_done(struct cmd *cmd);

extern void scan_expr(struct cmd *cmd);

extern void scan_mod(struct cmd *cmd);

extern void scan_null(struct cmd *cmd);

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

extern bool f_expression;

// Global functions

extern void check_errno(int ecode);

extern void echo_chr(int c);

extern void exec_cmd(void);

extern void get_flag(int *flag, struct cmd *cmd);

extern void init_env(void);

extern void init_qreg(void);

extern void init_term(void);

extern int getc_term(bool nowait);

extern void print_prompt(void);

extern void print_term(const char *str);

extern void put_bell(void);

extern void putc_term(int c);

extern void puts_term(const char *str, unsigned int nbytes);

extern bool help_command(void);

extern void print_badseq(void);

extern void read_cmd(void);

extern int read_term(void);

extern int teco_env(int n);

extern void write_term(const char *buf, unsigned int nbytes);

extern struct qreg *get_qreg(int qname, bool qlocal);

extern int get_delim(int delim);

// Functions that access the expression stack

extern bool empty_expr(void);

extern int get_n_arg(void);

extern void init_expr(void);

extern const char *next_cmd(void);

extern bool operand_expr(void);

extern void push_expr(int c, enum expr_type type);

extern void type_cmd(const char *p, int len);

extern bool valid_radix(int c);

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

#endif  // !defined(_TECO_H)
