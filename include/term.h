///
///  @file    term.h
///  @brief   Header file for TECO terminal functions.
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

#if     !defined(_TERM_H)

#define _TERM_H

#include <stdbool.h>            //lint !e451


#define FF_LINES    8           ///< No. of horizontal lines for form feed

#define VT_LINES    4           ///< No. of horizontal lines for vertical tab

#define TAB_WIDTH   8           ///< Horizontal tab stop width

#define WAIT    (bool)true      ///< Wait for terminal input

#define NO_WAIT (bool)false     ///< Don't wait for terminal input

#if     defined(DEBUG)

extern const char *key_name;

extern void putc_key(int c);

#endif

extern const char *table_8bit[];

extern int term_pos;

// Terminal buffer functions

extern tbuffer copy_tbuf(void);

extern int delete_tbuf(void);

extern void echo_tbuf(uint_t pos);

extern int fetch_tbuf(void);

extern int getlen_tbuf(void);

extern void init_tbuf(void);

extern void reset_tbuf(void);

extern void rubout_chr(int c);

extern void rubout_line(void);

extern uint_t start_tbuf(void);

extern void store_tbuf(int c);

// Terminal input/output functions

extern bool check_help(void);

extern void echo_in(int c);

extern int getc_term(bool nowait);

extern void init_term(void);

extern void print_alert(const char *msg);

extern void print_prompt(void);

extern void read_cmd(void);

extern void reset_term(void);

extern void retype_line(uint_t pos);

extern void type_out(int c);

#endif  // !defined(_TERM_H)
