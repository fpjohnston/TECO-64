///
///  @file    term_sys.c
///  @brief   System-specific terminal functions for Linux.
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
#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if     !defined(__DECC)

#include <termios.h>

#endif

#include <sys/ioctl.h>

#include "teco.h"
#include "ascii.h"
#include "display.h"
#include "editbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "exec.h"
#include "term.h"


#if     !defined(__DECC)

static struct termios saved_mode;       ///< Saved terminal mode

#endif

static bool term_active = false;        ///< Are terminal settings active?

// Local functions

#if     !defined(DISPLAY_MODE)

static void reset_term(void);

#endif

static void getsize(void);

static void sig_handler(int signal);


///
///  @brief    Reset terminal before we exit from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exit_term(void)
{
    reset_term();
}


///
///  @brief    Get single character from terminal.
///
///  @returns  Character read, or EOF if none available.
///
////////////////////////////////////////////////////////////////////////////////

int getc_term(bool wait)
{
    static bool LF_pending = false;

    if (LF_pending)
    {
        LF_pending = false;

        return LF;
    }

    int c = getchar_dpy(wait);

    if (c == EOF)
    {
        if (f.e0.ctrl_c)                // Error caused by CTRL/C?
        {
            if (f.et.ctrl_c)            // Yes, does user want CTRL/C?
            {
                f.et.ctrl_c = false;    // Reset flag

                return CTRL_C;          // Yes
            }

            echo_in(CTRL_C);
            echo_in(LF);

            throw(E_XAB);               // Execution aborted
        }
    }
    else                                // Got a real character
    {
        f.e0.ctrl_c = false;

        if (c == CR && getlen_tbuf() != 0)
        {
            LF_pending = true;
        }
    }

    return c;
}


///
///  @brief    Get current size of window (if display mode is not active).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void getsize(void)
{
    struct winsize ts;

    if (ioctl(fileno(stdin), (ulong)TIOCGWINSZ, &ts) == -1)
    {
        throw(E_SYS, NULL);             // Unexpected system error
    }

    w.width  = ts.ws_col;
    w.height = ts.ws_row;
}


///
///  @brief    Initialize terminal. Note that this function can be called more
///            than once, because we can start and stop display mode.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_term(void)
{
    static bool init_set = false;       // Initialization already done if true

    // The following only needs to be executed once, regardless
    // how many times terminal initialization is done.

    if (!init_set)
    {
        init_set = true;

#if     !defined(__DECC)

        (void)tcgetattr(fileno(stdin), &saved_mode);

#endif

        struct sigaction sa;

        sa.sa_handler = sig_handler;    // Set up general handler
        sa.sa_flags = 0;

        (void)sigfillset(&sa.sa_mask);  // Block all signals in handler

        (void)sigaction(SIGINT, &sa, NULL);

#if     !defined(__DECC)

        sa.sa_flags = SA_RESTART;       // Restarts are okay for screen resizing

#endif

        (void)sigaction(SIGWINCH, &sa, NULL);

        (void)setvbuf(stdout, NULL, _IONBF, 0uL);

        f.et.rubout    = true;          // Process DEL and ^U in scope mode
        f.et.lower     = true;          // Terminal can read lower case

#if     defined(DISPLAY_MODE)

        f.et.scope     = true;          // Terminal is a scope

#else

        f.et.scope     = false;         // Terminal is not a scope

#endif

        f.et.eightbit  = true;          // Terminal can use 8-bit characters

        getsize();
    }

    // The following is needed only if there is no display active and we haven't
    // already initialized the terminal mode.

    if (!f.e0.display && !term_active)
    {
        term_active = true;

#if     !defined(__DECC)

        struct termios mode;

        (void)tcgetattr(fileno(stdin), &mode);

        // Note: NL below means LF

        mode.c_lflag &= ~ICANON;        // Disable canonical (cooked) mode
        mode.c_lflag &= ~ECHO;          // Disable echo
        mode.c_iflag &= ~ICRNL;         // Don't map CR to NL on input
        mode.c_iflag &= ~INLCR;         // Don't map NL to CR on input
        mode.c_oflag &= ~ONLCR;         // Don't map CR to CR/NL on output

        (void)tcsetattr(fileno(stdin), TCSAFLUSH, &mode);

#endif

    }
}


///
///  @brief    Reset terminal characteristics.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

void reset_term(void)

#else

static void reset_term(void)

#endif

{
    if (term_active)
    {
        term_active = false;

#if     !defined(__DECC)

        (void)tcsetattr(fileno(stdin), TCSAFLUSH, &saved_mode);

#endif

    }
}


///
///  @brief    Signal handler for CTRL/C and display size changes.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void sig_handler(int signum)
{
    switch (signum)
    {
        case SIGINT:
            if (f.et.abort || f.e0.ctrl_c) // Should CTRL/C cause abort?
            {
                echo_in(CTRL_C);
                echo_in(LF);

                exec_EK(NULL);          // Kill any current edit

                if (t.Z != 0)
                {
                    setpos_ebuf(t.B);

                    delete_ebuf(t.Z);   // Kill the whole buffer
                }

                exit(EXIT_FAILURE);     // Cleanup, reset, and exit
            }

            f.e0.ctrl_c = true;         // Set flag saying we saw CTRL/C

            break;

        case SIGWINCH:
            getsize();

#if     defined(DISPLAY_MODE)

            resize_dpy();

#endif

            break;

        default:
            break;
    }
}
