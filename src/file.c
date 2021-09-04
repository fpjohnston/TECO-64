///
///  @file    file.c
///  @brief   File-handling functions.
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
#include "editbuf.h"
#include "errcodes.h"
#include "file.h"
#include "term.h"


struct ifile ifiles[IFILE_MAX];         ///< Input file descriptors

struct ofile ofiles[OFILE_MAX];         ///< Output file descriptors

uint istream = IFILE_PRIMARY;           ///< Current input stream

uint ostream = OFILE_PRIMARY;           ///< Current output stream

char last_file[PATH_MAX] = { NUL };     ///< Last opened file

// Local functions

static char *make_canonical(const char *name);


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

    ifile->cr = false;

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

void exit_files(void)
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
}


///
///  @brief    Create a file name specification in file name buffer. We copy
///            from the specified text string, skipping any characters such as
///            spaces or control characters, that aren't normal graphic char-
///            acters. We also ensure that the file name ends with a NUL, since
///            that's the required format for library calls such as fopen().
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

char *init_filename(const char *name, uint_t len, bool colon)
{
    assert(name != NULL);               // Error if no file name

    // The caller passed us a counted string, not a C string, so copy it to
    // a temp. buffer, add a NUL, and then change our source pointer.

    char temp[len + 1];

    memcpy(temp, name, (size_t)len);

    temp[len] = NUL;

    name= temp;

    uint_t skip = (uint_t)strspn(name, " "); // Skip leading spaces

    assert(skip < len);                 // Error if no file name

    name += skip;
    len -= skip;

    // Filenames must be C strings and can't have embedded NUL characters.

    if (memchr(name, NUL, (size_t)len) != NULL)
    {
        throw(E_IFN, NUL);              // Illegal file name
    }

    const char *home = getenv("HOME");
    int nbytes = (int)len;
    char *dst;

    // If filename has an initial "~" (tilde), replace it with the user's home
    // directory. This is the only change we make that increases the length of
    // the file specification, other than the call to build a string.

    if (name[0] == '~' && home != NULL) // Need to add home directory?
    {
        nbytes = snprintf(scratch, sizeof(scratch), "%s%.*s", home,
                          nbytes - 1, name + 1);
    }
    else
    {
        nbytes = snprintf(scratch, sizeof(scratch), "%.*s", nbytes, name);
    }

    assert(nbytes > 0);
    assert(nbytes < (int)sizeof(scratch));

    name = scratch;

    // Next, scan for match control characters.

    tstring string = build_string(name, (uint_t)(uint)nbytes);

    name = dst = string.data;

    // Ensure that there are no non-graphic ASCII characters in the filespec.

    for (uint_t i = 0; i < string.len; ++i)
    {
        int c = *name++;

        if (!isgraph(c))
        {
            throw(E_IFN, c);            // Invalid character in file name
        }

        *dst++ = (char)c;
    }

    *dst = NUL;

    len = (uint_t)(uint)(dst - scratch);

    if (len == 0)
    {
        if (colon)
        {
            return NULL;
        }

        throw(E_FNF, scratch);
    }

    return scratch;
}


///
///  @brief    Make file name canonical.
///
///  @returns  true if name was successfully translated, else false (which may
///            mean that the file does not exist).
///
////////////////////////////////////////////////////////////////////////////////

static char *make_canonical(const char *name)
{
    assert(name != NULL);               // Error if no file name

    char path[PATH_MAX];

    if (realpath(name, path) == NULL)   // Get resolved path for file name
    {
        set_last(name);                 // Set the unresolved name

        return NULL;
    }
    else
    {
        strcpy(scratch, path);          // Copy resolved name to scratch buffer

        set_last(scratch);              // Set the resolved name

        return scratch;
    }
}


///
///  @brief    Open indirect command file which may have an implicit .tec file
///            type/extension. We try to open the file as specified, but if
///            that fails, we append .tec to the name (assuming there was no
///            period in the name), and try again.
///
///  @returns  true if file opened, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool open_command(const char *name, uint stream, bool colon, tbuffer *text)
{
    assert(name != NULL);               // Error if no input file name
    assert(text != NULL);               // Error if no edit buffer

    struct ifile *ifile = find_command(name, stream, colon);

    // If there was no colon, then we've already thrown an exception.
    // Therefore, if ifile is NULL, a colon must have been present.

    if (ifile == NULL)
    {
        return false;
    }

    struct stat file_stat;

    if (stat(last_file, &file_stat) != 0)
    {
        close_input(stream);

        throw(E_ERR, last_file);        // General error
    }

    size_t size = (size_t)file_stat.st_size;

    // If there's data in the file, then allocate a buffer for it.

    if ((text->len = text->size = (uint_t)size) != 0)
    {
        char tempbuf[size];

        if (fread(tempbuf, 1uL, size, ifile->fp) != size)
        {
            close_input(stream);

            throw(E_ERR, ifile->name);  // General error
        }

        text->pos  = 0;
        text->data = alloc_mem((uint_t)size);

        memcpy(text->data, tempbuf, size);
    }

    close_input(stream);

    return true;
}


///
///  @brief    Open file for input.
///
///  @returns  Input file stream if success, NULL if failure. Note that a NULL
///            can only be returned if the colon flag was set.
///
////////////////////////////////////////////////////////////////////////////////

struct ifile *open_input(const char *name, uint stream, bool colon)
{
    assert(name != NULL);               // Error if no input file name

    close_input(stream);                // Close input file if open

    // Make canonical form of file name (w/ absolute path)

    if ((name = make_canonical(name)) == NULL)
    {
        return NULL;
    }

    // Check to see that the file exists

    struct stat file_stat;

    if (stat(name, &file_stat) != 0)
    {
        if (colon)
        {
            return NULL;
        }

        throw(E_FNF, name);             // File not found
    }

    // Check that file spec is a regular file

    if (!S_ISREG(file_stat.st_mode))
    {
        throw(E_FIL, name);             // Invalid file
    }

    // Check that the file is readable

    if (access(name, R_OK) != 0)        // Is file readable?
    {
        errno = EPERM;

        throw(E_ERR, name);             // General error
    }

    struct ifile *ifile = &ifiles[stream];

    // Now actually open the file

    if ((ifile->fp = fopen(name, "r")) == NULL)
    {
        throw(E_ERR, name);             // General error
    }

    ifile->size = (uint_t)file_stat.st_size;
    ifile->cr   = false;
    ifile->name = alloc_mem((uint_t)strlen(name) + 1);

    strcpy(ifile->name, name);

    if (stream == IFILE_PRIMARY || stream == IFILE_SECONDARY)
    {
        write_memory(ifile->name);
    }

    return ifile;
}


///
///  @brief    Open file for output. We are called to handle opens for EB, EL,
///            EW, and E% commands.
///
///  @returns  Output file stream if success, NULL if failure. Note that a NULL
///            can only be returned if the colon flag was set.
///
////////////////////////////////////////////////////////////////////////////////

struct ofile *open_output(const char *name, uint stream, bool colon, int c)
{
    assert(name != NULL);               // Error if no output file name
    assert(strchr("BLW%", c) != NULL);  // Ensure it's EB, EL, EW, or E%

    struct ofile *ofile = &ofiles[stream];

    // Note: EL will always close its file before calling us.

    if (ofile->fp != NULL)
    {
        throw(E_OFO);                   // Output file is already open
    }

    if (c == 'L')                       // EL command?
    {
        ofile->fp = fopen(name, "w+");  // Yes, always open for write
    }
    else if (access(name, F_OK) != 0)   // Does file already exist?
    {
        ofile->fp = fopen(name, "w+");  // No, so no temp file needed
    }
    else if (access(name, W_OK) != 0)   // File exists - is it writeable?
    {
        throw(E_ERR, name);             // General error
    }
    else
    {
        // Here if file exists and is writeable. Create a temporary file and
        // open that instead. That allows us to make off any changes in the
        // event of an EK command. If the file is closed normally, we will
        // rename the original, and then rename the temp file.

        ofile->fp = open_temp(name, stream);

        if (c == 'W')                   // Issue warning if EW command
        {
            tprint("%%Superseding existing file '%s'\n", name);
        }
    }

    // Here after we've tried to open the output (or temp) file

    if (ofile->fp == NULL)
    {
        if (colon)
        {
            return NULL;
        }

        throw(E_ERR, name);             // General error
    }

    // Here when we've either just opened a new file that didn't previously
    // exist, or we've held off on opening an existing file (in case the
    // user decides to cancel the edit). Regardless of which case, we can
    // now get the canonical form of the name (with absolute path instead
    // of any relative path), and use that to create a memory file.

    if ((name = make_canonical(name)) == NULL)
    {
        return NULL;                    // Can't get canonical name?
    }

    ofile->name = alloc_mem((uint_t)strlen(name) + 1);

    strcpy(ofile->name, name);

    if (c == 'B' || c == 'W')           // Save file name for EB or EW
    {
        write_memory(ofile->name);

        ofile->backup = true;           //  and say we want a backup file
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

    size_t size = sizeof(last_file);
    int nbytes = snprintf(last_file, size, "%s", name);

    if (nbytes < 0 || nbytes >= (int)size)
    {
        throw(E_MAX);                   // Internal program limit reached
    }

    last_len = (uint_t)(uint)nbytes;
}
