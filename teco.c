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
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "exec.h"
#include "qreg.h"
#include "textbuf.h"
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

jmp_buf jump_main;                  ///< longjmp() buffer to reset main loop

const char *mung_file = NULL;       ///< Name of file to MUNG

const char *log_file = NULL;        ///< Name of log file

///
///  @brief    Main program entry for TECO text editor.
///
///  @returns  The usual exit codes.
///
////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char * const argv[])
{
    f.ctrl_x    = 0;                    // Case-insensitive searches
    f.et.abort  = true;                 // Abort on error
//    f.e1.strict = true;                 // Strictly enforce syntax
    f.e2.dollar = true;                 // Allow dollar signs in symbols
    f.e2.ubar   = true;                 // Allow underscores in symbols
    f.e3.brace  = true;                 // Allow braced expressions
    f.e3.tilde  = true;                 // Allow tilde operator
    f.e3.msec   = true;                 // Return time in milliseconds

    init_env(argc, argv);               // Initialize environment
    init_term();                        // Initialize terminal
    init_buf();                         // Initialize command buffer
    init_qreg();                        // Initialize Q-registers
    init_files();                       // Initialize file streams
    init_EG();                          // Initialize EG command
    init_EI();                          // Initialize EI command
    init_loop();                        // Initialize loop stack
    init_search();                      // Initialize search string
    init_tbuf(EDIT_BUF_SIZE, (64 * 1024), EDIT_BUF_SIZE, 75);
                                        // Initialize edit buffer
    init_window();                      // Initialize window if needed

    // If a log file was requested on the command line, then open it now.

    if (log_file != NULL)
    {
        (void)open_output(&ofiles[OFILE_LOG], 'L', log_file,
                          (uint)strlen(log_file));
    }

    for (;;)                            // Loop forever
    {
        f.e0.exec  = false;             // Not executing command
        f.et.abort = false;             // Don't abort on error
        v.trace    = false;             // Disable tracing

        refresh_win();                  // Refresh window if needed

        switch (setjmp(jump_main))
        {
            case 0:                     // Normal entry
                if (!check_indirect())  // Indirect command to execute yet?
                {
                    read_cmd();         // No, read from terminal
                }

                init_expr();            // Initialize expression stack

                exec_cmd();             // Then execute what we have

                break;

            case 1:                     // CTRL/C typed
                reset_buf();            // TODO: is this correct?

                break;

            default:
            case 2:                     // Error
                close_indirect();       // Close any indirect file
                reset_buf();            // Reset the input buffer
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
