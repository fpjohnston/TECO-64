///
///  @file    teco.c
///  @brief   Main source file for TECO-64 text editor.
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
///  @mainpage TECO-64 - TECO text editor for 64-bit environments.
///
///  This is an implementation in C of the TECO text editor, for 64-bit
///  operating environments.
///
///  It is a complete re-write of version 147 of TECO C, and is a work in pro-
///  gress. Additional documentation is available in Markdown files included
///  as part of the same distribution.
///
///  @version 200
///
///  @author   Franklin P. Johnston / Nowwith Treble Software
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <limits.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "cbuf.h"
#include "display.h"
#include "editbuf.h"
#include "eflags.h"
#include "estack.h"
#include "exec.h"
#include "file.h"
#include "qreg.h"
#include "term.h"

//lint -save -e10 -e65 -e133 -e485 -e651

struct flags f =                    ///< Global flag variables
{
    .ctrl_e = 0,                    // Don't append form feed on output
    .ctrl_x = 1,                    // Searches are case-insensitive

    .e0.flag = 0,                   // All bits off in E0 flag

    .e1.xoper   = true,             // Enable extended operators
    .e1.text    = true,             // Enable extended text strings
    .e1.ctrl_a  = true,             // Enable colon-modified ^A commands
    .e1.equals  = true,             // Enable at-sign modified '=' commands
    .e1.new_ei  = true,             // Enable new-style EI commands
    .e1.bang    = true,             // !! starts end-of-line comment
    .e1.prompt  = true,             // Ensure prompt starts in 1st column
    .e1.radix   = true,             // Allow in-line radix control
    .e1.insert  = true,             // Allow nI w/o requiring n@I
    .e1.percent = true,             // Allow :%q
    .e1.newline = false,            // LF does not act like double ESCape

    .e2.flag = 0,                   // All bits off in E2 flag

#if     defined(__vms)

    .e3.nopage = false,             // FF is page delimiter

#else

    .e3.nopage = true,              // FF is normal character

#endif

    .e3.smart = true,               // Enable smart line detection

#if     defined(__linux) || defined(__APPLE__)

    .e3.CR_in   = false,            // Use LF for input lines
    .e3.CR_out  = false,            // Use LF for output lines

#elif   defined(__win64) || defined(__vms)

    .e3.CR_in   = true,             // Use CR/LF for input lines
    .e3.CR_out  = true,             // Use CR/LF for output lines

#else
#error  Unknown operating environment
#endif

    .e3.CR_type = true,             // Use CR/LF for typed lines

#if   defined(__vms)

    .e3.keepNUL = true,             // Don't discard input NUL chrs.

#else

    .e3.keepNUL = false,            // Discard input NUL chrs.

#endif

    .e4.line   = true,              // Line between text & command regions
    .e4.status = true,              // Display status on line

#if     defined(DISPLAY_MODE)

    .ed.escape  = true,             // Enable ESCape-sequences

#else

    .ed.escape  = false,            // Disable ESCape-sequences

#endif

    .ee = NUL,                      // No ESCape surrogate
    .eh.verbose = 2,                // Use standard verbosity for error msgs.
    .eh.line = true,                // Include line number for errors in macros

#if     defined(TEST)

    .eh.func = true,                // Include function information

#endif

    .ej = 0,                        // Operating system type
    .eo = 0,                        // TECO version number
    .es = 0,                        // Search verification flag
    .et.accent  = true,             // Use accent grave as delimiter
    .eu = -1,                       // No case flagging
    .ev = 0,                        // Don't print any lines before prompt
    .radix = 10,                    // Use decimal radix
    .trace.flag = 0,                // All trace bits off
};

//lint -restore

jmp_buf jump_main;                  ///< longjmp() buffer to reset main loop

char scratch[PATH_MAX];             ///< General scratch buffer


//  Local functions

static void exit_teco(void);

static void init_teco(int argc, const char * const argv[]);

static void reset_teco(void);


///
///  @brief    Main program entry for TECO-64 text editor.
///
///  @returns  The usual exit codes.
///
////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char * const argv[])
{
    init_teco(argc, argv);              // Initialize variables and things

    // After initialization is complete, we can "execute" the command-line
    // options we found, which usually consists of creating a string of
    // commands and inserting them into the command buffer.

    exec_options(argc, argv);           // Execute command-line options

    f.e0.init = (cbuf->pos != cbuf->len); // Reset if no initial command string

    for (;;)                            // Loop forever
    {
        struct cmd cmd = null_cmd;      // Command block

        switch (setjmp(jump_main))
        {
            case MAIN_NORMAL:           // Normal entry
                refresh_dpy();          // Update display if needed

                f.trace.flag = 0;       // Switch off all tracing bits
                f.et.abort = false;     // Don't abort on error

                nparens = 0;            // Reset parenthesis count

                if (!f.e0.init && !read_EI())
                {
                    read_cmd();         // Read input from terminal
                }

                init_x();               // Initialize expression stack

                f.e0.exec = true;       // Command is in progress
                exec_cmd(&cmd);         // Execute command string

                f.e0.error = false;     // Command completed w/o error

                break;

            case MAIN_ERROR:            // Error
                f.e0.error = true;      // Flag the error

                f.et.image  = false;    // Disable image output
                f.et.noecho = false;    // Enable echo for CTRL/T
                f.et.nowait = false;    // Enable wait for CTRL/T
                f.et.ctrl_c = false;    // Disable CTRL/C trap
                //lint -fallthrough

            case MAIN_CTRLC:            // CTRL/C typed
                reset_teco();           // Reset everything

                break;

            default:
                break;
        }

        f.e0.exec = false;              // No command active

        if (f.e0.init)
        {
            f.e0.init = false;          // Not initializing now

            reset_teco();
        }
    }
}


///
///  @brief    Exit TECO, after calling cleanup functions. Note that the
///            ordering of the first two or three function calls, as well as
///            the last two function calls, is significant, but the ordering of
///            the remaining function calls isn't especially important.
///
///  @returns  Nothing (also, it may not return, depending on whether an EG
///            command was issued).
///
////////////////////////////////////////////////////////////////////////////////

static void exit_teco(void)
{

#if     defined(DISPLAY_MODE)

    exit_dpy();                         // Disable display first

#endif

    exit_term();                        // Restore terminal settings next
    exit_files();                       // Close any open files

    reset_loop();                       // Deallocate memory for loops
    reset_indirect();                   // Deallocate memory for EI commands
    reset_search();                     // Deallocate memory for last search

    exit_map();                         // Deallocate memory for key mapping
    exit_error();                       // Deallocate memory for errors
    exit_qreg();                        // Deallocate memory for Q-registers
    exit_ebuf();                        // Deallocate memory for edit buffer
    exit_cbuf();                        // Deallocate memory for command buffer
    exit_tbuf();                        // Deallocate memory for terminal buffer

#if     defined(MEMCHECK)

    exit_mem();                         // Deallocate memory blocks

#endif

    exit_EG();                          // Check for possible system command
}


///
///  @brief    Initialize everything required to start TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void init_teco(int argc, const char * const argv[])
{
    f.e0.init = true;                   // TECO initialization is in progress
    f.et.abort = true;                  // Abort on error during initialization

    if (isatty(fileno(stdin)) == 0)     // Has stdin been redirected?
    {
        f.e0.i_redir = true;
    }

    if (isatty(fileno(stdout)) == 0)    // Has stdout been redirected?
    {
        f.e0.o_redir = true;
    }

    if (atexit(exit_teco) != 0)
    {
        printf("%s(): atexit() failure\n", __func__);

        exit(EXIT_FAILURE);             // Initialization error
    }

    init_env();                         // Initialize environment variables
    init_options(argc, argv);           // Initialize command-line options

    // Allocate memory that we'll need when executing commands.

    init_tbuf();                        // Initialize terminal buffer
    init_cbuf();                        // Initialize command buffer
    init_ebuf();                        // Initialize edit buffer
    init_qreg();                        // Initialize Q-registers

    // Change terminal characteristics and set signal handlers.

    init_term();                        // Initialize terminal settings
}


///
///  @brief    Reset everything before starting a new command. This frees up
///            some allocated memory, closes files, etc.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void reset_teco(void)
{
    init_x();                           // Reinitialize expression stack
    reset_if();                         // Reset conditional stack
    reset_loop();                       // Reset loop stack
    reset_cbuf();                       // Reset the input buffer
    reset_indirect();                   // Reset indirect command file
    reset_qreg();                       // Free up local Q-register storage
    reset_macro();                      // Reset macro stack
}
