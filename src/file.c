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

const char *last_file;                  ///< Last opened file

// Local functions

static void canonical_name(char **name);

static void reset_files(void);


///
///  @brief    Make file name canonical.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void canonical_name(char **name)
{
    assert(name != NULL);
    assert(*name != NULL);

    char path[PATH_MAX];

    // This function is only called after a successful fopen() (for either
    // reading or writing), so the call to realpath() shouldn't fail because
    // a non-existent file.

    if (realpath(*name, path) == NULL)  // Get absolute path for file name
    {
        print_err(E_SYS);
    }

    free_mem(name);                     // Deallocate previous file name

    last_file = *name = alloc_mem((uint)strlen(path) + 1);

    strcpy(*name, path);
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

    free_mem(&ifile->name);

    ifile->name = NULL;
    ifile->eof  = false;
    ifile->cr   = false;
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

    // If we're here after an EB command, then the output name points to the
    // same allocated string as the input name.

    if (!ofile->backup)
    {
        free_mem(&ofile->name);
    }

    free_mem(&ofile->temp);

    ofile->name   = NULL;
    ofile->temp   = NULL;
    ofile->backup = false;
}

///
///  @brief    Initialize file streams.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_files(void)
{
    reset_files();

    if (atexit(reset_files) != 0)
    {
        exit(EXIT_FAILURE);
    }
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

    // First, normalize the file name by removing all non-graphic chrs.

    char path[len + 1], *p = path;
    uint ncopied = 0;

    while (len-- > 0)
    {
        if (isgraph(*buf))
        {
            *p++ = *buf++;
            ++ncopied;
        }
        else
        {
            ++buf;
        }
    }

    *p = NUL;

    if (*name != NULL)                  // Deallocate any existing name
    {
        free_mem(name);
    }

    last_file = *name = alloc_mem(ncopied + 1);
    strcpy(*name, path);
}


///
///  @brief    Open file for input.
///
///  @returns  EXIT_SUCCESS or EXIT_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////

int open_input(struct ifile *ifile)
{
    assert(ifile != NULL);
    assert(ifile->name != NULL);

    if ((ifile->fp = fopen(ifile->name, "r")) == NULL)
    {
        return EXIT_FAILURE;
    }

    canonical_name(&ifile->name);

    ifile->eof = false;
    ifile->cr  = false;

    return EXIT_SUCCESS;
}


///
///  @brief    Open file for output.
///
///  @returns  EXIT_SUCCESS or EXIT_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////

int open_output(struct ofile *ofile, int c)
{
    assert(ofile != NULL);
    assert(ofile->name != NULL);

    const char *oname = ofile->name;

    c = toupper(c);

    if (ofile->fp != NULL)
    {
        if (c != 'L')                   // Can't reopen if not EL command
        {
            print_err(E_OFO);           // Output file is already open
        }

        close_output(OFILE_LOG);        // Re-opening a log file is okay
    }
    else
    {
        free_mem(&ofile->temp);
    }

    if (access(oname, F_OK) == 0)       // Does file already exist?
    {
        if (c != 'L')                   // Create temp file unless EL command
        {
            init_temp(&ofile->temp, oname);

            oname = ofile->temp;
        }

        if (c == 'W')                   // Issue warnings if EW command
        {
            print_str("%s", "%Superseding existing file\r\n");
        }
    }

    if ((ofile->fp = fopen(oname, "w")) == NULL)
    {
        return EXIT_FAILURE;
    }

    canonical_name(&ofile->name);

    if (c == 'L')                       // Immediately flush output to log file
    {
        (void)setvbuf(ofile->fp, NULL, _IONBF, 0uL);
    }

    return EXIT_SUCCESS;
}


///
///  @brief    Reset file streams.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void reset_files(void)
{
    for (uint i = 0; i < OFILE_MAX; ++i)
    {
        close_output(i);
    }

    ostream = OFILE_PRIMARY;

    for (uint i = 0; i < IFILE_MAX; ++i)
    {
        close_input(i);
    }

    istream = IFILE_PRIMARY;

    last_file = "";
}
