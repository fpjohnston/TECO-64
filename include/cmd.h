///
///  @file    cmd.h
///  @brief   Header file for functions to process TECO commands.
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
    bool qlocal;                    ///< If true, Q-register is local
    bool m_set;                     ///< m argument is valid
    int m_arg;                      ///< m argument
    bool n_set;                     ///< n argument is valid
    int n_arg;                      ///< n argument
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

///  @struct cmd_opts
///
///  @brief  Command options and requirements.

union cmd_opts
{
    struct
    {
        uint m  : 1;                ///< m argument allowed
        uint n  : 1;                ///< n argument allowed
        uint f  : 1;                ///< Flag argument (returns value if no n)
        uint c  : 1;                ///< : modifier allowed
        uint d  : 1;                ///< :: modifier allowed
        uint a  : 1;                ///< @ modifier allowed
        uint q  : 1;                ///< Q-register required
        uint w  : 1;                ///< W modifier allowed
        uint t1 : 1;                ///< 1 text argument allowed
        uint t2 : 1;                ///< 2 text arguments allowed
        uint z  : 1;                ///< No arguments allowed
    };

    int bits;                       ///< All of the bits above
};

///  @struct cmd_table
///
///  @brief  Format of command tables used to parse and execute commands.

struct cmd_table
{
    exec_func *exec;                ///< Execution function
    const union cmd_opts *opts;     ///< Command options

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

// Command buffer functions

extern int fetch_cbuf(void);

extern void init_cbuf(void);

extern void reset_cbuf(void);

extern void store_cbuf(int c);

// Miscellaneous functions

extern void check_args(struct cmd *cmd);

#endif  // !defined(_CMD_H)
