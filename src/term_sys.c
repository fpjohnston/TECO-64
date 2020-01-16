///
///  @file    term_sys.c
///  @brief   System-specific terminal functions for Linux.
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

#undef CURSES                           // TODO: add support for curses
#undef refresh                          // So we can use refresh as a symbol

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"

static struct termios saved_mode;       ///< Saved terminal mode

// Local functions

static void get_window(void);

void reset_term(void);

static void sigint(int signal);

static void sigwin(int signal);


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
#if     defined(TIOCGWINSZ)

    struct winsize ts;

    if (ioctl(fileno(stdin), (ulong)TIOCGWINSZ, &ts) == -1)
    {
        fatal_err(errno, E_SYS, NULL);
    }

    w.width  = ts.ws_col;
    w.height = ts.ws_row;

#elif   defined(TIOCGSIZE)

    struct ttysize ts;

    if (ioctl(fileno(stdin), TIOCGSIZE, &ts) == -1)
    {
        fatal_err(errno, E_SYS, NULL);
    }

    w.width  = ts.ts_cols;
    w.height = ts.ts_lines;

#endif
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
    f.et.rscope    = true;              // Terminal is a refresh scope
    f.et.eightbit  = true;              // Terminal can use 8-bit characters
//    f.et.accent    = true;              // Use accent grave as delimiter

    f.eu           = -1;                // No case flagging

    // TODO: use sigaction() instead of signal()

    if (signal(SIGINT, sigint) == SIG_ERR)
    {
        fatal_err(errno, E_UIT, NULL);
    }

    if (signal(SIGWINCH, sigwin) == SIG_ERR)
    {
        fatal_err(errno, E_UIT, NULL);
    }

    if (siginterrupt(SIGINT, 1) == -1)
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
    // Note: there is no point to adding error checking for this function,
    //       because we're already in the process of exiting when we get here.

    (void)tcsetattr(fileno(stdin), TCSAFLUSH, &saved_mode);
}


///
///  @brief    Signal handler for CTRL/C.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void sigint(int unused1)
{
    // TODO: use sigaction() instead of signal()

    if (signal(SIGINT, SIG_IGN) == SIG_ERR)
    {
        fatal_err(errno, E_SYS, NULL);
    }

    if (f.et.abort || f.e0.ctrl_c)      // Should CTRL/C cause abort?
    {
        echo_in(CTRL_C);
        print_chr(CRLF);

        // TODO: add code for EK and HK commands.

        exit(EXIT_FAILURE);             // Cleanup, reset, and exit
    }

    f.e0.ctrl_c = true;                 // Say we've seen CTRL/C

    if (signal(SIGINT, sigint) == SIG_ERR) // And reset interrupt handler
    {
        fatal_err(errno, E_SYS, NULL);
    }
}


///
///  @brief    Signal handler for window size changes.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void sigwin(int unused1)
{
    // TODO: use sigaction() instead of signal()

    if (signal(SIGWINCH, SIG_IGN) == SIG_ERR)
    {
        fatal_err(errno, E_SYS, NULL);
    }

    get_window();

    if (signal(SIGWINCH, sigwin) == SIG_ERR)
    {
        fatal_err(errno, E_SYS, NULL);
    }
}

