///
///  @file    w_cmd.c
///  @brief   Execute W command.
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#if     defined(SCOPE)

#include <ncurses.h>

#endif

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "term.h"
#include "window.h"


#define DEFAULT_HEIGHT          24      ///< Default terminal rows
#define DEFAULT_WIDTH           80      ///< Default terminal columns

///  @var    w
///  @brief  Variables to read and control scope display.

struct watch w =
{
    .type      = 8,                     // VT102 in ANSI mode
    .width     = DEFAULT_WIDTH,
    .height    = DEFAULT_HEIGHT,
    .seeall    = false,
    .mark      = 0,
    .hold      = 0,
    .topdot    = 0,
    .nlines    = 0,
    .noscroll  = false,
    .tchar     =
    {
        {                               //lint !e708
            .ansi_crt   = true,
            .edit_mode  = true,
            .rev_scroll = true,
            .spec_graph = true,
            .rev_video  = true,
            .term_width = true,
            .scroll_reg = true,
            .end_of_scr = true,
        }
    },
};


// Local functions

static int get_w(int n);

static void set_w(int m, int n);


///
///  @brief    Execute E6 command: set window colors.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E6(struct cmd *cmd)
{

#if     defined(SCOPE)

    assert(cmd != NULL);

    char keyword[cmd->text1.len + 1];

    sprintf(keyword, "%.*s", (int)cmd->text1.len, cmd->text1.buf);

    if (cmd->n_set)
    {
        // Set the saturation for a specified color. ncurses allows these
        // levels to range from 0 to 1000. Colors are defined with separate
        // levels for red, green, and blue. Note that setting a level only
        // makes sense for colors other than black, since black is defined
        // as having red, green, and blue all 0.

        int color = find_color(keyword);

        if (color == -1)
        {
            print_err(E_WIN);
        }

        int n = cmd->n_arg;              // Color saturation

        // Make sure it's in the range [0, SATMAX].

        if (n < 0)
        {
            n = 0;
        }
        else if (n > SATMAX)
        {
            n = SATMAX;
        }

        // Adjust color saturation

        short red   = (short)(color_table[color].red   * (uint)n / SATMAX);
        short green = (short)(color_table[color].green * (uint)n / SATMAX);
        short blue  = (short)(color_table[color].blue  * (uint)n / SATMAX);

        (void)init_color((short)color, red, green, blue);
    }
    else
    {
        char value[cmd->text2.len + 1];

        sprintf(value, "%.*s", (int)cmd->text2.len, cmd->text2.buf);

        if (*keyword != NUL)                // Anything to parse?
        {
            set_colors(keyword, value);
        }
    }

#else

    print_err(E_WIN);

#endif

}


///
///  @brief    Get watch scope variable.
///
///  @returns  Value of watch scope variable.
///
////////////////////////////////////////////////////////////////////////////////

static int get_w(int n)
{
    switch (n)
    {
        default:
        case 0:
            return w.type;

        case 1:
            return w.width;

        case 2:
            return w.height - w.nlines;

        case 3:
            return w.seeall ? -1 : 0;

        case 4:
            return w.mark;

        case 5:
            return w.hold;

        case 6:
            return w.topdot;

        case 7:
            return w.nlines;

        case 8:
            return w.noscroll ? -1 : 0;

        case 9:
            return (int)w.tchar.flag;
    }
}


///
///  @brief    Scan W command: process window functions.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_W(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->colon_set)
    {
        int n;

        if (!pop_expr(&cmd->n_arg))
        {
            cmd->n_arg = 0;             // :W = 0:W
        }
        else if (cmd->m_set)            // If m,n:W, then do set before read
        {
            set_w(cmd->m_arg, cmd->n_arg);
        }

        n = get_w(cmd->n_arg);

        push_expr(n, EXPR_VALUE);

        return;
    }

    if (!f.et.scope)                    // Scope mode allowed?
    {
        return;                         // No
    }

    if (cmd->n_set)
    {
        if (!f.e0.winact)
        {

#if     defined(SCOPE)

            reset_term();               // Don't reset if no window support

#endif

            init_win();
            clear_win();
        }
    }
    else
    {
        if (f.e0.winact)
        {
            reset_win();
            init_term();
        }
    }
}


///
///  @brief    Set watch scope variable.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void set_w(int m, int n)
{
    switch (n)
    {
        case 0:
            w.type = m;

            break;

        case 1:
            w.width = m;

            break;

        case 2:
            w.height = m;
            set_nrows();

            break;

        case 3:
            w.seeall = (n == -1) ? true : false;

            break;

        case 4:
            w.mark = m;

            break;

        case 5:
            w.hold = m;

            break;

        case 6:
            w.topdot = m;

            break;

        case 7:
            if (m <= 1 || w.height - m < 9)
            {
                print_err(E_WIN);       // Window error
            }

            w.nlines = m;

            set_scroll(w.height, w.nlines);

            break;

        case 8:
            w.noscroll = m ? true : false;

            break;

        case 9:                         // Flag not settable
            break;

        default:
            print_err(E_ARG);
    }
}
