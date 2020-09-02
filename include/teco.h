///
///  @file    teco.h
///  @brief   Main header file for TECO-64 text editor.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
////////////////////////////////////////////////////////////////////////////////

#if     !defined(_TECO_H)

#define _TECO_H

#include <setjmp.h>             //lint !e451 !e537
#include <stdbool.h>            //lint !e451 !e537
#include <stdio.h>              //lint !e451 !e537
#include <sys/types.h>          //lint !e451 !e537


/// @def    countof(array)
/// @brief  Returns the number of elements in an array.

#define countof(array) (sizeof(array) / sizeof(array[0]))

#define WAIT                 true       ///< Wait for terminal input

#define STR_SIZE_INIT        1024       ///< Initial string size

#if     defined(__clang__)

typedef unsigned long ulong;

#endif

///  @struct   buffer
///  @brief    Definition of general buffer, used both for the main command
///            string and for Q-register macros. This is dynamically allocated,
///            and can be resized as necessary. It consists of a pointer to a
///            memory block, the size of the block in bytes, and two counters,
///            one for storing new characters, and one for removing characters
///            as the buffer is read.

struct buffer
{
    char *data;                     ///< Buffer data
    uint size;                      ///< Total size of buffer in bytes
    uint len;                       ///< Current length stored
    uint pos;                       ///< Position of next character
};

///  @struct tstring
///  @brief  Definition of a TECO string, which is a counted string (not a
///          NUL-terminated string, as used in languages such as C).

struct tstring
{
    char *data;                     ///< String data
    uint len;                       ///< No. of characters
};


// Global variables

extern char eg_command[];

extern uint if_depth;

extern jmp_buf jump_main;

extern uint last_len;

extern uint loop_depth;

extern int radix;

extern int teco_version;

extern const char *teco_init;

extern const char *teco_library;

extern const char *teco_memory;

extern const char *teco_prompt;

extern const char *teco_vtedit;

// General-purpose common functions

extern void *alloc_mem(uint size);

extern uint build_string(char **dest, const char *src, uint len);

extern bool check_loop(void);

extern bool check_macro(void);

extern void *expand_mem(void *ptr, uint oldsize, uint newsize);

extern void flag_print(int flag);

extern void free_mem(void *ptr);

extern void init_env(int argc, const char * const argv[]);

extern void init_mem(void);

extern int isdelim(int c);

extern void register_exit(void (*func)(void));

extern void reset(void);

extern void set_config(int argc, const char * const argv[]);

extern void *shrink_mem(void *ptr, uint oldsize, uint newsize);

extern int teco_env(int n, bool colon);

void tprintf(const char *format, ...);

#endif  // !defined(_TECO_H)
