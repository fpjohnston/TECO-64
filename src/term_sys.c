///
///  @file    term_sys.c
///  @brief   System-specific terminal functions for Linux.
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
#include <ctype.h>
#include <curses.h>
#include <errno.h>
#include <signal.h>

#if     !defined(_STDBOOL_H)

#include <stdbool.h>

#endif

#if     !defined(_STDIO_H)

#include <stdio.h>

#endif

#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"
#include "textbuf.h"
#include "window.h"


static struct termios saved_mode;       ///< Saved terminal mode

// Local functions

static void get_window(void);

static void reset_term(void);

static void sig_handler(int signal);


///
///  @brief    Get single character from terminal.
///
///  @returns  Character read.
///
////////////////////////////////////////////////////////////////////////////////

int getc_term(bool wait)
{
    static bool LF_pending = false;

    int c = 0;

    if (LF_pending)
    {
        LF_pending = false;

        return LF;
    }

    if (!wait)
    {
        c = getch();                    // If no input, c = EOF
    }
    else if (read(fileno(stdin), &c, 1uL) == -1)
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

            print_err(E_XAB);           // Execution aborted
        }
        else                            // Something other than CTRL/C
        {
            fatal_err(errno, E_URC, NULL);
        }
    }

    f.e0.ctrl_c = false;                // Something other than CTRL/C

    if (f.et.rubout)
    {
        if (c == DEL)
        {
            return BS;
        }
        else if (c == BS)
        {
            return DEL;
        }
    }

    if (c == CR)
    {
        LF_pending = true;
    }

    return c;
}


///
///  @brief    Get size of terminal window (rows & columns).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void get_window(void)
{
    if (get_winsize((int *)&w.width, (int *)&w.height))
    {
        return;
    }
    else
    {
        struct winsize ts;

        if (ioctl(fileno(stdin), (ulong)TIOCGWINSZ, &ts) == -1)
        {
            fatal_err(errno, E_SYS, NULL);
        }

        w.width  = ts.ws_col;
        w.height = ts.ws_row;
    }
}


///
///  @brief    Initialize terminal.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_term(void)
{
    struct termios mode;

    if (tcgetattr(fileno(stdin), &mode) == -1)
    {
        fatal_err(errno, E_UIT, NULL);
    }

    saved_mode = mode;

    if (atexit(reset_term) != 0)
    {
        fatal_err(errno, E_UIT, NULL);
    }

    if (setvbuf(stdout, NULL, _IONBF, 0uL) != 0)
    {
        fatal_err(errno, E_UIT, NULL);
    }

    // Note: NL below means LF

    mode.c_lflag &= ~ICANON;            // Disable canonical (cooked) mode
    mode.c_lflag &= ~ECHO;              // Disable echo
    mode.c_iflag &= ~ICRNL;             // Don't map CR to NL on input
    mode.c_iflag &= ~INLCR;             // Don't map NL to CR on input
    mode.c_oflag &= ~ONLCR;             // Don't map CR to CR/NL on output

    if (tcsetattr(fileno(stdin), TCSAFLUSH, &mode) == -1)
    {
        fatal_err(errno, E_UIT, NULL);
    }

    get_window();

    f.et.rubout    = true;              // Process DEL and ^U in scope mode
    f.et.lower     = true;              // Terminal can read lower case
    f.et.scope     = true;              // Terminal is a scope
//    f.et.rscope    = true;              // Terminal is a refresh scope
    f.et.eightbit  = true;              // Terminal can use 8-bit characters
//    f.et.accent    = true;              // Use accent grave as delimiter

    f.eu           = -1;                // No case flagging

    struct sigaction sa;

    sa.sa_handler = sig_handler;        // Set up general handler
    sa.sa_flags = 0;

    (void)sigfillset(&sa.sa_mask);      // Block all signals in handler

    if (sigaction(SIGINT,   &sa, NULL) == -1)
    {
        fatal_err(errno, E_UIT, NULL);
    }

    sa.sa_handler = sig_handler;        // Set up general handler
    sa.sa_flags = SA_RESTART;

    (void)sigfillset(&sa.sa_mask);      // Block all signals in handler

    if (sigaction(SIGWINCH, &sa, NULL) == -1)
    {
        fatal_err(errno, E_UIT, NULL);
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
//    print_chr(CTRL_G);
}


///
///  @brief    Reset terminal characteristics.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void reset_term(void)
{
    // Note: there is no point to adding error checking for this function,
    //       because we're already in the process of exiting when we get here.

    (void)tcsetattr(fileno(stdin), TCSAFLUSH, &saved_mode);
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

                int Z = (int)getsize_tbuf();

                if (Z != 0)
                {
                    setpos_tbuf(B);

                    delete_tbuf(Z);     // Kill the whole buffer
                }

                exit(EXIT_FAILURE);     // Cleanup, reset, and exit
            }

            f.e0.ctrl_c = true;         // Say we've seen CTRL/C

            break;

        case SIGWINCH:
            get_window();

            break;

        default:
            break;
    }
}
