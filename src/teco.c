///
///  @file       teco.c
///  @brief      Main program for TECO text editor.
///
///  @bug        No known bugs.
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
#include "eflags.h"


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
    .b      = 0,                    ///< Beginning of buffer (always 0)
    .z      = 1000,                 ///< End of buffer (no. of chrs. in buffer)
    .dot    = 0,                    ///< Current pointer position in buffer
    .eof    = 0,                    ///< End of file flag
    .ff     = -1,                   ///< Form feed flag
    .radix  = 10,                   ///< Current output radix
    .ctrl_x = 0,                    ///< CTRL/X flag
    .trace  = false,                ///< Trace mode
};

///  @var     teco_version
///  @brief   TECO version number (200+).

int teco_version = TECO_VERSION;

///  @var     jump_main
///  @brief   longjmp() buffer used to return to main loop after error.

jmp_buf jump_main;

///  @var     teco_debug
///  @brief   true if TECO debugging features are enabled.

bool teco_debug = false;


///
///  @brief    Main program entry for TECO text editor.
///
///  @returns  The usual exit codes.
///
////////////////////////////////////////////////////////////////////////////////

int main(int unused1, const char * const argv[])
{
    assert(argv != NULL);               // (This assertion should NEVER fire)

    f.ei.strict = true;                 // TODO: temporary

    init_EG();                          // Initialize for EG command
    init_env();                         // Initialize environment
    init_term();                        // Initialize terminal
    init_buf();                         // Initialize command buffer
    init_edit(EDIT_BUF_SIZE, EDIT_BUF_SIZE, 75, (bool)true);
                                        // Initialize edit buffer
    init_qreg();                        // Initialize Q-registers
    init_files();                       // Initialize file streams

    for (;;)                            // Loop forever
    {
        if (setjmp(jump_main) == 0)
        {
            if (!read_indirect())       // Indirect command to execute yet?
            {
                read_cmd();             // No, read from terminal
            }

            init_expr();                // Initialize expression stack

            exec_cmd();                 // Then execute what we have
        }
        else                            // We get here on error
        {
            reset_buf();
        }
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
    f.et.abort = false;                 // Don't abort on error

    puts_term(prompt, (uint)strlen(prompt));
}
