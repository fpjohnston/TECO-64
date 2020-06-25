///
///  @file    qreg.h
///  @brief   Definitions and interfaces for Q-register functions.
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

#if     !defined(_QREG_H)

#define _QREG_H

#if     !defined(_STDBOOL_H)
#include <stdbool.h>
#endif

#if     !defined(_SYS_TYPES_H)
#include <sys/types.h>
#endif

#if     !defined(_TECO_H)
#include <teco.h>
#endif


///  @struct  qreg
///  @brief   Definition of Q-register storage, which includes a string and a
///           numeric value.

struct qreg
{
    int n;                          ///< Q-register numeric value
    struct buffer text;             ///< Q-register text storage
};

///  @var     QCHARS
///  @brief   List of (global) Q-registers.

#define QCHARS          "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"

///  @def     QREG_SIZE
///  @brief   No. of Q-registers in each set.

#define QREG_SIZE       (uint)(sizeof(QCHARS) - 1)

// Q-register functions

extern void append_qchr(int qname, bool qlocal, int c);

extern void delete_qtext(int qname, bool qlocal);

extern uint get_qall(void);

extern int get_qchr(int qname, bool qlocal, int n);

extern int get_qnum(int qname, bool qlocal);

extern struct qreg *get_qreg(int qname, bool qlocal);

extern uint get_qsize(int qname, bool qlocal);

extern void init_qreg(void);

extern void pop_qlocal(void);

extern bool pop_qreg(int qname, bool qlocal);

extern void print_qreg(int qname, bool qlocal);

extern void push_qlocal(void);

extern bool push_qreg(int qname, bool qlocal);

extern void reset_macro(void);

extern void reset_qreg(void);

extern void store_qchr(int qname, bool qlocal, int c);

extern void store_qnum(int qname, bool qlocal, int n);

extern void store_qtext(int qname, bool qlocal, struct buffer *text);

#endif  // !defined(_QREG_H)
