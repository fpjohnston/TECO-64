///
///  @file    teco.h
///  @brief   Main header file for TECO text editor.
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


/// @def    countof(array)
/// @brief  Returns the number of elements in an array.

#define countof(array) (sizeof(array) / sizeof(array[0]))

#define WAIT                 true       ///< Wait for terminal input

#define STR_SIZE_INIT        1024       ///< Initial string size

#define EXPR_SIZE            64         ///< Size of expression stack

///  @enum   expr_type
///  @brief  Type of item stored on expression stack.

enum expr_type
{
    EXPR_VALUE = '0',
    EXPR_NOT   = '\x1F',
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
    long value;                     // Operand/operator value
    enum expr_type type;            // Value type (operand/operator)
};

///  @struct estack
///  @brief  Definition of expression stack.

struct estack
{
    uint level;                     ///< Expression stack level
    struct e_obj obj[EXPR_SIZE];    ///< Expression stack objects
};

///  @struct   buffer
///  @brief    Definition of general buffer. This is dynamically allocated, and
///            can be resized as necessary. It consists of a pointer to a memory
///            block, the size of the block in bytes, and two counters, one for
///            storing new characters, and one for removing characters as the
///            buffer is read.

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

///  @struct  qreg
///  @brief   Definition of Q-register storage, which includes a string and a
///           numeric value.

struct qreg
{
    struct buffer text;             ///< Q-register text
    int n;                          ///< Q-register numeric value
};

///  @def     QREG_SIZE
///  @brief   No. of Q-registers in each set.

#define QREG_SIZE       (('9' - '0') + 1 + ('Z' - 'A') + 1)

///  @enum    file_status
///  @brief   Returned file status from functions opening files.

enum file_status
{
    OPEN_SUCCESS = -1,
    OPEN_FAILURE =  0
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
    OFILE_MAX                       ///< Maximum output files
};

///  @enum    backup_flag
///  @brief   Definition of flag that specifies whether or not a file is
///           opened for backup.

enum backup_flag
{
    NOBACKUP_FILE,                  ///< Not opening a file for backup
    BACKUP_FILE                     ///< Opening a file for backup
};

///  @struct  vars
///  @brief   Global variables that the user can display or set.

struct vars
{
    int b;                          ///< Beginning of buffer (always 0)
    int z;                          ///< End of buffer (no. of chrs. in buffer)
    int dot;                        ///< Current pointer position in buffer
    int eof;                        ///< End of file flag
    int ff;                         ///< Form feed flag
    int radix;                      ///< Current output radix
    int ctrl_x;                     ///< CTRL/X flag
    bool trace;                     ///< true if trace mode is on
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

extern struct estack estack;            ///< Expression stack

extern struct ifile ifiles[];

extern struct ofile ofiles[];

extern uint istream, ostream;

extern char *last_file;

extern int last_in;

// Global functions

extern void *alloc_more(void *ptr, uint oldsize, uint newsize);

extern void *alloc_new(uint size);

extern void dealloc(char **ptr);

extern void exec_cmd(void);

extern void init_env(void);

extern int teco_env(int n);

// Terminal input/output functions

extern void echo_chr(int c);

extern void exit_EG(void);

extern int getc_term(bool nowait);

extern bool help_command(void);

extern void init_EG(void);
 
extern void init_term(void);

extern void put_bell(void);

extern void putc_term(int c);

extern void puts_term(const char *str, unsigned int nbytes);

extern void print_prompt(void);

extern void print_term(const char *str);

extern void read_cmd(void);

// Q-register functions

extern void append_qchr(int qname, bool qdot, int c);

extern void append_qtext(int qname, bool qdot, struct tstring text);

extern uint get_qall(void);

extern int get_qchr(int qname, bool qdot, int n);

extern int get_qnum(int qname, bool qdot);

extern uint get_qsize(int qname, bool qdot);

extern void init_qreg(void);

extern bool pop_qreg(int qname, bool qdot);

extern void print_qreg(int qname, bool qdot);

extern bool push_qreg(int qname, bool qdot);

extern void store_qtext(int qname, bool qdot, struct tstring text);

extern void store_qchr(int qname, bool qdot, int c);

extern void store_qnum(int qname, bool qdot, int n);

// Functions that access the expression stack

extern int get_n_arg(void);

extern void init_expr(void);

extern bool operand_expr(void);

extern void push_expr(int c, enum expr_type type);

extern bool valid_radix(int c);

// Buffer functions

extern struct tstring copy_buf(void);

extern uint count_buf(void);

extern int delete_buf(void);

extern void echo_buf(int pos);

extern bool empty_buf(void);

extern int fetch_buf(void);

extern void init_buf(void);

extern bool match_buf(const char *str);

extern char *next_buf(void);

extern void reset_buf(void);

extern uint start_buf(void);

extern void store_buf(int c);

extern void unfetch_buf(int c);

// File functions

extern void close_indirect(void);

extern void create_filename(const struct tstring *text);

extern const char *get_oname(struct ofile *ofile, uint nbytes);

extern int get_wild(void);

extern void init_files(void);

extern int open_input(const char *filespec, uint stream);

extern int open_output(const struct tstring *text, int backup);

extern bool read_indirect(void);

extern void rename_output(struct ofile *ofile);

extern void set_wild(const char *filename);

#endif  // !defined(_TECO_H)
