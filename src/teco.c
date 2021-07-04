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
///  It is a complete re-write of version 147 of TECO C, and is a work in
///  progress. Further documentation will be added later.
///
///  @version 200
///
///  @author   Franklin P. Johnston / Nowwith Treble Software
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdlib.h>

#include "teco.h"
#include "cbuf.h"
#include "display.h"
#include "editbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "file.h"
#include "qreg.h"
#include "search.h"
#include "term.h"


struct flags f;                     ///< Global flag variables

jmp_buf jump_main;                  ///< longjmp() buffer to reset main loop


//  Local functions

static void exit_teco(void);

static void init_teco(int argc, const char * const argv[]);


///
///  @brief    Main program entry for TECO-64 text editor.
///
///  @returns  The usual exit codes.
///
////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char * const argv[])
{
    f.e0.flag = 0;
    f.e0.init = true;                   // Starting TECO
    init_teco(argc, argv);              // Initialize variables and things
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
                f.et.ctrl_o = false;    // Disable CTRL/O
                f.et.nowait = false;    // Enable wait for CTRL/T
                f.et.ctrl_c = false;    // Disable CTRL/C trap
                //lint -fallthrough

            case MAIN_CTRLC:            // CTRL/C typed
                reset();                // Reset everything

                break;

            default:
                break;
        }

        f.e0.exec = false;              // No command active

        if (f.e0.init)
        {
            f.e0.init = false;          // Not initializing now

            reset();
        }
    }
}


///
///  @brief    Exit TECO, after calling cleanup functions. Note that the
///            ordering of the first two or three function calls, as well as
///            the last two function calls, is significant, but the remaining
///            function calls isn't especially required.
///
///  @returns  Nothing (also, it may not return, depending on whether an EG
///            command was issued).
///
////////////////////////////////////////////////////////////////////////////////

static void exit_teco(void)
{

#if     defined(TECO_DISPLAY)

    exit_dpy();                         // Disable display first

#endif

    exit_term();                        // Restore terminal settings next
    exit_files();                       // Close any open files

    exit_tbuf();                        // Deallocate memory for terminal buffer
    exit_cbuf();                        // Deallocate memory for command buffer
    exit_ebuf();                        // Deallocate memory for edit buffer
    exit_map();                         // Deallocate memory for key mapping
    exit_error();                       // Deallocate memory for errors
    exit_search();                      // Deallocate memory for searches
    exit_loop();                        // Deallocate memory for loops
    exit_EI();                          // Deallocate memory for EI commands
    exit_qreg();                        // Deallocate memory for Q-registers

#if     defined(TECO_DEBUG)

    exit_mem();                         // Deallocate memory blocks

#endif

    exit_EG();                          // Check for possible system command
}


///
///  @brief    Initialize everything required to start TECO, and set some
///            defaults.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void init_teco(int argc, const char * const argv[])
{
    // Default settings for E1

    f.e1.xoper  = true;                 // Enable extended operators
    f.e1.text   = true;                 // Enable extended text strings
    f.e1.dollar = true;                 // $ is a valid symbol character
    f.e1.ubar   = true;                 // _ is a valid symbol character
    f.e1.new_ei = true;                 // Enable new-style EI commands
    f.e1.bang   = true;                 // !! starts end-of-line comment
    f.e1.prompt = true;                 // Ensure prompt starts in 1st column
    f.e1.radix  = true;                 // Allow in-line radix control

    // Default settings for E2

    f.e2.flag    = 0;                   // All bits off

    // Default settings for E3

#if   defined(__vms)

    f.e3.nopage = false;                // FF is page delimiter

#else

    f.e3.nopage = true;                 // FF is normal character

#endif

    f.e3.smart = true;                  // Enable smart line detection

#if     defined(__linux) || defined(__APPLE__)

    f.e3.icrlf = false;                 // Use LF for input lines
    f.e3.ocrlf = false;                 // Use LF for output lines

#elif   defined(__win64) || defined(__vms)

    f.e3.icrlf = true;                  // Use CR/LF for input lines
    f.e3.ocrlf = true;                  // Use CR/LF for output lines

#else

#error  Unknown operating environment

#endif

    f.e4.line   = true;                 // Line between text & command regions
    f.e4.status = true;                 // Display status on line

#if     defined(TECO_DISPLAY)

    f.ed.escape  = true;                // Enable ESCape-sequences

#else

    f.ed.escape  = false;               // Disable ESCape-sequences

#endif

    f.et.abort   = true;                // Abort on error during initialization
    f.et.accent  = true;                // Use accent grave as delimiter

#if     defined(CONFIG_EU)

    f.eu         = -1;                  // No case flagging

#endif

    f.ctrl_x     = 1;                   // Searches are case-insensitive
    f.radix      = 10;                  // Use decimal radix

    init_term();                        // Initialize terminal
    init_tbuf();                        // Initialize terminal buffer
    init_cbuf();                        // Initialize command buffer
    init_qreg();                        // Initialize Q-registers
    init_files();                       // Initialize file streams
    init_loop();                        // Initialize loop stack
    init_env(argc, argv);               // Initialize environment
    init_ebuf(EDITBUF_INIT, EDITBUF_MAX, EDITBUF_STEP, EDITBUF_WARN);
                                        // Initialize edit buffer

    if (atexit(exit_teco) != 0)
    {
        throw(E_INI, "Can't register exit function");
                                        // Initialization error
    }
}


///
///  @brief    Reset everything before starting a new command. This frees up
///            allocated, closes files, etc.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset(void)
{
    reset_if();                         // Reset conditional stack
    reset_loop();                       // Reset loop stack
    reset_cbuf();                       // Reset the input buffer
    reset_indirect();                   // Reset indirect command file
    reset_qreg();                       // Free up Q-register storage
    reset_macro();                      // Reset macro stack
}
