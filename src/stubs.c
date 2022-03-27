///
///  @file    stubs.c
///  @brief   Stubs for display functions and variables.
///
///  @copyright 2022 Franklin P. Johnston / Nowwith Treble Software
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

#include "teco.h"
#include "display.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"


const bool esc_seq_def = false;         ///< Escape sequences disabled by default


///
///  @brief    (STUB) Check for special display input characters.
///
///  @returns  Next input character to process.
///
////////////////////////////////////////////////////////////////////////////////

int check_dpy_chr(int c, bool unused)
{
    return c;                           // Just return what we got if no display
}


///
///  @brief    (STUB) Check to see if escape sequences were enabled or disabled.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void check_escape(bool unused)
{
    f.ed.escape = false;                // Always off if no display mode
}


///
///  @brief    (STUB) Clear screen and redraw display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void clear_dpy(void)
{
    ;
}


///
///  @brief    (STUB) Clear to end of line.
///
///  @returns  false.
///
////////////////////////////////////////////////////////////////////////////////

bool clear_eol(void)
{
    return false;
}


///
///  @brief    (STUB) Check for ending display mode.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void end_dpy(void)
{
    throw(E_DPY);                       // No display mode available
}


///
///  @brief    (STUB) Execute F1 command: set colors for command region.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F1(struct cmd *unused)
{
    ;
}


///
///  @brief    (STUB) Execute F2 command: set colors for edit region.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F2(struct cmd *unused)
{
    ;
}


///
///  @brief    (STUB) Execute F3 command: set colors for status line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F3(struct cmd *unused)
{
    ;
}


///
///  @brief    (STUB) Execute F4 command: set colors for divider line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F4(struct cmd *unused)
{
    ;
}


///
///  @brief    (STUB) Execute FM command: map key to command string, or unmap key.
///
///  @returns  Returns failure for :FM command, else exception is thrown.
///
////////////////////////////////////////////////////////////////////////////////

void exec_FM(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->colon)
    {
        push_x(FAILURE, X_OPERAND);     // Command failed
    }
    else
    {
        throw(E_DPY);                   // Display mode not active
    }
}


///
///  @brief    (STUB) Execute FQ command: map key to Q-register.
///
///  @returns  Returns failure for :FQ command, else exception is thrown.
///
////////////////////////////////////////////////////////////////////////////////

void exec_FQ(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->colon)
    {
        push_x(FAILURE, X_OPERAND);     // Command failed
    }
    else
    {
        throw(E_DPY);                   // Display mode not active
    }
}


///
///  @brief    (STUB) Reset display mode prior to exiting from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exit_dpy(void)
{
    ;
}


///
///  @brief    (STUB) Read next character without wait (non-blocking I/O).
///
///  @returns  Throws E_DPY error.
///
////////////////////////////////////////////////////////////////////////////////

int get_nowait(void)
{
    throw(E_DPY);                       // Read w/o wait requires ncurses
}


///
///  @brief    (STUB) Get tabsize.
///
///  @returns  Tab size.
///
////////////////////////////////////////////////////////////////////////////////

int get_tab(void)
{
    return 8;                           // Default tab size
}


///
///  @brief    (STUB) Read next character (if in display mode).
///
///  @returns  Throws E_DPY error.
///
////////////////////////////////////////////////////////////////////////////////

int get_wait(void)
{
    throw(E_DPY);                       // Read w/ wait requires ncurses
}


///
///  @brief    (STUB) Initialize display windows.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_windows(void)
{
    ;
}


///
///  @brief    (STUB) Mark edit buffer as having changed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void mark_ebuf(void)
{
    ;
}


///
///  @brief    (STUB) Output character to command window.
///
///  @returns  Always false (because display is never active).
///
////////////////////////////////////////////////////////////////////////////////

bool putc_cmd(int unused)
{
    return false;
}

///
///  @brief    (STUB) Read display key.
///
///  @returns  Same character received.
///
////////////////////////////////////////////////////////////////////////////////

int readkey_dpy(int c)
{
    return c;
}


///
///  @brief    (STUB) Refresh screen.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void refresh_dpy(void)
{
    ;
}


///
///  @brief    (STUB) Reset region colors to defaults.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_colors(void)
{
    ;
}


///
///  @brief    (STUB) Reset all mapped keys.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_map(void)
{
    ;
}


///
///  @brief    (STUB) Rubout character on display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void rubout_dpy(int unused)
{
    ;
}


///
///  @brief    (STUB) Set parity.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_parity(bool unused)
{
    ;
}


///
///  @brief    (STUB) Set tabsize.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_tab(int unused)
{
    ;
}


///
///  @brief    (STUB) Check for starting display mode.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void start_dpy(void)
{
    throw(E_DPY);                       // No display mode available
}
