///
///  @file    file.c
///  @brief   File-handling functions.
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

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "file.h"
#include "term.h"


struct ifile ifiles[IFILE_MAX];         ///< Input file descriptors

struct ofile ofiles[OFILE_MAX];         ///< Output file descriptors

uint istream = IFILE_PRIMARY;           ///< Current input stream

uint ostream = OFILE_PRIMARY;           ///< Current output stream

char *last_file = NULL;                 ///< Last opened file

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
    assert(name != NULL);
    assert(*name != NULL);

    char path[PATH_MAX];

    // This function is only called after a successful fopen() (for either
    // reading or writing), so the call to realpath() shouldn't fail because
    // a non-existent file.

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
    assert(name != NULL);
    assert(buf != NULL);
    assert(len != 0);

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
            printc_err(E_IFN, c);
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
///  @brief    Open file for input.
///
///  @returns  Input file stream if success, NULL if failure. Note that a NULL
///            can only be returned if colon is set.
///
////////////////////////////////////////////////////////////////////////////////

struct ifile *open_input(const char *name, uint len, uint stream, int colon,
                         int c)
{
    assert(name != NULL);

    close_input(stream);                // Close input file if open

    struct ifile *ifile = &ifiles[stream];

    init_filename(&ifile->name, name, len);

    if (!canonical_name(&ifile->name) ||
        ((ifile->fp = fopen(ifile->name, "r")) == NULL))
    {
        if ((colon && (errno == ENOENT || errno == ENODEV)) || c == NUL)
        {
            return NULL;
        }
        
        prints_err(E_INP, ifile->name); // Input file error
    }

    ifile->eof = false;
    ifile->cr  = false;

    return ifile;
}


///
///  @brief    Open log file.
///
///  @returns  true if success, false if failure.
///
////////////////////////////////////////////////////////////////////////////////

bool open_log(struct ofile *ofile)
{
    assert(ofile != NULL);
    assert(ofile->name != NULL);

    if ((ofile->fp = fopen(ofile->name, "a")) == NULL)
    {
        return false;
    }

    (void)canonical_name(&ofile->name);

    // Write log output immediately and do not buffer.

    (void)setvbuf(ofile->fp, NULL, _IONBF, 0uL);

    return true;
}


///
///  @brief    Open file for output.
///
///  @returns  Output file stream if success, NULL if failure. Note that a NULL
///            can only be returned if the colon flag was set.
///
////////////////////////////////////////////////////////////////////////////////

struct ofile *open_output(const char *name, uint len, uint stream, bool colon,
                          int c)
{
    assert(name != NULL);
    assert(c == 'B' || c == 'W' || c == 'Z');

    struct ofile *ofile = &ofiles[stream];

    if (ofile->fp != NULL)
    {
        print_err(E_OFO);               // Output file is already open
    }

    init_filename(&ofile->name, name, len);

    // If the output file already exists, then we create a temporary file and
    // open that instead. That allows us to make off any changes in the event
    // of an EK command. If the file is closed normally, we will rename the
    // original, and then rename the temp file.

    const char *oname = ofile->name;

    if (access(oname, F_OK) == 0)       // Does file already exist?
    {
        init_temp(&ofile->temp, oname);

        oname = ofile->temp;

        if (c == 'W')                   // Issue warnings if EW command
        {
            print_str("%s", "%Superseding existing file\r\n");
        }
    }

    if ((ofile->fp = fopen(oname, "w")) == NULL)
    {
        if (colon)
        {
            return NULL;
        }

        prints_err(E_OUT, ofile->name);
    }

    (void)canonical_name(&ofile->name);

    if (c == 'B' || c == 'W')           // Save file name for EB or EW
    {
        write_memory(ofile->name);
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
    assert(name != NULL);

    free_mem(&last_file);

    // Make copy of name of last file opened.

    last_file = alloc_mem((uint)strlen(name) + 1);

    strcpy(last_file, name);
}
