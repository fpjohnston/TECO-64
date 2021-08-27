///
///  @file    exec.h
///  @brief   Header file for parsing and executing TECO commands.
///
/* (INSERT: WARNING NOTICE) */
///
///  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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

#include <assert.h>             //lint !e451 !e537

#include "teco.h"               //lint !e451 !e537
#include "eflags.h"             //lint !e451 !e537
#include "errcodes.h"           //lint !e451 !e537

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
    tstring text1;                  ///< 1st text string
    tstring text2;                  ///< 2nd text string
};

#if     defined(NOSTRICT)       // Disable strict syntax checking

#define reject_atsign(atsign)
#define reject_colon(colon)
#define reject_dcolon(dcolon)
#define reject_m(m_set)
#define reject_neg_m(m_set, m_arg)
#define reject_neg_n(n_set, n_arg)
#define reject_n(n_set)
#define require_n(m_set, n_set)

#else

// *** Note that the following functions are inline as an optimization. ***


///
///  @brief    Error if at sign and command doesn't allow it.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static inline void reject_atsign(bool atsign)
{
    if (f.e2.atsign && atsign)
    {
        throw(E_ATS);
    }
}


///
///  @brief    Error if colon and command doesn't allow it.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static inline void reject_colon(bool colon)
{
    if (f.e2.colon && colon)
    {
        throw(E_COL);
    }
}


///
///  @brief    Error if dcolon and command doesn't allow it.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static inline void reject_dcolon(bool dcolon)
{
    if (f.e2.colon && dcolon)
    {
        throw(E_COL);
    }
}


///
///  @brief    Error if m argument and command doesn't allow it.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static inline void reject_m(bool m_set)
{
    if (f.e2.m_arg && m_set)
    {
        throw(E_IMA);
    }
}


///
///  @brief    Error if m argument is negative.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static inline void reject_neg_m(bool m_set, int_t m_arg)
{
    if (m_set && m_arg < 0)
    {
        throw(E_NCA);
    }
}


///
///  @brief    Error if n argument is negative.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static inline void reject_neg_n(bool n_set, int_t n_arg)
{
    if (n_set && n_arg < 0)
    {
        throw(E_NCA);
    }
}


///
///  @brief    Error if n argument and command doesn't allow it.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static inline void reject_n(bool n_set)
{
    if (f.e2.n_arg && n_set)
    {
        throw(E_INA);
    }
}

///
///  @brief    Error if m argument not followed by n argument.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static inline void require_n(bool m_set, bool n_set)
{
    if (m_set && !n_set)
    {
        throw(E_NON);
    }
}

#endif

// Global variables

extern char eg_command[];

extern tstring ez;

extern uint nparens;

extern const struct cmd null_cmd;

// Functions that scan commands

/* (INSERT: SCAN FUNCTIONS) */

// Functions that execute commands

/* (INSERT: EXEC FUNCTIONS) */

// Helper functions for executing commands

extern bool append(bool n_set, int_t n_arg, bool colon_set);

extern bool append_line(void);

extern int check_EI(void);

extern bool check_semi(void);

extern void close_files(void);

extern void exec_cmd(struct cmd *cmd);

extern bool exec_ctrl_F(int key);

extern void exec_macro(tbuffer *macro, struct cmd *cmd);

extern void exec_insert(const char *buf, uint_t len);

extern int find_eg(char *buf);

extern bool next_page(int_t start, int_t end, bool ff, bool yank);

extern bool next_yank(void);

extern bool read_EI(void);

extern void reset_if(void);

extern void reset_indirect(void);

extern void reset_loop(void);

extern bool skip_cmd(struct cmd *cmd, const char *skip);

extern void scan_texts(struct cmd *cmd, int ntexts, int delim);

#endif  // !defined(_EXEC_H)
