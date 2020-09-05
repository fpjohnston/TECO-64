///
///  @file    file.h
///  @brief   Header file for TECO file functions.
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

#if     !defined(_FILE_H)

#define _FILE_H

#include <stdio.h>              //lint !e451 !e537
#include <stdbool.h>            //lint !e451 !e537
#include <sys/types.h>          //lint !e451 !e537


///  @struct  ifile
///  @brief   Definition of variables used to keep track of input files.

struct ifile
{
    FILE *fp;                       ///< Input file stream
    char *name;                     ///< Input file name
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

// Global variables

extern struct ifile ifiles[];

extern struct ofile ofiles[];

extern uint istream;

extern uint ostream;

extern char *last_file;

// File functions

extern void close_input(uint stream);

extern void close_output(uint stream);

extern struct ifile *find_command(char *base, uint len, uint stream,
                                  bool colon);

extern int get_wild(void);

extern void init_filename(char **name, const char *buf, uint len);

extern void init_files(void);

extern bool open_command(const char *buf, uint len, uint stream, bool colon,
                         struct buffer *text);

extern struct ifile *open_input(const char *name, uint len, uint stream,
                                bool colon);

extern struct ofile *open_output(const char *name, uint len, uint stream,
                                 bool colon, int c);

extern FILE *open_temp(char **temp, const char *oname);

extern void read_memory(char *p, uint len);

extern void rename_output(struct ofile *ofile);

extern void set_last(const char *name);

extern bool set_wild(const char *filename);

extern void write_memory(const char *file);

#endif  // !defined(_FILE_H)
