///
///  @file    cmd.h
///  @brief   Header file for functions to process TECO commands.
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

#if     !defined(_CMD_H)

#define _CMD_H

#include "teco.h"               //lint !e451 !e537

/// @def    empty_cbuf()
/// @brief  Returns true if all data in command string has been read, else false.

#define empty_cbuf()    (cbuf->pos == cbuf->len)

/// @def    get_cbuf()
/// @brief  Returns the current command string.

#define get_cbuf()      cbuf

/// @def    peek_cbuf()
/// @brief  Peeks at next character in command string.

#define peek_cbuf()     cbuf->data[cbuf->pos]

/// @def    set_cbuf()
/// @brief  Sets the current command string.

#define set_cbuf(p)     (cbuf = (p))

///  @struct cmd
///  @brief  Command block structure.

struct cmd
{
    char c1;                        ///< 1st command character
    char c2;                        ///< 2nd command character (or NUL)
    char c3;                        ///< 3rd command character (or NUL)
    char qname;                     ///< Q-register name
    INT m_arg;                      ///< m argument
    INT n_arg;                      ///< n argument
    bool qlocal;                    ///< If true, Q-register is local
    bool m_set;                     ///< m argument is valid
    bool n_set;                     ///< n argument is valid
    bool h;                         ///< H found
    bool ctrl_y;                    ///< CTRL/Y found
    bool w;                         ///< W found
    bool colon;                     ///< : found
    bool dcolon;                    ///< :: found
    bool atsign;                    ///< @ found
    char delim;                     ///< Delimiter for @ modifier
    struct tstring text1;           ///< 1st text string
    struct tstring text2;           ///< 2nd text string
};

///  @typedef exec_func
///  @brief   Function to execute command.

typedef void (exec_func)(struct cmd *cmd);

///  @enum   cmd_opts
///  @brief  Options defined for each command.

enum cmd_opts
{
    OPT_M  = 1 << 0,        //lint !e835
    OPT_N  = 1 << 1,
    OPT_F  = 1 << 2,
    OPT_C  = 1 << 3,
    OPT_D  = 1 << 4,
    OPT_A  = 1 << 5,
    OPT_Q  = 1 << 6,
    OPT_T1 = 1 << 7,
    OPT_T2 = 1 << 8,
    OPT_S  = 1 << 9,
    OPT_B  = 1 << 10
};

///  @struct cmd_table
///
///  @brief  Format of command tables used to parse and execute commands.

struct cmd_table
{
    exec_func *exec;                ///< Execution function
    enum cmd_opts opts;             ///< Command options

#if     defined(TECO_TRACE)

    const char *name;               ///< Command function name

#endif

};

extern struct buffer *cbuf;

extern const struct cmd_table cmd_table[];

extern const uint cmd_max;

extern const struct cmd_table e_table[];

extern const uint e_max;

extern const struct cmd_table f_table[];

extern const uint f_max;

extern char *eg_result;

extern uint nparens;

extern const struct cmd null_cmd;

// Command buffer functions

extern int fetch_cbuf(void);

extern void init_cbuf(void);

extern void reset_cbuf(bool noerror);

extern void store_cbuf(int c);

// Miscellaneous functions

extern void check_args(struct cmd *cmd);

#endif  // !defined(_CMD_H)
