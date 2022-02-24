///
///  @file    term_sys.c
///  @brief   System-specific terminal functions for Linux.
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
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
#include "file.h"
#include "term.h"


#if     !defined(__DECC)

static struct termios saved_mode;       ///< Saved terminal mode

#endif

static bool term_active = false;        ///< Are terminal settings active?

const char *key_name = NULL;            ///< Name of file for keystrokes

static FILE *key_fp = NULL;             ///< Keystroke file descriptor

// Local functions

static void getsize(void);

static void sig_handler(int signal);


///
///  @brief    Detach TECO from terminal. fork() is required for compliance with
///            POSIX standards, so we shouldn't encounter errors when compiling
///            or linking this code, but as a safeguard we only try to detach
///            for known operating environments.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void detach_term(void)
{

#if     defined(__linux) || defined(__win64) || defined(__APPLE__)

    pid_t pid = fork();                 // Create child process

    if (pid < 0)
    {
        throw(E_DET, strerror(errno));  // Detach error
    }

    if (pid == 0)                       // Child process
    {
        // Close streams we won't need while detached

        fclose(stdin);
        fclose(stdout);
        fclose(stderr);

        // This creates a new session, which ensures that the child process
        // is not killed if the parent process is terminated. If we get an
        // error, then errno contains the reason why, but there's not much
        // we can do with it, because we've already detached, so we just abort.

        if (setsid() == -1)
        {
            abort();
        }
    }
    else                                // Parent process
    {
        tprint("Detached child process with ID %u\n", (uint)pid);

        // Clearing the pointers below ensures that exiting the parent process
        // does not delete any output files possibly created during the current
        // editing session, thus allowing the child process, which was given a
        // copy of all the parent's file descriptors, to continue editing any
        // open files.

        ofiles[ostream].name = ofiles[ostream].temp = NULL;

        exec_EK(NULL);                  // Kill any current edit

        if (t.Z != 0)
        {
            setpos_ebuf(t.B);

            delete_ebuf(t.Z);           // Kill the whole buffer
        }

        // Note that we used atexit() when TECO was initialized, so that
        // exit_teco() will be called now to clean up and reset things.

        exit(EXIT_SUCCESS);             // Cleanup, reset, and exit
    }

#else

    throw(E_DET, "Detach not supported");

#endif

}


///
///  @brief    Reset terminal before we exit from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exit_term(void)
{
    if (key_fp != NULL)
    {
        fclose(key_fp);

        key_fp = NULL;
    }

    reset_term();
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

    if (f.e0.i_redir)
    {
        return;                         // Nothing to do if stdin redirected
    }
    else if (ioctl(fileno(stdin), (ulong)TIOCGWINSZ, &ts) == -1)
    {
        throw(E_ERR, NULL);             // General error
    }
    else
    {
        w.width  = ts.ws_col;
        w.height = ts.ws_row;
    }
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

        if (!f.e0.i_redir)
        {
            (void)tcgetattr(fileno(stdin), &saved_mode);
        }

#endif

        struct sigaction sa;

        sa.sa_handler = sig_handler;    // Set up general handler
        sa.sa_flags = 0;

        (void)sigfillset(&sa.sa_mask);  // Block all signals in handler

        (void)sigaction(SIGINT, &sa, NULL);
        (void)sigaction(SIGABRT, &sa, NULL); // (mostly for assertion failures)

#if     !defined(__DECC)

        sa.sa_flags = SA_RESTART;       // Restarts are okay for screen resizing

#endif

        (void)sigaction(SIGWINCH, &sa, NULL);

        (void)setvbuf(stdout, NULL, _IONBF, 0uL);

        f.et.rubout    = true;          // Process DEL and ^U in scope mode
        f.et.lower     = true;          // Terminal can read lower case
        f.et.scope     = true;          // Terminal is a scope
        f.et.eightbit  = true;          // Terminal can use 8-bit characters

        getsize();

        if (key_name != NULL)
        {
            if ((key_fp = fopen(key_name, "w+")) != NULL)
            {
                // Write output immediately and do not buffer.

                (void)setvbuf(key_fp, NULL, _IONBF, 0uL);
            }
        }
    }

    // The following is needed only if there is no display active and we haven't
    // already initialized the terminal mode.

    if (!f.e0.display && !term_active)
    {
        term_active = true;

#if     !defined(__DECC)

        if (!f.e0.i_redir)
        {
            struct termios mode;

            (void)tcgetattr(fileno(stdin), &mode);

            // Note: NL below means LF

            mode.c_lflag &= ~ICANON;    // Disable canonical (cooked) mode
            mode.c_lflag &= ~ECHO;      // Disable echo
            mode.c_iflag |=  ICRNL;     // Map CR to NL on input
            mode.c_iflag &= ~INLCR;     // Don't map NL to CR on input
            mode.c_oflag &= ~ONLCR;     // Don't map CR to CR/NL on output

            (void)tcsetattr(fileno(stdin), TCSAFLUSH, &mode);
        }

#endif

    }
}


///
///  @brief    Output character to keystroke file (if we have one).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void putc_key(int c)
{
    if (key_fp != NULL)
    {
        fputc(c, key_fp);
    }
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

        if (!f.e0.i_redir)
        {
            (void)tcsetattr(fileno(stdin), TCSAFLUSH, &saved_mode);
        }

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
    static bool aborting = false;

    switch (signum)
    {
        case SIGABRT:
            //  The following line is included because any assertion failure
            //  message will have been output before resetting the terminal
            //  characteristics, so any <LF> would not have included a <CR>.
            //  But even if it did, an extra <CR> won't hurt.

            type_out(CR);

            if (aborting)
            {
                reset_term();           // Reset terminal characteristics
            }
            else
            {
                aborting = true;        // Prevent aborts during aborts

                exec_EK(NULL);          // Kill any current edit

                if (t.Z != 0)
                {
                    setpos_ebuf(t.B);

                    delete_ebuf(t.Z);   // Kill the whole buffer
                }

                // Note that we used atexit() when TECO was initialized, so that
                // exit_teco() will be called now to clean up and reset things.

                exit(EXIT_FAILURE);     // Cleanup, reset, and exit
            }

            break;

        case SIGINT:
            if (f.et.abort || f.e0.ctrl_c) // Should CTRL/C cause abort?
            {
                echo_in(CTRL_C);
                exec_EK(NULL);          // Kill any current edit

                if (t.Z != 0)
                {
                    setpos_ebuf(t.B);

                    delete_ebuf(t.Z);   // Kill the whole buffer
                }

                // Note that we used atexit() when TECO was initialized, so that
                // exit_teco() will be called now to clean up and reset things.

                exit(EXIT_FAILURE);     // Cleanup, reset, and exit
            }

            f.e0.ctrl_c = true;         // Set flag saying we saw CTRL/C

            break;

        case SIGWINCH:
            getsize();

#if     defined(DISPLAY_MODE)

            resize_signal();

#endif

            break;

        default:
            break;
    }
}
