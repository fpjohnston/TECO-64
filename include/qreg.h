///
///  @file    qreg.h
///  @brief   Definitions and interfaces for Q-register functions.
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

#if     !defined(_QREG_H)

#define _QREG_H

#include <stdbool.h>            //lint !e451 !e537
#include <sys/types.h>          //lint !e451 !e537

#include "teco.h"               //lint !e451 !e537
#include "exec.h"               //lint !e451 !e537


///  @struct  qreg
///  @brief   Definition of Q-register storage, which includes a string and a
///           numeric value.

struct qreg
{
    int_t n;                        ///< Q-register numeric value
    tbuffer text;             ///< Q-register text storage
};

///  @var     QNAMES
///  @brief   List of Q-register names.

#define QNAMES      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"

///  @def     QCOUNT
///  @brief   No. of Q-registers in each set (global and local).

#define QCOUNT      (int)(sizeof(QNAMES) - 1)

// Q-register functions

extern void append_qchr(int qindex, int c);

extern void delete_qtext(int qindex);

extern uint_t get_qall(void);

extern int get_qchr(int qindex, int_t n);

extern int get_qindex(int qname, bool qlocal);

extern int_t get_qnum(int qindex);

extern struct qreg *get_qreg(int qindex);

extern uint_t get_qsize(int qindex);

extern void init_qreg(void);

extern void pop_qlocal(void);

extern bool pop_qreg(int qindex);

extern void print_qreg(int qindex);

extern void push_qlocal(void);

extern bool push_qreg(int qindex);

extern void reset_macro(void);

extern void reset_qreg(void);

extern void scan_qreg(struct cmd *cmd);

extern void store_qchr(int qindex, int c);

extern void store_qnum(int qindex, int_t n);

extern void store_qtext(int qindex, tbuffer *text);

#endif  // !defined(_QREG_H)
