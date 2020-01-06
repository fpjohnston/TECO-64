///
///  @file    w_cmd.c
///  @brief   Execute W command.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
///
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files (the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


#define DEFAULT_HEIGHT          24      ///< Default terminal rows
#define DEFAULT_WIDTH           80      ///< Default terminal columns

///  @var    w
///  @brief  Variables to read and control scope display.

struct watch w =
{
    .type      = 4,                     // VT100 in ANSI mode
    .width     = DEFAULT_WIDTH,
    .height    = DEFAULT_HEIGHT,
    .seeall    = false,
    .mark      = 0,
    .hold      = 0,
    .topdot    = 0,
    .nscroll   = 0,
    .spacemark = 0,
    .keypad    = 0,
};


// Local functions

static int get_w(int n);

static void set_w(int m, int n);


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
        case 0: return (int)w.type;

        case 1: return (int)w.width;

        case 2: return (int)w.height;

        case 3: return w.seeall ? -1 : 0;

        case 4: return w.mark;

        case 5: return w.hold;

        case 6: return (int)w.topdot;

        case 7: return (int)w.nscroll;

        case 8: return (int)w.spacemark;

        case 9: return (int)w.keypad;
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

    if (cmd->colon_set)                 // n:W returns a value
    {
        push_expr(cmd->n_set ? cmd->n_arg : 0, EXPR_VALUE);

        return;
    }

    // Here for W command without a colon.

    if (!f.et.scope)
    {
        return;
    }

    if (cmd->colon_set)
    {
        int n;

        if (cmd->n_set)
        {
            if (cmd->m_set)
            {
                set_w(cmd->m_arg, cmd->n_arg);
            }

            n = cmd->n_arg;
        }
        else
        {
            n = 0;                      // :W = 0:W
        }

        n = get_w(n);

        push_expr(n, EXPR_VALUE);
    }
    else if (cmd->n_set)
    {
        if (f.et.rscope)
        {
            if (cmd->n_arg == 0)
            {
                // TODO: turn off the refresh scope display
            }
            else if (cmd->n_arg > 0)
            {
                // TODO: set number of display lines
            }
            else
            {
                // TODO: error?
            }
        }
        else
        {
            if (cmd->n_arg == -1000)
            {
                // TODO: forget that output was done
            }
            else if (cmd->n_arg == -1)
            {
                // TODO: refresh screen
            }
            else if (cmd->n_arg < 0)
            {
                // TODO: top n-1 lines have been altered
            }
            else if (cmd->n_arg == 0)
            {
                // TODO: equivalent to 16W
            }
            else
            {
                // TODO: place cursor line at line n
            }
        }
    }
    else if (f.et.rscope)
    {
        // TODO: update the refresh scope screen
    }
    else
    {
        // TODO: forget screen image and special scope modes
    }
}


///
///  @brief    Set scope type.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void set_type(int m)
{
    switch (m)
    {
        case 0:                         // VT52
        case 1:                         // VT61
        case 2:                         // VT100 in VT52 mode
        case 4:                         // VT100 in VT100 mode
        case 6:                         // VT05
        case 8:                         // VT102
        case 10:                        // VK100
        case 11:                        // VT200 in VT200 mode
        case 12:                        // VT200 in ANSI (VT100) mode
        case 13:                        // VT200 in VT52 mode
            w.type = (uint)m;

            break;

        default:
            print_err(E_ARG);           // Invalid argument

            break;
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
            set_type(m);
            break;

        case 1: w.width = (uint)m;                   break;

        case 2: w.height = (uint)m;                  break;

        case 3: w.seeall = (n == -1) ? true : false; break;

        case 4: w.mark = m;                          break;

        case 5: w.hold = m;                          break;

        case 6: w.topdot = (uint)m;                  break;

        case 7: w.nscroll = (uint)m;                 break;

        case 8: w.spacemark = (uint)m;               break;

        case 9: w.keypad = (uint)m;                  break;

        default: print_err(E_ARG);
    }
}
