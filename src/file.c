///
///  @file    file.c
///  @brief   File-handling functions.
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

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "estack.h"
#include "file.h"
#include "term.h"


struct ifile ifiles[IFILE_MAX];         ///< Input file descriptors

struct ofile ofiles[OFILE_MAX];         ///< Output file descriptors

uint istream = IFILE_PRIMARY;           ///< Current input stream

uint ostream = OFILE_PRIMARY;           ///< Current output stream

char *last_file = NULL;                 ///< Last opened file

#define ERR_FILE_SIZE   200             ///< Max. len. of file causing error

static char err_file[ERR_FILE_SIZE + 1]; ///< Name of file causing error

// Local functions

static bool canonical_name(char **name);

static void exit_files(void);


///
///  @brief    Make file name canonical.
///
///  @returns  true if name was successfully translated, else false (which may
///            mean that the file does not exist).
///
////////////////////////////////////////////////////////////////////////////////

static bool canonical_name(char **name)
{
    assert(name != NULL);               // Error if no file name
    assert(*name != NULL);              // Error if file name is a null string

    char path[PATH_MAX];

    if (realpath(*name, path) == NULL)  // Get absolute path for file name
    {
        set_last(*name);

        return false;
    }

    free_mem(name);                     // Deallocate previous file name

    *name = alloc_mem((uint)strlen(path) + 1);

    strcpy(*name, path);

    set_last(*name);

    return true;
}


///
///  @brief    Close input file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void close_input(uint stream)
{
    struct ifile *ifile = &ifiles[stream];

    if (ifile->fp != NULL)
    {
        fclose(ifile->fp);

        ifile->fp = NULL;
    }

    ifile->eof  = false;
    ifile->cr   = false;

    free_mem(&ifile->name);
}


///
///  @brief    Close input file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void close_output(uint stream)
{
    struct ofile *ofile = &ofiles[stream];

    if (ofile->fp != NULL)
    {
        fclose(ofile->fp);

        ofile->fp = NULL;
    }

    free_mem(&ofile->name);
    free_mem(&ofile->temp);

    ofile->name   = NULL;
    ofile->temp   = NULL;
    ofile->backup = false;
}


///
///  @brief    Clean up memory before we exit from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exit_files(void)
{
    for (uint i = 0; i < OFILE_MAX; ++i)
    {
        close_output(i);
    }

    ostream = OFILE_PRIMARY;

    for (uint i = 0; i < IFILE_MAX; ++i)
    {
        close_input(i);

        struct ifile *ifile = &ifiles[i];

        free_mem(&ifile->name);

        ifile->name = NULL;
    }

    istream = IFILE_PRIMARY;

    free_mem(&last_file);
}


///
///  @brief    Initialize file streams.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_files(void)
{
    register_exit(exit_files);

    istream = IFILE_PRIMARY;
    ostream = OFILE_PRIMARY;
}


///
///  @brief    Create a file name specification in file name buffer. We copy
///            from the specified text string, skipping any characters, such as
///            spaces or control characters, that aren't normal graphic char-
///            acters. We also ensure that the file name ends with a NUL, since
///            that's the required format for library calls such as fopen().
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_filename(char **name, const char *buf, uint len)
{
    assert(name != NULL);               // Error if no output file name
    assert(buf != NULL);                // Error if no input file name
    assert(len != 0);                   // Error if input name is a null string

    // Check the file name for invalid characters. Currently, this just means
    // ignoring whitespace, and issuing an error for any other control or
    // non-ASCII characters. Other characters could be checked, depending on
    // what is valid for a specfic operating system.

    char path[len + 1], *p = path;
    uint ncopied = 0;

    while (len-- > 0)
    {
        int c = *buf++;

        if (isspace(c))
        {
            ;
        }
        else if (iscntrl(c) || !isascii(c))
        {
            throw(E_IFN, c);            // Illegal character in file name
        }
        else
        {
            *p++ = (char)c;
            ++ncopied;
        }
    }

    *p = NUL;

    if (*name != NULL)                  // Deallocate any existing name
    {
        free_mem(name);
    }

    // The following allows the use of string building characters in filenames.

    (void)build_string(name, path, ncopied);
}


///
///  @brief    Open indirect command file which may have an implicit .tec file
///            type/extension. We try to open the file as specified, but if
///            that fails, we append .tec to the name (assuming there was no
///            period in the name), and try again.
///
///  @returns  true if file opened and it has data, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool open_command(const char *buf, uint len, uint stream, bool colon,
                  const char *libdir, struct buffer *text)
{
    if (len == 0)                       // Any file to open?
    {
        return false;
    }

    assert(buf != NULL);                // Error if no input file name
    assert(text != NULL);               // Error if no text buffer

    char name[len + 1];                 // NUL-terminated file name

    (void)snprintf(name, sizeof(name), "%.*s", (int)len, buf);

    struct ifile *ifile = find_command(name, len, stream, colon, libdir);

    // If there was no colon, then we've already thrown an exception.
    // So if ifile is NULL here, then a colon must have been present.

    if (ifile == NULL)
    {
        push_expr(0, EXPR_VALUE);

        return false;
    }
    else if (colon)
    {
        push_expr(-1, EXPR_VALUE);
    }

    struct stat file_stat;

    if (stat(last_file, &file_stat) != 0)
    {
        close_input(stream);

        throw(E_SYS, last_file);       // Unexpected system error
    }

    uint size = (uint)file_stat.st_size;

    free_mem(&text->data);             // Free any previous storage

    // If there's data in the file, then allocate a buffer for it.

    if (size != 0)
    {
        text->len   = size;
        text->pos   = 0;
        text->size += size;            // Caller may have preset this
        text->data  = alloc_mem(text->size);

        if (fread(text->data, 1uL, (ulong)size, ifile->fp) != size)
        {
            throw(E_SYS, ifile->name); // Unexpected system error
        }
    }

    close_input(stream);

    return (size != 0) ? true : false;
}


///
///  @brief    Open file for input.
///
///  @returns  Input file stream if success, NULL if failure and a colon is set.
///
////////////////////////////////////////////////////////////////////////////////

struct ifile *open_input(const char *name, uint len, uint stream, bool colon)
{
    assert(name != NULL);               // Error if no input file name

    close_input(stream);                // Close input file if open

    struct ifile *ifile = &ifiles[stream];

    init_filename(&ifile->name, name, len);

    struct stat file_stat;

    if (stat(ifile->name, &file_stat) != 0)
    {
        if (colon)
        {
            return NULL;
        }

        throw(E_FNF, ifile->name);      // File not found
    }

    if (!S_ISREG(file_stat.st_mode))
    {
        throw(E_FIL, ifile->name);      // Illegal file
    }

    if (access(ifile->name, R_OK) != 0) // Is file readable?
    {
        errno = EPERM;

        throw(E_SYS, ifile->name);      // System error
    }

    if (!canonical_name(&ifile->name) ||
        ((ifile->fp = fopen(ifile->name, "r")) == NULL))
    {
        throw(E_SYS, ifile->name);      // Unexpected system error
    }

    ifile->eof = false;
    ifile->cr  = false;

    return ifile;
}


///
///  @brief    Open file for output. We are called to handle opens for EB, EL,
///            EW, and EZ commands.
///
///  @returns  Output file stream if success, NULL if failure. Note that a NULL
///            can only be returned if the colon flag was set.
///
////////////////////////////////////////////////////////////////////////////////

struct ofile *open_output(const char *name, uint len, uint stream, bool colon,
                          int c)
{
    assert(name != NULL);               // Error if no output file name
    assert(strchr("BLWZ", c) != NULL);  // Error if unexpected command

    struct ofile *ofile = &ofiles[stream];

    // Note: EL will always close its file before calling us.

    if (ofile->fp != NULL)
    {
        throw(E_OFO);                   // Output file is already open
    }

    init_filename(&ofile->name, name, len);

    // If the output file already exists, then we create a temporary file and
    // open that instead. That allows us to make off any changes in the event
    // of an EK command. If the file is closed normally, we will rename the
    // original, and then rename the temp file.

    if (c == 'L')
    {
        ofile->fp = fopen(ofile->name, "a+");
    }
    else if (access(ofile->name, F_OK) == 0) // Does file already exist?
    {
        if (access(ofile->name, W_OK) != 0) // Yes, but is it writeable?
        {
            snprintf(err_file, sizeof(err_file), "%.*s", ERR_FILE_SIZE,
                     ofile->name);

            close_output(ostream);      // Deallocate stream resources

            throw(E_SYS, err_file);     // Unexpected system error
        }

        ofile->fp = open_temp(&ofile->temp, ofile->name);

        if (c == 'W')                   // Issue warnings if EW command
        {
            print_str("%s", "%Superseding existing file\r\n");
        }
    }
    else
    {
        ofile->fp = fopen(ofile->name, "w+");
    }      

    if (ofile->fp == NULL || !canonical_name(&ofile->name))
    {
        if (colon)
        {
            return NULL;
        }

        snprintf(err_file, sizeof(err_file), "%.*s", ERR_FILE_SIZE,
                 ofile->name);

        close_output(ostream);          // Deallocate stream resources

        throw(E_SYS, err_file);         // Unexpected system error
    }

    if (c == 'B' || c == 'W')           // Save file name for EB or EW
    {
        write_memory(ofile->name);
    }
    else if (c == 'L')
    {
        // Write log output immediately and do not buffer.

        (void)setvbuf(ofile->fp, NULL, _IONBF, 0uL);
    }

    return ofile;
}


///
///  @brief    Save name of last file opened.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_last(const char *name)
{
    assert(name != NULL);               // Error if no file name

    free_mem(&last_file);

    // Make copy of name of last file opened.

    last_file = alloc_mem((uint)strlen(name) + 1);

    strcpy(last_file, name);
}
