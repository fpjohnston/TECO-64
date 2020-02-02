///
///  @file    teco.h
///  @brief   Main header file for TECO text editor.
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

extern void print_chr(int c);

extern void print_echo(int c);

extern void print_str(const char *fmt, ...);


/// @def    countof(array)
/// @brief  Returns the number of elements in an array.

#define countof(array) (sizeof(array) / sizeof(array[0]))

#define WAIT                 true       ///< Wait for terminal input

#define STR_SIZE_INIT        1024       ///< Initial string size

#define EXPR_SIZE            64         ///< Size of expression stack

enum
{
    TECO_FAILURE = 0,                   ///< TECO function failure
    TECO_SUCCESS = -1                   ///< TECO function success
};

///  @enum   oper_type
///  @brief  Type of operator stored on expression stack

enum oper_type
{
    TYPE_GROUP = 1,                 // Group command (,),{,}
    TYPE_OPER  = 2                  // Operator command
};

///  @enum   expr_type
///  @brief  Type of item stored on expression stack.

enum expr_type
{
    EXPR_NONE   = 0,
    EXPR_VALUE  = '0',              // General value
    EXPR_LPAREN = '(',              // Left parenthesis
    EXPR_RPAREN = ')',              // Right parenthesis
    EXPR_LBRACE = '{',              // Left brace
    EXPR_RBRACE = '}',              // Right brace
    EXPR_COMP   = '\x1F',           // 1's complement
    EXPR_OR     = '#',              // Bitwise OR
    EXPR_AND    = '&',              // Bitwise AND
    EXPR_XOR    = '~',              // Bitwise XOR
    EXPR_MUL    = '*',              // Multiplication
    EXPR_PLUS   = '+',              // Addition and unary plus
    EXPR_MINUS  = '-',              // Subtraction and unary minus
    EXPR_DIV    = '/',              // Division w/ quotient
    EXPR_REM    = '%',              // Division w/ remainder (//)
    EXPR_NOT    = '!',              // Logical NOT
    EXPR_LT     = '<',              // Less than
    EXPR_GT     = '>',              // Greater than
    EXPR_EQ     = '=',              // Equals (==)
    EXPR_NE     = 'N',              // Not equals (<>)
    EXPR_LE     = 'L',              // Less than or equal (<=)
    EXPR_GE     = 'G',              // Greater than or equal (>=)
    EXPR_LEFT   = 'S',              // Left shift (<<)
    EXPR_RIGHT  = 'U'               // Right shift (>>)
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
    uint len;                       ///< Current length stored
    uint pos;                       ///< Position of next character
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
    IFILE_QREGISTER,                ///< EQ input stream
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
    bool backup;                    ///< File is open for backup
};

///  @enum    otype
///  @brief   Definition of output file stream types.

enum otype
{
    OFILE_PRIMARY,                  ///< Primary output stream
    OFILE_SECONDARY,                ///< Secondary output stream
    OFILE_QREGISTER,                ///< E%q command stream
    OFILE_LOG,                      ///< EL log file
    OFILE_MAX                       ///< Maximum output files
};

///  @struct  vars
///  @brief   Global variables that the user can display or set.

struct vars
{
    int radix;                      ///< Current output radix
    int ctrl_s;                     ///< CTRL/S flag
    bool ff;                        ///< Form feed flag
    bool trace;                     ///< true if trace mode is on
    bool warn;                      ///< true if edit buffer is almost full
    bool full;                      ///< true if edit buffer is full
};

#define CMD_START       (bool)true  ///< Read character at start of command
#define NOCMD_START     (bool)false ///< Read character in middle of command

///  @enum   search_type
///  @brief  Type of search requested.

enum search_type
{
    SEARCH_S = 1,                   ///< Local search within page
    SEARCH_N = 2,                   ///< Non-stop search
    SEARCH_U = 3,                   ///< Search with yank protection
    SEARCH_E = 4                    ///< Search w/o yank protection
};

///  @struct  search
///  @brief   Common block used by all search functions.

struct search
{
    enum search_type type;              ///< Search type
    bool (*search)(struct search *s);   ///< Search function
    int count;                          ///< No. of iterations for search
    int text_start;                     ///< Start search at this position
    int text_end;                       ///< End search at this position
    int text_pos;                       ///< Position of string relative to dot
    uint match_len;                     ///< No. of characters left to match
    const char *match_buf;              ///< Next character to match
};

// Global variables

extern struct vars v;

extern int teco_version;

extern jmp_buf jump_main;

extern const char *prompt;

extern char *filename_buf;

extern char *eg_command;

extern struct estack estack;        ///< Expression stack

extern struct ifile ifiles[];

extern struct ofile ofiles[];

extern uint istream, ostream;

extern char *last_file;

extern const char *log_file;

extern const char *mung_file;

extern struct buffer *cmdbuf;

extern struct watch w;

extern uint macro_depth;

extern struct tstring last_search;

extern uint last_len;

// Global functions

extern void *alloc_mem(uint size);

extern uint build_string(char **dest, const char *src, uint len);

extern bool check_loop(void);

extern bool check_macro(void);

extern void *expand_mem(void *ptr, uint oldsize, uint newsize);

extern void free_mem(void *ptr);

extern void exec_cmd(void);

extern void init_env(int argc, const char * const argv[]);

extern void init_search(void);

extern int isdelim(int c);

extern bool search_loop(struct search *s);

extern void search_print(void);

extern bool search_backward(struct search *s);

extern bool search_forward(struct search *s);

extern int teco_env(int n);

extern void set_config(int argc, const char * const argv[]);

extern void *shrink_mem(void *ptr, uint oldsize, uint newsize);

// Terminal input/output functions

extern void echo_in(int c);

extern void echo_out(int c);

extern int getc_term(bool nowait);

extern void init_term(void);

extern void print_prompt(void);

extern void put_bell(void);

extern void read_cmd(void);

extern void reset_term(void);

// Functions that access the expression stack

extern void init_expr(void);

extern bool pop_expr(int *operand);

extern void push_expr(int c, enum expr_type type);

// Buffer functions

extern struct buffer *copy_buf(void);

extern int delete_buf(void);

extern void echo_buf(int pos);

extern bool empty_buf(void);

extern int fetch_buf(bool start);

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

extern int open_output(struct ofile *ofile, int c, const char *name, uint nbytes);

extern void rename_output(struct ofile *ofile);

extern void set_wild(const char *filename);

#endif  // !defined(_TECO_H)
