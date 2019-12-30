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
#include "exec.h"
#include "qreg.h"
#include "textbuf.h"


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
    .ctrl_x = 0,                    ///< CTRL/X flag
    .ff     = false,                ///< Form feed flag
    .trace  = false,                ///< Trace mode
    .warn   = false,                ///< Edit buffer is almost full
    .full   = false,                ///< Edit buffer is full
};

int teco_version = TECO_VERSION;    ///< TECO version number (200+)

jmp_buf jump_main;                  ///< longjmp() buffer to reset main loop

bool teco_debug = false;            ///< true if debugging features enabled

const char *mung_file = NULL;       ///< Name of file to MUNG


///
///  @brief    Main program entry for TECO text editor.
///
///  @returns  The usual exit codes.
///
////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char * const argv[])
{
    init_env(argc, argv);               // Initialize environment
    init_term();                        // Initialize terminal
    init_buf();                         // Initialize command buffer
    init_tbuf(EDIT_BUF_SIZE, (64 * 1024), EDIT_BUF_SIZE, 75);
                                        // Initialize edit buffer
    init_qreg();                        // Initialize Q-registers
    init_files();                       // Initialize file streams
    init_EG();                          // Initialize for EG command
    init_loop();                        // Initialie for loops

    f.ei.strict = true;                 // TODO: temporary

    for (;;)                            // Loop forever
    {
        f.ei.exec = false;              // Not executing command

        int jump = setjmp(jump_main);

        if (jump == 0)
        {
            if (!check_indirect())      // Indirect command to execute yet?
            {
                read_cmd();             // No, read from terminal
            }

            init_expr();                // Initialize expression stack

            exec_cmd();                 // Then execute what we have
        }
        else if (jump == 1)             // ^C exit from macro
        {
            (void)fflush(NULL);
            reset_buf();
        }
        else if (jump == 2)             // Error occurred
        {
            (void)fflush(NULL);         // Flush all streams
            close_indirect();           // Close any indirect file
            reset_buf();                // Reset the input buffer
            reset_qreg();               // Free up Q-register storage
            reset_if();                 // Reset if statement depth
            reset_loop();               // Free up loop structures
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
