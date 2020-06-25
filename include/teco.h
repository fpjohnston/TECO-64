///
///  @file    teco.h
///  @brief   Main header file for TECO-64 text editor.
///
///  @bug     No known bugs.
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
////////////////////////////////////////////////////////////////////////////////

#if     !defined(_TECO_H)
#define _TECO_H

#if     !defined(_SETJMP_H)
#include <setjmp.h>
#endif

#if     !defined(_STDBOOL_H)
#include <stdbool.h>
#endif

#if     !defined(_STDIO_H)
#include <stdio.h>
#endif

#if     !defined(_SYS_TYPES_H)
#include <sys/types.h>
#endif

/// @def    countof(array)
/// @brief  Returns the number of elements in an array.

#define countof(array) (sizeof(array) / sizeof(array[0]))

#define WAIT                 true       ///< Wait for terminal input

#define STR_SIZE_INIT        1024       ///< Initial string size

#if     defined(__clang__)

typedef unsigned long ulong;

#endif

enum
{
    TECO_FAILURE = 0,                   ///< TECO function failure
    TECO_SUCCESS = -1                   ///< TECO function success
};

///  @struct   buffer
///  @brief    Definition of general buffer, used both for the main command
///            string and for Q-register macros. This is dynamically allocated,
///            and can be resized as necessary. It consists of a pointer to a
///            memory block, the size of the block in bytes, and two counters,
///            one for storing new characters, and one for removing characters
///            as the buffer is read.

struct buffer
{
    char *buf;                      ///< Start of buffer
    uint size;                      ///< Total size of buffer in bytes
    uint len;                       ///< Current length stored
    uint pos;                       ///< Position of next character
};

///  @struct tstring
///  @brief  Definition of a TECO string, which is a counted string (not a
///          NUL-terminated string, as used in languages such as C).

struct tstring
{
    char *buf;                      ///< Start of string
    uint len;                       ///< No. of characters
};


// Global variables

extern int radix;

extern struct buffer *term_buf;

extern uint if_depth;

extern uint loop_depth;

extern struct buffer *current;

extern char eg_command[];

extern jmp_buf jump_main;

extern uint last_len;

extern bool main_active;

extern const char *prompt;

extern int teco_version;

// Global functions

extern void *alloc_mem(uint size);

extern uint build_string(char **dest, const char *src, uint len);

extern bool check_loop(void);

extern bool check_macro(void);

extern void exec_cmd(void);

extern void *expand_mem(void *ptr, uint oldsize, uint newsize);

extern void free_mem(void *ptr);

extern void init_env(int argc, const char * const argv[]);

extern void init_mem(void);

extern int isdelim(int c);

extern void flag_print(int flag);

extern void set_config(int argc, const char * const argv[]);

extern void *shrink_mem(void *ptr, uint oldsize, uint newsize);

extern int teco_env(int n, bool colon);

// Command buffer functions

extern int fetch_cbuf(bool start);

extern struct buffer *get_cbuf(void);

extern void init_cbuf(void);

extern char *next_cbuf(void);

extern void register_exit(void (*func)(void));

extern void reset_cbuf(void);

extern void set_cbuf(struct buffer *buf);

extern void store_cbuf(int c);

extern void unfetch_cbuf(int c);

#endif  // !defined(_TECO_H)
