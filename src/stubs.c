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
///  @brief    Check for input characters that require special processing.
///
///  @returns  Next input character to process.
///
////////////////////////////////////////////////////////////////////////////////

int check_key(int c)
{
    return c;                           // Just return what we got if no display
}


///
///  @brief    Clear to end of line.
///
///  @returns  false.
///
////////////////////////////////////////////////////////////////////////////////

bool clear_eol(void)
{
    return false;
}


///
///  @brief    Execute F1 command: set colors for command window.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F1(struct cmd *unused)
{
    ;                                   // Nothing to do if no display
}


///
///  @brief    Execute F2 command: set colors for edit window.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F2(struct cmd *unused)
{
    ;                                   // Nothing to do if no display
}


///
///  @brief    Execute F3 command: set colors for status line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F3(struct cmd *unused)
{
    ;                                   // Nothing to do if no display
}


///
///  @brief    Execute F4 command: set colors for divider line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F4(struct cmd *unused)
{
    ;                                   // Nothing to do if no display
}


///
///  @brief    Execute FM command: map key to command string, or unmap key.
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
}


///
///  @brief    Execute FQ command: map key to Q-register.
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
}


///
///  @brief    Execute ncurses keys such as F1, Home, or PgDn.
///
///  @returns  Same character received.
///
////////////////////////////////////////////////////////////////////////////////

int exec_key(int c)
{
    return c;
}


///
///  @brief    Reset display mode prior to exiting from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exit_dpy(void)
{
    ;                                   // Nothing to do if no display
}


///
///  @brief    Read next character without wait (non-blocking I/O).
///
///  @returns  Throws E_DPY error.
///
////////////////////////////////////////////////////////////////////////////////

int get_nowait(void)
{
    throw(E_DPY);                       // Read w/o wait requires ncurses
}


///
///  @brief    Get tabsize.
///
///  @returns  Tab size.
///
////////////////////////////////////////////////////////////////////////////////

int get_tab(void)
{
    return 8;                           // Default tab size
}


///
///  @brief    Read next character (if in display mode).
///
///  @returns  Throws E_DPY error.
///
////////////////////////////////////////////////////////////////////////////////

int get_wait(void)
{
    throw(E_DPY);                       // Read w/ wait requires ncurses
}


///
///  @brief    Initialize display mode.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_dpy(void)
{
    ;                                   // Nothing to do if no display
}


///  @brief    Initialize keysize[] array with the length of each character.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_keys(void)
{
    ;                                   // Nothing to do if no display
}


///
///  @brief    Output character to command window.
///
///  @returns  Always false (because display is never active).
///
////////////////////////////////////////////////////////////////////////////////

bool putc_cmd(int unused)
{
    return false;
}


///
///  @brief    Refresh screen.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void refresh_dpy(void)
{
    ;                                   // Nothing to do if no display
}


///
///  @brief    Reset window colors to defaults.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_colors(void)
{
    ;                                   // Nothing to do if no display
}


///
///  @brief    Un-highlight cursor, and say that we need a cursor update.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_cursor(void)
{
    ;                                   // Nothing to do if no display
}


///
///  @brief    Reset display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_dpy(bool unused)
{
    ;                                   // Nothing to do if no display
}


///
///  @brief    Reset all mapped keys.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_map(void)
{
    ;                                   // Nothing to do if no display
}


///
///  @brief    Rubout character on display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void rubout_key(int unused)
{
    ;                                   // Nothing to do if no display
}


///
///  @brief    Tell ncurses when 7-/8-bit setting changes.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_bits(bool unused)
{
    ;                                   // Nothing to do if no display
}


///
///  @brief    Tell ncurses when escape setting changes.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_escape(bool unused)
{
    f.ed.escape = false;                // Always off if no display mode
}


///
///  @brief    Set tabsize.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_tab(int unused)
{
    ;                                   // Nothing to do if no display
}
