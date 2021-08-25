///
///  @file    display.h
///  @brief   Header file for display mode functions.
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

#if     !defined(_DISPLAY_H)

#define _DISPLAY_H

#include <stdbool.h>            //lint !e451 !e537

#include "teco.h"               //lint !e451 !e537


#define SATMAX      1000            ///< Maximum color saturation

///  @enum   region_pair
///  @brief  Values of foreground/background pairs for defined regions.

enum region_pair
{
    CMD = 1,                        ///< Command region
    EDIT,                           ///< Edit region
    STATUS                          ///< Status line
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
    int type;                       ///< Type of scope
    int width;                      ///< Terminal width in columns
    int height;                     ///< Terminal height in rows
    bool seeall;                    ///< SEEALL mode
    int mark;                       ///< "Mark" status
    int hold;                       ///< Hold mode indicator
    int_t topdot;                   ///< Buffer position of upper left corner
    int nlines;                     ///< No. of scrolling lines
    bool noscroll;                  ///< Disable scrolling region
    union tchar tchar;              ///< Terminal characteristics
};

extern struct watch w;

// Display functions

#if     defined(DISPLAY_MODE)

extern void check_escape(uint escape);

extern bool clear_eol(void);

#endif

extern void clear_dpy(void);

#if     defined(DISPLAY_MODE)

extern bool exec_key(int key);

extern void init_dpy(void);

extern bool putc_dpy(int c);

#endif

extern int readkey_dpy(int c);

extern void refresh_dpy(void);

extern void reset_colors(void);

#if     defined(DISPLAY_MODE)

extern void reset_dpy(void);

extern void resize_key(void);

extern void resize_signal(void);

#endif

extern void set_nrows(void);

extern void set_scroll(int height, int nscroll);

#endif  // !defined(_DISPLAY_H)
