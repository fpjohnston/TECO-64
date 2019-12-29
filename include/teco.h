///
///  @file    teco.h
///  @brief   Main header file for TECO text editor.
///
///  @bug     No known bugs.
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

#define EXPR_SIZE            64         ///< Size of expression stack

#define B                    (0)        ///< Beginning of text buffer

enum
{
    TECO_FAILURE = 0,                   ///< TECO function failure
    TECO_SUCCESS = -1                   ///< TECO function success
};

#define DUMMY_VALUE     1               ///< Temporary dummy value

///  @enum   expr_type
///  @brief  Type of item stored on expression stack.

enum expr_type
{
    EXPR_VALUE = '0',
    EXPR_NOT   = '\x1F',                // ^^ (CTRL/^) = ASCII RS
    EXPR_OR    = '#',
    EXPR_AND   = '&',
    EXPR_LEFT  = '(',
    EXPR_RIGHT = ')',
    EXPR_MUL   = '*',
    EXPR_PLUS  = '+',
    EXPR_MINUS = '-',
    EXPR_DIV   = '/'
};

///  @struct e_obj
///  @brief  Definition of objects on expression stack.

struct e_obj
{
    long value;                     ///< Operand value (if type = EXPR_VALUE)
    enum expr_type type;            ///< Value type (operand/operator)
};

///  @struct estack
///  @brief  Definition of expression stack.

struct estack
{
    uint level;                     ///< Expression stack level
    struct e_obj obj[EXPR_SIZE];    ///< Expression stack objects
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
    uint get;                       ///< Index of next character to fetch
    uint put;                       ///< Index of next character to store
};

///  @struct tstring
///  @brief  Definition of TECO-string, which is a counted (not NUL-terminated)
///          string.

struct tstring
{
    char *buf;                      ///< Start of string
    uint len;                       ///< No. of characters
};

///  @struct  ifile
///  @brief   Definition of variables used to keep track of input files.

struct ifile
{
    FILE *fp;                       ///< Input file stream
    bool eof;                       ///< End of file reached
    bool cr;                        ///< Last character was CR
};

///  @enum    itype
///  @brief   Definition of input file stream types.

enum itype
{
    IFILE_PRIMARY,                  ///< Primary input stream
    IFILE_SECONDARY,                ///< Secondary input stream
    IFILE_INDIRECT,                 ///< EI command stream
    IFILE_MAX                       ///< Maximum input files
};

///  @struct  ofile
///  @brief   Definition of variables used to keep track of output files.

struct ofile
{
    FILE *fp;                       ///< Output file stream
    char *name;                     ///< Output file name
    char *temp;                     ///< Temporary file name
    bool backup;                    ///< File is opened for backup
};

///  @enum    otype
///  @brief   Definition of output file stream types.

enum otype
{
    OFILE_PRIMARY,                  ///< Primary output stream
    OFILE_SECONDARY,                ///< Secondary output stream
    OFILE_INDIRECT,                 ///< E%q command stream
    OFILE_LOG,                      ///< EL log file
    OFILE_MAX                       ///< Maximum output files
};

///  @struct  vars
///  @brief   Global variables that the user can display or set.

struct vars
{
    int radix;                      ///< Current output radix
    int ctrl_x;                     ///< CTRL/X flag
    bool ff;                        ///< Form feed flag
    bool trace;                     ///< true if trace mode is on
    bool warn;                      ///< true if edit buffer is almost full
    bool full;                      ///< true if edit buffer is full
};

// Global variables

extern struct vars v;

extern int teco_version;

extern jmp_buf jump_main;

extern const char *prompt;

extern uint rows;

extern uint cols;

extern bool teco_debug;

extern char *filename_buf;

extern char *eg_command;

extern struct estack estack;        ///< Expression stack

extern struct ifile ifiles[];

extern struct ofile ofiles[];

extern uint istream, ostream;

extern char *last_file;

extern int last_in;

extern const char *mung_file;

extern struct buffer *cmd_buf;      ///< Current command string

// Global functions

extern void *alloc_mem(uint size);

extern void *expand_mem(void *ptr, uint oldsize, uint newsize);

extern void free_mem(void *ptr);

extern void *shrink_mem(void *ptr, uint oldsize, uint newsize);

extern void exec_cmd(void);

extern void init_env(int argc, const char * const argv[]);

extern void set_config(int argc, const char * const argv[]);

extern int teco_env(int n);

// Terminal input/output functions

extern void echo_chr(int c);

extern int getc_term(bool nowait);

extern void init_term(void);

extern void print_prompt(void);

extern void print_term(const char *str);

extern void put_bell(void);

extern void putc_term(int c);

extern void puts_term(const char *str, unsigned int nbytes);

extern void read_cmd(void);

// Functions that access the expression stack

extern void init_expr(void);

extern bool pop_expr(int *operand);

extern void push_expr(int c, enum expr_type type);

// Buffer functions

extern struct buffer *copy_buf(void);

extern int delete_buf(void);

extern void echo_buf(int pos);

extern bool empty_buf(void);

extern int fetch_buf(void);

extern struct buffer *get_buf(void);

extern void init_buf(void);

extern char *next_buf(void);

extern void reset_buf(void);

extern void set_buf(struct buffer *buf);

extern uint start_buf(void);

extern void store_buf(int c);

extern void unfetch_buf(int c);

// File functions

extern void close_output(uint stream);

extern void create_filename(const struct tstring *text);

extern const char *get_oname(struct ofile *ofile, uint nbytes, bool exists);

extern int get_wild(void);

extern void init_files(void);

extern int open_input(const char *filespec, uint stream);

extern void rename_output(struct ofile *ofile);

extern void set_wild(const char *filename);

#endif  // !defined(_TECO_H)
