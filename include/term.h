///
///  @file    term.h
///  @brief   Header file for TECO terminal functions.
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

#if     !defined(_TERM_H)

#define _TERM_H

#include <stdbool.h>            //lint !e451 !e537
#include <sys/types.h>          //lint !e451 !e537


// Terminal buffer functions

extern int delete_tbuf(void);

extern void echo_tbuf(int pos);

extern int fetch_tbuf(void);

extern void init_tbuf(void);

extern void reset_tbuf(void);

extern uint start_tbuf(void);

extern void store_tbuf(int c);

// Terminal input/output functions

extern void echo_in(int c);

extern void echo_out(int c);

extern int getc_term(bool nowait);

extern void init_term(void);

extern void print_chr(int c);

extern void print_echo(int c);

extern void print_prompt(void);

extern void print_str(const char *fmt, ...);

extern void put_bell(void);

extern void read_cmd(void);

extern void reset_term(void);

#endif  // !defined(_TERM_H)
