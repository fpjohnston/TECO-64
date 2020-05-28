///
///  @file    cmd.h
///  @brief   Header file for functions to process TECO commands.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
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

#if     !defined(_CMD_H)

#define _CMD_H

#if     !defined(_TECO_H)

#include "teco.h"

#endif


#define CMD_START       (bool)true  ///< Read character at start of command

#define NOCMD_START     (bool)false ///< Read character in middle of command

///  @struct  scan
///  @brief   Internal scanning state

struct scan
{
    uint nparens;                   ///< No. of unmatched left parentheses
    uint nbraces;                   ///< No. of unmatched left braces
    int sum;                        ///< Accumulated sum of digits scanned
    bool digits;                    ///< Accumulated sum is valid
    bool expr;                      ///< Current command is part of expression
    bool mod;                       ///< Command modifier seen
    bool space;                     ///< Last chr. scanned was whitespace
    bool flag;                      ///< Command is a mode control flag
    bool brace_opt;                 ///< Command has alternate meaning in braces
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

///  @typedef exec_func
///  @brief   Function to execute command.

typedef void (exec_func)(struct cmd *cmd);

///  @struct cmd_table
///  @brief  Format of command tables used to parse and execute commands.

struct cmd_table
{
    exec_func *exec;                ///< Execution function
    const char *opts;               ///< Command modifiers and options
};

extern const struct cmd_table cmd_table[];

extern const struct cmd_table cmd_e_table[];

extern const struct cmd_table cmd_f_table[];

extern const uint cmd_count;

extern const uint cmd_e_count;

extern const uint cmd_f_count;

// Miscellaneous functions

extern exec_func *next_cmd(struct cmd *cmd);

extern void print_cmd(struct cmd *cmd);

extern void reset_scan(void);

extern exec_func *scan_cmd(struct cmd *cmd);

#endif  // _CMD_H
