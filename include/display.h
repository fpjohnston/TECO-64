///
///  @file    display.h
///  @brief   Header file for display mode functions.
///
///  @copyright 2019-2022 Franklin P. Johnston / Nowwith Treble Software
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

#if     !defined(_DISPLAY_H)

#define _DISPLAY_H

#include <limits.h>             //lint !e451
#include <stdbool.h>            //lint !e451

#include "teco.h"


///  @var     pair
///  @brief   Values of foreground/background pairs for defined windows.

enum pair
{
    CMD = 1,
    EDIT,
    STATUS,
    LINE,
    MAX_PAIR = LINE
};

///  @struct  tchar
///  @brief   Terminal characteristics flag.

union tchar
{
    struct
    {
        uint ansi_crt   : 1;        ///< Terminal is an ANSI CRT
        uint edit_mode  : 1;        ///< Terminal has EDIT mode features
        uint rev_scroll : 1;        ///< Terminal can do reverse scrolling
        uint spec_graph : 1;        ///< Terminal has special graphics
        uint rev_video  : 1;        ///< Terminal can do reverse video
        uint term_width : 1;        ///< Terminal can change its width
        uint scroll_reg : 1;        ///< Terminal has scrolling regions
        uint end_of_scr : 1;        ///< Terminal can erase to end of screen
    };

    uint flag;                      ///< Combined above flags
};

///  @struct  watch
///  @brief   Display mode variables

struct watch
{
    int type;                       ///< 0:W - Type of scope
    int width;                      ///< 1:W - Terminal width in columns
    int height;                     ///< 2:W - Terminal height in rows
    bool seeall;                    ///< 3:W - SEEALL mode
    int_t mark;                     ///< 4:W - Mark status
    int_t topdot;                   ///< 6:W - Buffer position of upper left corner
    int nlines;                     ///< 7:W - No. of command window lines
    bool noscroll;                  ///< 8:W - Disable scrolling region
    union tchar tchar;              ///< 9:W - Terminal characteristics
    int maxline;                    ///< 11:W - Length of longest line in edit buffer
    int_t botdot;                   ///< Buffer position of bottom right corner
};


extern const bool esc_seq_def;

extern struct watch w;


extern int check_key(int c);

extern bool clear_eol(void);

extern void exit_dpy(void);

extern int exec_key(int c);

extern bool exec_soft(int key);

extern int get_nowait(void);

extern int get_tab(void);

extern int get_wait(void);

extern void init_dpy(void);

extern void init_keys(void);

extern bool putc_cmd(int c);

extern void refresh_dpy(void);

extern void reset_colors(void);

extern void reset_dpy(bool all);

extern void rubout_key(int c);

extern void set_bits(bool parity);

extern void set_escape(bool escape);

extern void set_tab(int n);


// Define data used only internally within display code

#if     defined(DISPLAY_INTERNAL)

#define STATUS_WIDTH        22      ///< Width of status window

#define STATUS_HEIGHT        5      ///< Min. height for status window

///  @struct  display
///  @brief   Display mode variables

struct display
{
    WINDOW *edit;                   ///< Edit window
    WINDOW *status;                 ///< Status window
    WINDOW *cmd;                    ///< Command window
    WINDOW *fence;                  ///< Fence between edit and command windows
    int row, col;                   ///< Current cursor coordinates
    int newrow, newcol;             ///< Pending cursor coordinates
    int cursor;                     ///< Cursor character
    bool updown;                    ///< true if last cmd was up/down arrow
    int line;                       ///< Previous line no. for dot
    int oldcol;                     ///< Previous column for dot
    int minrow;                     ///< Min. row for edit window
    int mincol;                     ///< Min. column for edit window
    int maxrow;                     ///< Max. row for edit window
    int maxcol;                     ///< Max. column for edit window
    int ybias;                      ///< Vertical bias for edit window
    int xbias;                      ///< Horizontal bias for edit window
    int nrows;                      ///< No. of rows in edit window
    int ncols;                      ///< No. of columns in edit window
};


extern struct display d;

extern char keysize[UCHAR_MAX + 1];


extern int_t find_column(void);

extern void init_colors(void);

extern void refresh_status(void);


#endif

#endif  // !defined(_DISPLAY_H)
