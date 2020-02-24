///
///  @file       teco.c
///  @brief      Main program for TECO text editor.
///
///  @bug        No known bugs.
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
///  @mainpage TECO 64 - TECO text editor for 64-bit environments.
///
///  This is an implementation in C of the TECO text editor for 64-bit operating
///  environments.
///
///  It is a complete re-write of version 147 of TECO C, and is a work in
///  progress. Further documentation will be added later.
///
///  @version 200
///
///  @author   Franklin P. Johnston / Nowwith Treble Software
///
///  @remarks  Previous authors of TECO, who deserve credit for their work on
///            TECO, and on whose the shoulders I stood to develop this version.
///
///  @remarks  Dan Murphy - Created TECO on a PDP-1 in 1963 at MIT.
///
///  @remarks  Mark Bramhall - Developed TECO-11.
///
///  @remarks  Stan Rabinowitz - Standardized features of TECO-8, TECO-10, and
///                              TECO-11.
///
///  @remarks  Rick Murphy - Translated TECO-11 to TECO-32.
///
///  @remarks  Andy Goldstein - Developed TECO-11 and TECO-32.
///
///  @remarks  Pete Siemsen - Developed TECO C in 1983. The first TECO written
///                           in a language other than assembler.
///
///  @remarks  Ben Mesander - ?
///
///  @remarks  Jerry Leichter - ?
///
///  @remarks  Tom Almy - Developed TECO C.
///
///  @remarks  Mark Henderson - Developed TECO C.
///
///  @remarks  Blake McBride - Developed version 147 of TECO C, the most recent
///                            version as of October 2019.
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "ascii.h"
#include "editbuf.h"
#include "eflags.h"
#include "exec.h"
#include "qreg.h"
#include "window.h"


#define TECO_VERSION    200             ///< Our version of TECO

#define EDIT_BUF_SIZE   (16 * 1024)     ///< Edit buffer is initially 16KB

///  @var     prompt
///  @brief   Command-line prompt (usually '*').

const char *prompt = "*";

///  @var     f
///  @brief   Global flag variables.

struct flags f;

///  @var     v
///  @brief   Global variables that the user can display or set.

struct vars v =
{
    .radix  = 10,                   ///< Current output radix
    .ctrl_s = 0,                    ///< CTRL/S flag
    .ff     = false,                ///< Form feed flag
    .trace  = false,                ///< Trace mode
    .warn   = false,                ///< Edit buffer is almost full
    .full   = false,                ///< Edit buffer is full
};

int teco_version = TECO_VERSION;    ///< TECO version number (200+)

/// @var    main_active
///
/// @brief  This flag specifies whether a call to setjmp() has been issued,
///         and that the main loop is fully active. This is used by the error-
///         handling functions so that they don't attempt a longjmp() call
///         before initialization has been completed. If it hasn't, then any
///         error becomes fatal and causes an immediate exit.
///         

bool main_active = false;

jmp_buf jump_main;                  ///< longjmp() buffer to reset main loop


///
///  @brief    Main program entry for TECO text editor.
///
///  @returns  The usual exit codes.
///
////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char * const argv[])
{
    f.ctrl_x     = 0;                   // Searches are case-insensitive

    f.eu         = -1;                  // No case flagging

    f.et.abort   = true;                // Abort on error
    f.et.accent  = true;                // Use accent grave as delimiter

    f.e1.strict  = true;                // Strictly enforce syntax
    f.e1.winline = true;                // Line between text & command regions

    f.e2.in_crlf = true;                // Convert CR/LF to LF on input
    f.e2.out_lf  = OUT_CRLF;            // Convert LF to CR/LF on output
    f.e2.dollar  = true;                // Allow dollar signs in symbols
    f.e2.ubar    = true;                // Allow underscores in symbols

    f.e3.brace   = true;                // Allow braced expressions
    f.e3.tilde   = true;                // Allow tilde operator
    f.e3.msec    = true;                // Return time in milliseconds

    // init_win() must be called before init_term(), because the latter will
    // skip some of its initialization if we're going to be in scope mode.

    if (f.e0.window)
    {
        init_win();                     // Initialize window
    }

    init_term();                        // Initialize terminal

    // After we initialize the terminal settings, and therefore know our width
    // and height, check to see if the user wants to start with a split screen.

    if (f.e0.window && w.nscroll != 0)
    {
        clear_win();
    }

    init_tbuf();                        // Initialize terminal buffer
    init_cbuf();                        // Initialize command buffer
    init_qreg();                        // Initialize Q-registers
    init_files();                       // Initialize file streams
    init_EG();                          // Initialize EG command
    init_loop();                        // Initialize loop stack
    init_search();                      // Initialize search string
    init_env(argc, argv);               // Initialize environment

    // TODO: magic numbers for initial buffer size and percentage

    init_ebuf(EDIT_BUF_SIZE, (64 * 1024), EDIT_BUF_SIZE, 75);
                                        // Initialize edit buffer
    main_active = true;                 // Initialization is complete

    for (;;)                            // Loop forever
    {
        switch (setjmp(jump_main))
        {
            case 0:                     // Normal entry
                f.e0.exec  = false;     // Not executing command
                f.et.abort = false;     // Don't abort on error
                v.trace    = false;     // Disable tracing

                refresh_win();          // Refresh window if needed

                // If command buffer empty, read another command

                if (current->pos == current->len)
                {
                    read_cmd();
                }

                init_expr();            // Initialize expression stack
                exec_cmd();             // Execute what we have

                break;

            case 1:                     // CTRL/C typed
                reset_tbuf();           // Reset terminal buffer
                reset_cbuf();           // TODO: is this correct?

                break;

            default:
            case 2:                     // Error
                reset_indirect();       // Close any indirect file
                reset_tbuf();            // Reset terminal buffer
                reset_cbuf();           // Reset the input buffer
                reset_qreg();           // Free up Q-register storage
                reset_if();             // Reset if statement depth
                reset_loop();           // Free up loop structures

                macro_depth = 0;

                break;
        }
    }
}


///
///  @brief    Check for delimiter (LF, VT, or FF) at current position.
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
///  @brief    Print the TECO prompt (this may be the standard asterisk, or
///            something else specified by the user).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_prompt(void)
{
    print_str("%s", prompt);
}
