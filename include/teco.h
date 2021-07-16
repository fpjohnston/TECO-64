///
///  @file    teco.h
///  @brief   Main header file for TECO-64 text editor.
///
///  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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


#if     defined(__DECC)

#define noreturn

#else

#include <stdnoreturn.h>

#endif

#if     defined(LONG_64)

typedef long int_t;               ///< Size of m and n arguments

#else

typedef int int_t;                ///< Size of m and n arguments

#endif

/// @def    countof(array)
/// @brief  Returns the number of elements in an array.

#define countof(array) (sizeof(array) / sizeof(array[0]))

#define WAIT             true       ///< Wait for terminal input

#define STR_SIZE_INIT    1024       ///< Initial string size

typedef unsigned char uchar;        ///< Unsigned character type

#if     defined(__clang__)

typedef unsigned long ulong;

#endif


/// @def    isdelim(c)
/// @brief  Check character to see if it's a line delimiter.

#define isdelim(c)  (c == LF || c == VT || c == FF)


#define SUCCESS         ((int_t)-1) ///< Command succeeded
#define FAILURE         ((int_t)0)  ///< Command failed

enum
{
    MAIN_NORMAL,                    ///< Normal main loop entry
    MAIN_ERROR,                     ///< Error entry
    MAIN_CTRLC                      ///< CTRL/C or abort entry
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

extern uint if_depth;

extern jmp_buf jump_main;

extern uint last_len;

extern uint loop_depth;

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

extern void exec_options(int argc, const char * const argv[]);

extern void exit_cbuf(void);

#if     defined(DISPLAY_MODE)

extern void exit_dpy(void);

#endif

extern void exit_ebuf(void);

extern void exit_EG(void);

extern void exit_error(void);

extern void exit_files(void);

extern void exit_map(void);

#if     defined(MEMCHECK)

extern void exit_mem(void);

#endif

extern void exit_qreg(void);

extern void exit_search(void);

extern void exit_tbuf(void);

extern void exit_term(void);

extern void *expand_mem(void *ptr, uint oldsize, uint newsize);

extern void free_mem(void *ptr);

extern void init_env(void);

extern void init_options(int argc, const char * const argv[]);

extern void print_flag(int flag);

extern void *shrink_mem(void *ptr, uint oldsize, uint newsize);

extern char *strdup_mem(const char *ptr);

extern int teco_env(int n, bool colon);

#endif  // !defined(_TECO_H)
