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
#include "eflags.h"
#include "errors.h"

#define TECO_VERSION    200             ///< Our version of TECO

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
    init_qreg();                        // Initialize Q-registers
    init_files();                       // Initialize file streams

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


///
///  @brief    Get more memory.
///
///  @returns  Nothing (error if memory allocation fails).
///
////////////////////////////////////////////////////////////////////////////////

void *alloc_more(void *ptr, uint oldsize, uint newsize)
{
    assert(ptr != NULL);
    assert(oldsize != newsize);
    assert(oldsize < newsize);

    void *newptr = realloc(ptr, (size_t)newsize);

    if (newptr == NULL)
    {
        print_err(E_MEM);               // Memory overflow
    }

    return newptr;
}


///
///  @brief    Get new memory.
///
///  @returns  Nothing (error if memory allocation fails).
///
////////////////////////////////////////////////////////////////////////////////

void *alloc_new(uint size)
{
    void *ptr = calloc(1uL, (size_t)size);

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
