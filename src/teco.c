///
///  @file    teco.c
///  @brief   Main source file for TECO-64 text editor.
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "ascii.h"
#include "display.h"
#include "editbuf.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "file.h"
#include "qreg.h"
#include "search.h"
#include "term.h"


struct flags f;                     ///< Global flag variables

int radix = 10;                     ///< Current output radix

jmp_buf jump_main;                  ///< longjmp() buffer to reset main loop


//  Local functions

static void init_teco(int argc, const char * const argv[]);


///
///  @brief    Main program entry for TECO-64 text editor.
///
///  @returns  The usual exit codes.
///
////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char * const argv[])
{
    init_teco(argc, argv);              // Initialize variables and things

    for (;;)                            // Loop forever
    {
        switch (setjmp(jump_main))
        {
            case 0:                     // Normal entry
                f.e0.trace = false;     // Disable tracing

                f.et.abort = false;     // Don't abort on error

                nparens = 0;            // Reset parenthesis count

                refresh_dpy();          // Refresh display if needed

                read_cmd();             // Read the next command
                init_expr();            // Initialize expression stack

                f.e0.exec = true;       // Executing a command

                exec_cmd(NULL);         // Execute what we have

                f.e0.exec  = false;     // Not executing a command
                f.e0.error = false;     // Command completed w/o error

                break;

            case 1:                     // CTRL/C typed
                reset();                // Reset for next command

                break;

            default:
            case 2:                     // Error
                f.e0.exec = false;      // Not executing a command
                f.e0.error = true;      // Flag the error

                f.et.image  = false;    // Disable image output
                f.et.noecho = false;    // Enable echo for CTRL/T
                f.et.ctrl_o = false;    // Disable CTRL/O
                f.et.nowait = false;    // Enable wait for CTRL/T

                break;
        }

        f.e0.init = false;              // No initialization in progress
    }
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
    f.e0.init    = true;                // Initialization is in progress

    // Default settings for E1

    f.e1.xoper  = true;                 // Enable extended operators
    f.e1.msec   = true;                 // Return time in milliseconds
    f.e1.text   = true;                 // Enable extended text strings
    f.e1.dollar = true;                 // $ is a valid symbol character
    f.e1.ubar   = true;                 // _ is a valid symbol character
    f.e1.new_ei = true;                 // Enable new-style EI commands
    f.e1.bang   = true;                 // !! starts end-of-line comment
    f.e1.prompt = true;                 // Ensure prompt starts in 1st column

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

    f.eu         = -1;                  // No case flagging

    f.ctrl_x     = 0;                   // Searches are case-insensitive

    // Note: this has to be first, since it will be the last called when we
    //       exit, and it might execute a system command.

    init_EG();                          // EG command initialization
    init_mem();                         // Initialize memory allocation
    init_term();                        // Initialize terminal
    init_tbuf();                        // Initialize terminal buffer
    init_cbuf();                        // Initialize command buffer
    init_qreg();                        // Initialize Q-registers
    init_files();                       // Initialize file streams
    init_EI();                          // EI command initialization
    init_loop();                        // Initialize loop stack
    init_search();                      // Initialize search string
    init_env(argc, argv);               // Initialize environment
    init_ebuf(EDITBUF_INIT, EDITBUF_MAX, EDITBUF_STEP, EDITBUF_WARN);
                                        // Initialize edit buffer
}


///
///  @brief    Check for line delimiter (LF, VT, or FF) at current position.
///
///            Note that we return 1/0 instead of true/false for compatibility
///            with the ANSI isxxx() functions.
///
///  @returns  1 if a delimiter found, else 0.
///
////////////////////////////////////////////////////////////////////////////////

int isdelim(int c)
{
    if (c == LF || c == VT || c == FF)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


///
///  @brief    Register function to be called at program exit.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void register_exit(void (*func)(void))
{
    if (atexit(func) != 0)
    {
        throw(E_INI, "Can't register exit function");
                                        // Initialization error
    }
}


///
///  @brief    Do a reset after an error or CTRL/C. This is mostly to do such
///            as freeing up dynamically allocated memory, closing files, and
///            the like.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset(void)
{
    reset_if();                         // Reset conditional stack
    reset_loop();                       // Reset loop stack
    reset_indirect();                   // Close any indirect file
    reset_cbuf((bool)true);             // Reset the input buffer
    reset_qreg();                       // Free up Q-register storage
    reset_macro();                      // Reset macro stack
}
