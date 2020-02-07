///
///  @file    window.h
///  @brief   Header file for window support functions.
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

#if     !defined(_WINDOW_H)

#define _WINDOW_H

#if     !defined(_TECO_H)

#include "teco.h"

#endif

#if     !defined(_STDBOOL_H)

#include <stdbool.h>

#endif

///  @struct  watch
///  @brief   Watch scope variables (for controlling display).

struct watch
{
    int type;                       ///< Type of scope
    int width;                      ///< Terminal width in columns
    int height;                     ///< Terminal height in rows
    bool seeall;                    ///< SEEALL mode
    int mark;                       ///< "Mark" status
    int hold;                       ///< Hold mode indicator
    int topdot;                     ///< Buffer position of upper left corner
    int nscroll;                    ///< No. of scrolling lines
};

// Window functions

extern void clear_win(void);

extern int getchar_win(bool wait);

extern void getsize_win(void);

extern void init_win(void);

extern bool putc_win(int c);

extern bool puts_win(const char *buf);

extern bool readkey_win(int c);

extern void reset_win(void);

extern void refresh_win(void);

extern void set_nrows(void);

extern void set_scroll(int height, int nscroll);

#endif  // _EXEC_H
