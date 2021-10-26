///
///  @file    w_cmd.c
///  @brief   Execute W command.
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "display.h"
#include "eflags.h"
#include "estack.h"
#include "exec.h"
#include "term.h"


#define DEFAULT_HEIGHT          24      ///< Default terminal rows
#define DEFAULT_WIDTH           80      ///< Default terminal columns

#define MIN_HEIGHT              10      ///< Minimum no. of rows
#define MIN_WIDTH               10      ///< Minimum no. of columns

///  @var    w
///  @brief  Variables to read and control scope display.

struct watch w =
{
    .type     = 8,                      // VT102 in ANSI mode
    .width    = DEFAULT_WIDTH,
    .height   = DEFAULT_HEIGHT,
    .topdot   = 0,
    .nlines   = 0,
    .noscroll = false,
    .tchar    =
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

static int_t get_w(int_t n);

static void set_w(int_t m, int_t n);


///
///  @brief    Get watch scope variable.
///
///  @returns  Value of watch scope variable.
///
////////////////////////////////////////////////////////////////////////////////

static int_t get_w(int_t n)
{
    switch (n)
    {
        default:
        case 0:
            return w.type;

        case 1:
            return w.width;

        case 2:
            n = (int_t)w.height - (int_t)w.nlines;

            if (f.e0.display && f.e4.line)
            {
                --n;
            }

            return n;

        case 3:
        case 4:
        case 5:
            return 0;

        case 6:
            return w.topdot;

        case 7:
            return w.nlines;

        case 8:
            return w.noscroll ? -1 : 0;

        case 9:
            return (int_t)w.tchar.flag;
    }
}


///
///  @brief    Execute W command: process display functions.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_W(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->colon)                     // n:W already processed
    {
        return;
    }

#if     defined(DISPLAY_MODE)

    if (!f.et.scope)                    // Scope mode allowed?
    {
        return;                         // No
    }

    if (!cmd->n_set)                    // Was it W with no argument?
    {
        if (f.e0.display)
        {
            reset_dpy();
            init_term();
        }
    }
    else if (cmd->n_arg == -1)          // Was it -1W?
    {
        if (!f.e0.display)
        {
            reset_term();               // Reset if display mode support
            init_dpy();
            color_dpy();
            clear_dpy();
        }
    }

#endif

}


///
///  @brief    Scan W command: process display functions.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_W(struct cmd *cmd)
{
    assert(cmd != NULL);

    require_n(cmd->m_set, cmd->n_set);
    reject_dcolon(cmd->dcolon);
    reject_atsign(cmd->atsign);

    if (!cmd->colon)
    {
        reject_m(cmd->m_set);           // m,nW is invalid

        return false;
    }

    if (!cmd->n_set)
    {
        cmd->n_set = true;
        cmd->n_arg = 0;                 // :W = 0:W
    }

    if (cmd->m_set)                     // m,n:W
    {
        set_w(cmd->m_arg, cmd->n_arg);
    }

    int_t n = get_w(cmd->n_arg);

    push_x(n, X_OPERAND);

    cmd->colon = cmd->dcolon = false;   // Reset for next command
    cmd->m_set = false;
    cmd->m_arg = 0;

    return true;
}


///
///  @brief    Set watch scope variable.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void set_w(int_t m, int_t n)
{
    switch (n)
    {
        case 0:
            w.type = (int)m;

            break;

        case 1:
            if (m >= MIN_WIDTH)
            {
                w.width = (int)m;
            }

            break;

        case 2:
            if (m >= MIN_HEIGHT)
            {
                w.height = (int)m;

                set_nrows();
            }

            break;

        case 7:
            if (m > 1 && w.height - m >= 9)
            {
                w.nlines = (int)m;

                set_scroll(w.height, w.nlines);
            }

            break;

        case 8:
            w.noscroll = m ? true : false;

            break;

        default:
            break;
    }
}
