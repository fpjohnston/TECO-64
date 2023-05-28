///
///  @file    teco.h
///  @brief   Main header file for TECO-64 text editor.
///
///  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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

#include <limits.h>                 //lint !e451
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>


//
//  Define the int_t and uint_t types that we will subsequently use whenever we
//  require an integral type that can vary depending on the environment we've
//  been compiled for. There are currently three possibilities: 16, 32, and 64
//  bits, with 32 being the default. Others could perhaps be added, but not
//  willy-nilly, which is why we abort with a preprocessor error if we see
//  something strange.
//
//  Note that since the maximum value in a 16-bit environment is only 64 K,
//  there is no reason to define the MB and GB macros.
//

#if     !defined(INT_T)

#define INT_T 32                    ///< Default integer size is 32 bits

#endif

#if     INT_T == 64

typedef long int_t;                 ///< Size of m and n arguments
typedef size_t uint_t;              ///< Use largest possible unsigned int

#define KB  (1024uL)                ///< No. of bytes in a kilobyte
#define MB  (KB * KB)               ///< No. of bytes in a megabyte
#define GB  (MB * KB)               ///< No. of bytes in a gigabyte

#elif   INT_T == 32

typedef int int_t;                  ///< Size of m and n arguments
typedef unsigned int uint_t;        ///< Use regular unsigned int

#define KB  (1024u)                 ///< No. of bytes in a kilobyte
#define MB  (KB * KB)               ///< No. of bytes in a megabyte
#define GB  (MB * KB)               ///< No. of bytes in a gigabyte

// The following is reserved for a possible future implementation
// of TECO for a 16-bit environment.

//#elif   INT_T == 16

//typedef short int_t;                ///< Size of m and n arguments
//typedef unsigned short uint_t;      ///< Use regular unsigned int

//#define KB  (1024u)                 ///< No. of bytes in a kilobyte

#else

#error Invalid integer size: expected 32 or 64

#endif

/// @def    countof(array)
/// @brief  Returns the number of elements in an array.

#define countof(array) (sizeof(array) / sizeof(array[0]))

typedef unsigned char uchar;            ///< Unsigned character shortcut

//
//  The following may be redundant if the compiler already defines 'ulong', but
//  as long as the definitions are identical, it shouldn't cause an error.
//

//lint -save -e761

typedef unsigned long ulong;            ///< Unsigned long shortcut

//lint -restore

/// @def    isdelim(c)
/// @brief  Check character to see if it's a line delimiter.

#define isdelim(c)  (c == LF || c == VT || c == FF)


#define SUCCESS         ((int_t)-1)     ///< Command succeeded
#define FAILURE         ((int_t)0)      ///< Command failed

enum
{
    MAIN_NORMAL,                        ///< Normal main loop entry
    MAIN_ERROR,                         ///< Error entry
    MAIN_CTRLC                          ///< CTRL/C or abort entry
};


///  @struct   tbuffer
///  @brief    Definition of general buffer, used both for the main command
///            string and for Q-register macros. This is dynamically allocated,
///            and can be resized as necessary. It consists of a pointer to a
///            memory block, the size of the block in bytes, and two counters,
///            one for storing new characters, and one for removing characters
///            as the buffer is read.

struct tbuffer
{
    char *data;                         ///< Buffer data
    uint_t size;                        ///< Total size of buffer in bytes
    uint_t len;                         ///< Current length stored
    uint_t pos;                         ///< Position of next character
};

typedef struct tbuffer tbuffer;         ///< TECO buffer


///  @struct tstring
///  @brief  Definition of a TECO string, which is a counted string (not a
///          NUL-terminated string, as used in languages such as C).

struct tstring
{
    char *data;                     ///< String data
    uint_t len;                     ///< No. of characters
};

typedef struct tstring tstring;     ///< TECO string


///  @struct cmd
///  @brief  Command block structure.

struct cmd
{
    char c1;                        ///< 1st command character
    char c2;                        ///< 2nd command character
    char c3;                        ///< 3rd command character
    char qname;                     ///< Q-register name
    bool qlocal;                    ///< Q-register is local
    int qindex;                     ///< Q-register index (if not -1)
    bool m_set;                     ///< m argument is valid (comma seen)
    int_t m_arg;                    ///< m argument
    bool n_set;                     ///< n argument is valid
    int_t n_arg;                    ///< n argument
    bool h;                         ///< H found
    bool colon;                     ///< : found
    bool dcolon;                    ///< :: found
    bool atsign;                    ///< @ found
    tstring text1;                  ///< 1st text string
    tstring text2;                  ///< 2nd text string
};


#define MAX_LOOPS   (32)            ///< Maximum nesting level for loops

///  @struct  loop
///  @brief   Variables we need to keep track of for each loop level.

struct loop
{
    int_t iter;                     ///< Iteration count for loop
    uint_t pos;                     ///< Starting position of loop
    uint_t line;                    ///< Line number of loop start
    uint depth;                     ///< Depth of if statements
};

///  @struct  ctrl
///  @brief   Command control structure.

struct ctrl
{
    uint depth;                     ///< Conditional nesting depth
    uint level;                     ///< Current loop level
    struct loop loop[MAX_LOOPS];    ///< Nested loop array
};

///  @struct  ifile
///  @brief   Definition of variables used to keep track of input files.

struct ifile
{
    FILE *fp;                       ///< Input file stream
    char *name;                     ///< Input file name
    uint_t size;                    ///< Input file size
    bool LF;                        ///< First LF has been read
};

///  @struct  ofile
///  @brief   Definition of variables used to keep track of output files.

struct ofile
{
    FILE *fp;                       ///< Output file stream
    char *name;                     ///< Output file name
    char *temp;                     ///< Temporary file name
    bool backup;                    ///< File is open for backup
};


// Global variables

extern uint_t cmd_line;

extern jmp_buf jump_main;

extern uint_t last_len;

extern char scratch[PATH_MAX];

extern const char *teco_init;

extern const char *teco_library;

extern const char *teco_memory;

extern const char *teco_prompt;

extern const char *teco_vtedit;

// General-purpose common functions

extern void *alloc_mem(uint_t size);

extern tbuffer alloc_tbuf(uint_t size);

extern tstring build_string(const char *src, uint_t len);

extern const char *build_trimmed(const char *src, uint_t len);

extern bool check_loop(void);

extern bool check_macro(void);

extern void detach_term(void);

extern void exit_cbuf(void);

extern void exit_edit(void);

extern void exit_EG(void);

extern void exit_error(void);

extern void exit_files(void);

extern void exit_map(void);

extern void exit_mem(void);

extern void exit_qreg(void);

extern void exit_tbuf(void);

extern void exit_term(void);

extern void *expand_mem(void *p1, uint_t size, uint_t delta);

extern void free_mem(void *ptr);

extern uint getloop_base(void);

extern uint getloop_depth(void);

extern uint_t getloop_start(void);

extern void init_env(void);

extern void init_options(int argc, const char * const argv[]);

extern void print_flag(int_t flag);

extern void print_size(uint_t size);

extern void reset_map(void);

extern void setloop_depth(uint depth);

extern void setloop_base(uint base);

extern void *shrink_mem(void *p1, uint_t size, uint_t delta);

extern int teco_env(int n, bool colon);

extern int tprint(const char *format, ...);

#endif  // !defined(_TECO_H)
