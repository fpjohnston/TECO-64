///
///  @file    exec.h
///  @brief   Header file for parsing and executing TECO commands.
///
/* (INSERT: WARNING NOTICE) */
///
///  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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

enum
{
    NO_EXIT,                        ///< End of list
    NO_ATSIGN,                      ///< Don't allow atsign
    NO_COLON,                       ///< Don't allow colon
    NO_DCOLON,                      ///< Don't allow double colon
    NO_M,                           ///< Don't allow m argument
    NO_M_ONLY,                      ///< Don't allow m without n
    NO_N,                           ///< Don't allow n argument
    NO_NEG_M,                       ///< Don't allow negative m
    NO_NEG_N                        ///< Don't allow negative n
};

#if     defined(NOSTRICT)       // Disable strict syntax checking

#define confirm(cmd, ...)

#else

/// @def    confirm
/// @brief  Tests restrictions on command syntax.

#define confirm(cmd, ...) if (f.e0.exec) confirm_cmd(cmd, ## __VA_ARGS__, NO_EXIT)

void confirm_cmd(struct cmd *cmd, ...);

#endif


// Global variables

extern char eg_command[];

extern tstring ez;

extern struct ctrl ctrl;

extern const struct cmd null_cmd;

// Functions that scan commands

/* (INSERT: SCAN FUNCTIONS) */

// Functions that execute commands

/* (INSERT: EXEC FUNCTIONS) */

// Helper functions for executing commands

extern bool append(bool n_set, int_t n_arg, bool colon_set);

extern bool check_semi(void);

extern void default_n(struct cmd *cmd, int_t n_default);

extern void close_files(void);

extern void exec_cmd(struct cmd *cmd);

extern void exec_ctrl_F(int key);

extern void exec_macro(tbuffer *macro, struct cmd *cmd);

extern void exec_insert(const char *buf, uint_t len);

extern void exec_str(const char *string);

extern void exit_loop(struct cmd *cmd);

extern int find_eg(char *buf);

extern bool finish_cmd(struct cmd *cmd, int c);

extern bool next_page(int_t start, int_t end, bool ff, bool yank);

extern bool next_yank(void);

extern bool read_EI(void);

extern void reset_if(void);

extern void reset_indirect(void);

extern void reset_loop(void);

extern void reset_search(void);

extern bool skip_cmd(struct cmd *cmd, const char *skip);

extern void scan_texts(struct cmd *cmd, int ntexts, int delim);

#endif  // !defined(_EXEC_H)
