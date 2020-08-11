///
///  @file    term_sys.c
///  @brief   System-specific terminal functions for Linux.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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

#include "teco.h"
#include "ascii.h"
#include "editbuf.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"
#include "term.h"
#include "window.h"


#if     !defined(__DECC)

static struct termios saved_mode;       ///< Saved terminal mode

#endif

static bool term_active = false;        ///< Are terminal settings active?

// Local functions

static void exit_term(void);

static void sig_handler(int signal);


///
///  @brief    Reset terminal before we exit from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exit_term(void)
{
    reset_term();
}


///
///  @brief    Get single character from terminal.
///
///  @returns  Character read, or -1 if none available.
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

    int c = getchar_win(wait);

    if (c == -1)
    {
        if (f.e0.ctrl_c)                // Error caused by CTRL/C?
        {
            if (f.et.ctrl_c)            // Yes, does user want CTRL/C?
            {
                f.et.ctrl_c = false;    // Reset flag

                return CTRL_C;          // Yes
            }

            echo_in(CTRL_C);
            print_echo(CRLF);

            throw(E_XAB);               // Execution aborted
        }
    }
    else                                // Got a real character
    {
        f.e0.ctrl_c = false;

        if (c == CR)
        {
            LF_pending = true;
        }
    }

    return c;
}


///
///  @brief    Initialize terminal. Note that this function can be called more
///            than once, because we can start and stop window/scope mode.
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

        register_exit(exit_term);

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
        f.et.scope     = true;          // Terminal is a scope
        f.et.eightbit  = true;          // Terminal can use 8-bit characters

        getsize_win();
    }

    // The following is needed only if there is no window active and we haven't
    // already initialized the terminal mode.

    if (!f.e0.winact && !term_active)
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
///  @brief    Output bell (CTRL/G) to terminal. This has a separate function
///            because in some operating environments we may choose to flash
///            something on a screen.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void put_bell(void)
{
    print_chr(CTRL_G);
}


///
///  @brief    Reset terminal characteristics.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_term(void)
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
///  @brief    Signal handler for CTRL/C and window size changes.
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
                print_chr(CRLF);

                exec_EK(NULL);          // Kill any current edit

                if (t.Z != 0)
                {
                    setpos_ebuf(t.B);

                    delete_ebuf(t.Z);   // Kill the whole buffer
                }

                exit(EXIT_FAILURE);     // Cleanup, reset, and exit
            }

            f.e0.ctrl_c = true;         // Say we've seen CTRL/C

            break;

        case SIGWINCH:
            getsize_win();

            break;

        default:
            break;
    }
}
