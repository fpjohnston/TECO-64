///
///  @file    file_sys.c
///  @brief   System-dependent file-handling functions.
///
///  @copyright 2019-2022 Franklin P. Johnston / Nowwith Treble Software
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
#include <glob.h>                       // for glob()
#include <libgen.h>                     // for dirname()
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>                     // for access()

#include <sys/stat.h>                   // for stat()

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errcodes.h"
#include "file.h"
#include "term.h"


#define TEC_TYPE    ".tec"              ///< Command file extension ("source")
#define TCO_TYPE    ".tco"              ///< Command file extension ("compiled")

static glob_t pglob;                    ///< Saved list of wildcard files

static char **next_file;                ///< Next file in pglob

// Local functions

static struct ifile *find_file(const char *name, uint stream, const char *type);

static uint_t parse_file(const char *file, char *dir, char *base);


///
///  @brief    Try to open command file; if failure, then try again with TECO
///            file type (.tec).
///
///  @returns  File pointer, or NULL if all opens failed and colon modifier was
///            present.
///
////////////////////////////////////////////////////////////////////////////////

struct ifile *find_command(const char *name, uint stream, bool colon)
{
    assert(name != NULL);

    struct ifile *ifile;

    if ((ifile = find_file(name, stream, "")) != NULL
        || (ifile = find_file(name, stream, TCO_TYPE)) != NULL
        || (ifile = find_file(name, stream, TEC_TYPE)) != NULL)
    {
        return ifile;
    }

    // Here if open failed. See if we should return error status.

    if (colon)
    {
        return NULL;
    }

    // If failure, issue error using original file name (plus explicit or
    // implicit file type) provided by user.

    throw(E_FNF, name);                 // File not found
}


///
///  @brief    Open command file in current directory; if not found, then try
///            library directory.
///
///  @returns  File pointer, or NULL if all opens failed and colon modifier was
///            present.
///
////////////////////////////////////////////////////////////////////////////////

static struct ifile *find_file(const char *name, uint stream, const char *type)
{
    assert(name != NULL);
    assert(type != NULL);

    // Split the file spec into a directory name and a base name

    size_t len = strlen(name);
    char dir[len + 1];
    char base[len + 1];
    char file[strlen(name) + strlen(type) + 1];
    struct ifile *ifile;

    (void)parse_file(name, dir, base);

    if (strchr(base, '.'))              // Is there a file type/extension?
    {
        type = "";                      // If so, don't use default type
    }

#if     defined(NDEBUG)
    snprintf(file, sizeof(file), "%s%s", name, type);
#else
    int nbytes = snprintf(file, sizeof(file), "%s%s", name, type);
#endif

    assert(nbytes > 0);
    assert((size_t)(uint)nbytes < sizeof(file));

    if ((ifile = open_input(file, stream, (bool)true)) != NULL)
    {
        return ifile;                   // Open succeeded, so we're done
    }

    // If we have a relative path and a library directory, then try again.

    if (dir[0] != '/' && teco_library != NULL)
    {
        char libfile[strlen(teco_library) + 1 + strlen(file) + 1];

#if     defined(NDEBUG)
        snprintf(libfile, sizeof(libfile), "%s/%s", teco_library, file);
#else
        nbytes = snprintf(libfile, sizeof(libfile), "%s/%s", teco_library, file);
#endif

        assert(nbytes > 0);
        assert((size_t)(uint)nbytes < sizeof(libfile));

        if ((ifile = open_input(libfile, stream, (bool)true)) != NULL)
        {
            return ifile;               // Open succeeded, so we're done
        }
    }

    return NULL;
}


///
///  @brief    Get next filename matching wildcard specification.
///
///  @returns  EXIT_SUCCESS if found another match, else EXIT_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////

int get_wild(void)
{
    if (next_file != NULL)
    {
        // Loop through the remaining file specifications, skipping anything
        // that's not a regular file (we can't open directories, for example).

        const char *filename;

        while ((filename = *next_file++) != NULL)
        {
            struct stat file_stat;

            if (stat(filename, &file_stat) != 0)
            {
                next_file = NULL;       // Make sure we can't repeat this

                throw(E_ERR, filename); // General error
            }

            if (S_ISREG(file_stat.st_mode))
            {
                set_last(filename);

                return EXIT_SUCCESS;
            }
        }

        next_file = NULL;               // Say we're all done
    }

    return EXIT_FAILURE;
}


///
///  @brief    Open temp file name. We are passed the output file name the
///            user specified, but we can't use it if we are opening it for
///            output, because that might supersede and truncate an existing
///            file. So if a file exists, we create a temporary name to use for
///            the actual open, and then when we close the output stream, we
///            will delete (or, if a backup copy was requested, rename) the
///            original file, and then rename the temporary file. This allows
///            for the situation where the user decides to kill the output file
///            with an EK command, in which we can simply close and delete the
///            temporary file and leave the original intact.
///
///            This function is system-dependent because some operating environ-
///            ments have alternative methods of dealing with output files that
///            may need to be deleted, such as versioning on VMS.
///
///  @returns  File pointer to temp file.
///
////////////////////////////////////////////////////////////////////////////////

FILE *open_temp(const char *oname, uint stream)
{
    assert(oname != NULL);              // Error if NULL output file

    struct ofile *ofile = &ofiles[stream];
    struct stat statbuf;

    if (stat(oname, &statbuf) != 0)
    {
        throw(E_ERR, oname);            // General error
    }

    char dir[strlen(oname) + 1];

    (void)parse_file(oname, dir, NULL);

    char tempfile[PATH_MAX];
    int nbytes = snprintf(tempfile, sizeof(tempfile), "%s%s", dir, "_teco_XXXXXX");
    int fd = mkstemp(tempfile);

    if (fd == -1)
    {
        throw(E_ERR, tempfile);         // General error
    }

    fchmod(fd, statbuf.st_mode);  // Use same permissions as old file

    ofile->temp = alloc_mem((uint_t)(uint)nbytes + 1);

    strcpy(ofile->temp, tempfile);

    return fdopen(fd, "w+");
}


///
///  @brief    Parse file name, separating the device/directory from the
///            name/type.
///
///  @returns  Total bytes for directory and name.
///
////////////////////////////////////////////////////////////////////////////////

static uint_t parse_file(const char *file, char *dir, char *base)
{
    assert(file != NULL);
    assert(base != NULL || dir != NULL);

    const char *slash = strrchr(file, '/');
    int nbytes = 0;

    // Split file name into directory and base name. We don't use dirname() or
    // basename() here, since we don't like how they handle corner cases.

    if (slash == NULL)
    {
        if (file[0] == NUL || !strcmp(file, "..") || !strcmp(file, "."))
        {
            throw(E_FNF, file);         // File not found
        }
        else
        {
            if (base != NULL)
            {
                nbytes = sprintf(base, "%s", file);
            }

            if (dir != NULL)
            {
                dir[0] = NUL;
            }
        }
    }
    else                                // We found a slash
    {
        if (slash[1] == NUL)
        {
            throw(E_FNF, file);         // File not found
        }

        if (base != NULL)
        {
            nbytes = sprintf(base, "%s", slash + 1);
        }

        if (dir != NULL)
        {
            nbytes += sprintf(dir, "%.*s", (int)(slash + 1 - file), file);
        }
    }

    return (uint_t)(uint)nbytes;
}


///
///  @brief    Read file specification from memory file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void read_memory(char *p, uint len)
{
    assert(p != NULL);                  // Error if no place to store data

    if (teco_memory != NULL)
    {
        FILE *fp = fopen(teco_memory, "r");

        if (fp == NULL)
        {
            if (errno != ENOENT && errno != ENODEV)
            {
                tprint("%%Can't open memory file '%s'\n", teco_memory);
            }
        }
        else
        {
            int c;

            while (--len > 0 && (c = fgetc(fp)) != EOF && isgraph(c))
            {
                *p++ = (char)c;
            }

            fclose(fp);
        }
    }

    *p = NUL;
}


///
///  @brief    Rename output file. This is system-dependent, because on Linux
///            we use a temporary name when opening the file, and we will need
///            to delete the original file and then rename the temporary file.
///            If a backup was requested, we will just rename the original file
///            instead of deleting it.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void rename_output(struct ofile *ofile)
{
    assert(ofile != NULL);              // Error if no output file

    if (ofile->temp == NULL)            // Nothing to do if no temp. file
    {
        return;
    }

    if (ofile->backup)
    {
        char saved_name[strlen(ofile->name) + 1 + 1];

        snprintf(saved_name, sizeof(saved_name), "%s~", ofile->name);

        // Rename old file

        if (rename(ofile->name, saved_name) != 0)
        {
            throw(E_ERR, ofile->name);  // General error
        }
    }
    else if (remove(ofile->name) != 0)
    {
        throw(E_ERR, ofile->name);      // General error
    }

    // Rename temp. file name to actual name

    if (rename(ofile->temp, ofile->name) != 0)
    {
        throw(E_ERR, ofile->name);      // General error
    }
}


///
///  @brief    Set wildcard filename buffer.
///
///  @returns  true if we found a match, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool set_wild(const char *filename)
{
    int flags;

#if     defined(__GNUC__)

    flags = GLOB_ERR | GLOB_MARK | GLOB_TILDE; //lint !e835

#else

    flags = GLOB_ERR | GLOB_MARK;       //lint !e835

#endif

    switch (glob(filename, flags, NULL, &pglob))
    {
        case 0:                         // Success
            next_file = &pglob.gl_pathv[0];

            return true;

        case GLOB_NOSPACE:
            throw(E_MEM);               // Ran out of memory

        case GLOB_NOMATCH:
            return false;               // No matches

        default:                        // Something unexpected
            throw(E_ERR, filename);// General error
    }
}


///
///  @brief    Write EB or EW file to memory file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void write_memory(const char *file)
{
    assert(file != NULL);               // Error if no output file

    FILE *fp;

    if (teco_memory == NULL)
    {
        return;
    }

    if ((fp = fopen(teco_memory, "w")) == NULL)
    {
        tprint("%%Can't open memory file '%s'\n", teco_memory);

        return;
    }

    fprintf(fp, "%s\n", file);

    fclose(fp);

}
