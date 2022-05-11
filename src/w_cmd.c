///
///  @file    w_cmd.c
///  @brief   Execute W, F0, FH, and FZ commands.
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
#define DEFAULT_MAXLINE        500      ///< Default maximum line length

#define MIN_HEIGHT              10      ///< Minimum no. of rows
#define MIN_WIDTH               10      ///< Minimum no. of columns

///  @var    w
///  @brief  Variables to read and control scope display.

struct watch w =
{
    .type     = 8,                      // 0:W - VT102 in ANSI mode
    .width    = DEFAULT_WIDTH,          // 1:W
    .height   = DEFAULT_HEIGHT,         // 2:W
    .seeall   = false,                  // 3:W
    .mark     = 0,                      // 4:W
    .topdot   = 0,                      // 6:W and F0
    .nlines   = 0,                      // 7:W
    .noscroll = false,                  // 8:W
    .tchar    =                         // 9:W
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
    .maxline  = DEFAULT_MAXLINE,        // 11:W
    .status   = 0,                      // 12:W
    .botdot   = 0,                      // FZ
};


// Local functions

static int_t get_w(int_t n);

static void set_w(int_t m, int_t n);


///
///  @brief    Execute W command: process display functions.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_W(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!cmd->colon)                    // Is it nW?
    {
        if (f.et.scope)                 // Scope mode allowed?
        {
            if (!cmd->n_set || cmd->n_arg == 0)
            {
                exit_dpy();             // W or 0W stops display mode
            }
            else if (cmd->n_arg == -1)
            {
                init_dpy();             // -1W starts display mode
            }
            else
            {
                throw(E_DPY);           // Anything else is an error
            }
        }
        else
        {
            throw(E_DPY);               // Display mode requires scope terminal
        }
    }
}


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
        case 0:                         // Terminal type
            return w.type;

        case 1:                         // Horizontal size of editing window
            return w.width;

        case 2:                         // Vertical size of editing window
            n = (int_t)w.height - (int_t)w.nlines;

            if (f.e0.display && f.e4.fence)
            {
                --n;
            }

            return n;

        case 3:                         // SEEALL mode
            return w.seeall ? -1 : 0;

        case 4:                         // Mark status
            return w.mark;

        case 5:                         // Hold mode indicator
            return 0;

        case 6:                         // Buffer position of top left corner
            return w.topdot;

        case 7:                         // No. of lines of command window
            return w.nlines;

        case 8:                         // Edit window enable/disable status
            return w.noscroll ? -1 : 0;

        case 9:                         // Terminal characteristics
            return (int_t)w.tchar.flag;

        case 10:                        // Horizontal tab size
            return get_tab();

        case 11:                        // Maximum length of line in edit buffer
            return w.maxline;

        case 12:                        // Width of status window (or 0 if none)
            return w.status;

        default:
            throw(E_ARG);               // n:W is out of range
    }
}


///
///  @brief    Scan F0 command: return position for top left corner.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_F0(struct cmd *cmd)
{
    scan_x(cmd);
    push_x((int_t)w.topdot, X_OPERAND);

    return true;
}


///
///  @brief    Scan FH command: equivalent to F0,FZ.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_FH(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);

    cmd->m_set = true;
    cmd->m_arg = w.topdot;

    push_x(w.botdot, X_OPERAND);

    return true;
}


///
///  @brief    Scan FZ command: return position for bottom right corner.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_FZ(struct cmd *cmd)
{
    scan_x(cmd);

    push_x(w.botdot, X_OPERAND);

    return true;
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

    if (cmd->m_set && f.e0.exec)        // m,n:W
    {
        set_w(cmd->m_arg, cmd->n_arg);
    }

    cmd->m_set = false;
    cmd->m_arg = 0;

    if (cmd->colon)
    {
        cmd->colon = cmd->dcolon = false; // Reset for next command

        int_t n = get_w(cmd->n_arg);

        push_x(n, X_OPERAND);

        return true;
    }
    else
    {
        return false;
    }
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
            if (!f.e0.display && m >= MIN_WIDTH)
            {
                w.width = (int)m;
            }

            break;

        case 2:
            if (!f.e0.display && m >= MIN_HEIGHT)
            {
                w.height = (int)m;
            }

            break;

        case 3:
            if (w.seeall != (bool)m)
            {
                w.seeall = m ? true : false;

                init_keys();            // Adjust widths of display chrs.
            }

            break;

        case 4:
            w.mark = m;

            break;

        case 5:
        case 6:
            break;

        case 7:
            if (m > 1 && w.height - m >= 9)
            {
                if (w.nlines != (int)m)
                {
                    w.nlines = (int)m;

                    reset_dpy((bool)true);
                }
            }

            break;

        case 8:
            if (w.noscroll != (bool)m)
            {
                w.noscroll = m ? true : false;

                reset_dpy((bool)true);
            }

            break;

        case 9:
            break;

        case 10:
            if (get_tab() != m)
            {
                set_tab(m);

                reset_dpy((bool)false);
            }

            break;

        case 11:
            if (w.maxline != m)
            {
                w.maxline = m;

                reset_dpy((bool)true);
            }

            break;

        case 12:
            if (w.status != m)
            {
                w.status = m;

                reset_dpy((bool)true);
            }

            break;

        default:
            throw(E_ARG);               // m,n:W is out of range
    }
}
