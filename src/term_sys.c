///
///  @file    term_sys.c
///  @brief   System-specific terminal functions for Linux.
///
///  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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
#include <fcntl.h>
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
#include "errors.h"
#include "exec.h"
#include "file.h"
#include "page.h"
#include "term.h"


#if     !defined(__DECC)

static struct termios saved_mode;       ///< Saved terminal mode

#endif

static bool term_active = false;        ///< Are terminal settings active?

#if     defined(DEBUG)

const char *key_name = NULL;            ///< Name of file for keystrokes

static FILE *key_fp = NULL;             ///< Keystroke file descriptor

#endif

static struct sigaction old_act;        ///< Saved action for SIGWINCH signal

static bool old_saved = false;          ///< true if old_act has valid data

// Local functions

static void getsize(void);

static void runaway(const char *msg);

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

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

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

        close_output(OFILE_LOG);        // Close any log file
        kill_edit();                    // Kill the current buffer

        exit(EXIT_SUCCESS);             // Clean up, reset, and exit
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

#if     defined(DEBUG)

    if (key_fp != NULL)
    {
        fclose(key_fp);

        key_fp = NULL;
    }

#endif

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

    // Set up handler for the signals we want to catch.

    struct sigaction sa;

    sa.sa_handler = sig_handler;        // Set up general handler
    sa.sa_flags = 0;

    sigfillset(&sa.sa_mask);            // Block all signals in handler

    sigaction(SIGINT, &sa, NULL);       // This catches Ctrl/C
    sigaction(SIGABRT, &sa, NULL);      // This catches assertion failures
    sigaction(SIGQUIT, &sa, NULL);      // This catches Ctrl-Backslash
    sigaction(SIGSEGV, &sa, NULL);      // This catches segmentation faults

#if     !defined(__DECC)

    sa.sa_flags = SA_RESTART;           // Restarts are okay for screen resizing

#endif

    sigaction(SIGWINCH, &sa, &old_act); // This catches window resizes

    old_saved = true;

    // The following only needs to be executed once, regardless
    // how many times terminal initialization is done.

    if (!init_set)
    {
        init_set = true;

#if     !defined(__DECC)

        if (!f.e0.i_redir)
        {
            tcgetattr(fileno(stdin), &saved_mode);
        }

#endif

        setvbuf(stdout, NULL, _IONBF, 0uL);

        f.et.rubout    = true;          // Process DEL and ^U in scope mode
        f.et.lower     = true;          // Terminal can read lower case
        f.et.scope     = true;          // Terminal is a scope
        f.et.eightbit  = true;          // Terminal can use 8-bit characters

        getsize();                      // Get the current window size

#if     defined(DEBUG)

        if (key_name != NULL)
        {
            if ((key_fp = fopen(key_name, "w+")) != NULL)
            {
                // Write output immediately and do not buffer.

                setvbuf(key_fp, NULL, _IONBF, 0uL);
            }
        }

#endif

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

            tcgetattr(fileno(stdin), &mode);

            // Note: NL below means LF

            mode.c_lflag &= ~ICANON;    // Disable canonical (cooked) mode
            mode.c_lflag &= ~ECHO;      // Disable echo
            mode.c_iflag |=  ICRNL;     // Map CR to NL on input
            mode.c_iflag &= ~INLCR;     // Don't map NL to CR on input
            mode.c_oflag &= ~ONLCR;     // Don't map CR to CR/NL on output

            tcsetattr(fileno(stdin), TCSAFLUSH, &mode);
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

#if     defined(DEBUG)

void putc_key(int c)
{
    if (key_fp != NULL)
    {
        fputc(c, key_fp);
    }
}

#endif


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

        // If we're switching to display mode, reset the signal handler for
        // window resizing so that ncurses will use its own handler instead
        // of ours.

        if (f.e0.display && old_saved)
        {
            sigaction(SIGWINCH, &old_act, NULL);
        }

#if     !defined(__DECC)

        if (!f.e0.i_redir)
        {
            tcsetattr(fileno(stdin), TCSAFLUSH, &saved_mode);
        }

#endif

    }
}


///
///  @brief    Stop TECO and exit due to an unexpected or unrecoverable error.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void runaway(const char *msg)
{
    assert(msg != NULL);

    exit_dpy();                         // Disable display so message is visible

    print_alert(msg);

    close_output(OFILE_LOG);            // Close any log file
    exec_EK(NULL);                      // Kill any current edit
    kill_edit();                        // Kill the current buffer

    exit(EXIT_FAILURE);                 // Clean up, reset, and exit
}


///
///  @brief    Signal handler for CTRL/C and display size changes.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void sig_handler(int signum)
{
    static bool active = false;

    if (active)                         // Double fault?
    {
        abort();                        // Yes, just quit
    }

    active = true;

    switch (signum)
    {
        case SIGABRT:                   // An assertion failure causes this
            type_out(CR);               // Assertion msg. might not include CR

            runaway("Abort");           // Print message and exit

            break;

        case SIGINT:                    // Ctrl-C causes this
            if (f.et.abort || f.e0.sigint) // Should we abort?
            {
                runaway("Cancel");      // Print message and exit
            }
            else if (f.et.ctrl_c)       // Trapping CTRL/C?
            {
                f.et.ctrl_c = false;    // Say we've seen CTRL/C
            }
            else
            {
                f.e0.sigint = true;     // Say that we've seen SIGINT
                f.e0.exec = false;      // Stop any command execution
            }

            break;

        case SIGQUIT:                   // Ctrl-\ causes this
            runaway("Interrupt");       // Print message and exit

            break;

        case SIGSEGV:                   // Segmentation fault
            runaway("Fatal error");     // Print message and exit

            break;

        case SIGWINCH:                  // Window resizing causes this
            getsize();                  // Update the size

            break;

        default:                        // Ignore remaining signals
            break;
    }

    active = false;                     // Done with signal, reset flag
}
