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

bool teco_debug = false;

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

    f.ei.strict = true;                 // TODO: temporary

    init_env();                         // Initialize environment
    init_term();                        // Initialize terminal
    init_buf();                         // Initialize command buffer
    init_qreg();                        // Initialize Q-registers
    init_files();                       // Initialize file streams

    if (atexit(cleanup) != 0)           // Clean up when we exit
    {
        exit(EXIT_FAILURE);
    }

    for (;;)                            // Loop forever
    {
        if (setjmp(jump_main) == 0)
        {
            read_cmd();                 // Read one or more commands

            init_expr();                // Initialize expression stack

            exec_cmd();                 // Then execute what we have
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

    print_term("<TBD: free digit buffer>");
    print_term("<TBD: free edit buffer>");
    print_term("<TBD: free search buffer>");

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


///
///  @brief    Get more memory.
///
///  @returns  Nothing (error if memory allocation fails).
///
////////////////////////////////////////////////////////////////////////////////

void *alloc_more(void *ptr, size_t size)
{
    // TODO: check for relative size change?

    void *newptr = realloc(ptr, size);

    if (newptr == NULL)
    {
        print_err(E_MEM);               // Memory overflow)
    }

    return newptr;
}


///
///  @brief    Get new memory.
///
///  @returns  Nothing (error if memory allocation fails).
///
////////////////////////////////////////////////////////////////////////////////

void *alloc_new(size_t size)
{
    void *ptr = calloc(1, size);

    if (ptr == NULL)
    {
        print_err(E_MEM);               // Memory overflow
    }

    return ptr;
}


///
///  @brief    Deallocate memory.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void dealloc(char **ptr)
{
    assert(ptr != NULL);                // Make sure pointer to pointer is real

    if (*ptr != NULL)
    {
        free(*ptr);

        *ptr = NULL;                    // Make sure we don't use this again
    }
}
