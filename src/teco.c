///
///  @file    teco.c
///  @brief   Main program for TECO text editor.
///
///  @author  Nowwith Treble Software
///
///  @bug     No known bugs.
///
///  @copyright  tbd
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
///  @mainpage teco - TECO text editor
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"

#define TECO_VERSION    200

const char *prompt = "*";

struct flags f;

int radix = 10;

int teco_version = TECO_VERSION;

int ctrl_x = 0;

bool trace_mode = false;

jmp_buf jump_main;

int form_feed = -1;

// Local functions

static void cleanup(void);


///
///  @brief    Main program entry for TECO text editor.
///
///  @returns  The usual exit codes.
///
////////////////////////////////////////////////////////////////////////////////

int main(int unused1, const char * const argv[])
{
    assert(argv != NULL);               // (This assertion should NEVER fire)

    f.ei.error = false;
    f.ei.strict = true;

    init_env();                         // Initialize environment
    init_term();                        // Initialize terminal
    init_cmd();                         // Initialize command buffer
    init_qreg();                        // Initialize Q-registers

    if (atexit(cleanup) != 0)           // Clean up when we exit
    {
        exit(EXIT_FAILURE);
    }

    for (;;)                            // Loop forever
    {
        if (setjmp(jump_main) == 0)
        {
            read_cmd();                 // Read one or more commands

#if     defined(TEST_SKIP)

            int command = 0;

            while (!empty_cmd())
            {
                const char *buf = next_cmd();
                int len = strlen(buf);
            
                printf("command #%d: ", ++command); fflush(stdout);
                type_cmd(buf, len);
                putc_term(CRLF);

//                skip_cmd();
            }
#else
            exec_cmd();                 // Then execute them
#endif
        }
    }
}


///
///  @brief    General cleanup, initiated when exit() is called.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void cleanup(void)
{
    // TODO: finish these

    print_term("<Close primary input stream>");
    print_term("<Close secondary input stream>");
    print_term("<Close primary output stream>");
    print_term("<Close secondary output stream>");

    print_term("<Free the digit buffer>");
    print_term("<Free the edit buffer>");
    print_term("<Free the filename buffer>");
    print_term("<Free the search buffer>");

    (void)fflush(stdout);
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

    puts_term(prompt, strlen(prompt));
}
