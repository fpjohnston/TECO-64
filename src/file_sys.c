///
///  @file    file_sys.c
///  @brief   System-dependent file-handling functions.
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
#include <glob.h>                       // for glob()
#include <libgen.h>                     // for dirname()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>                   // for stat()
#include <unistd.h>                     // for access()

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "file.h"


#define TEMP_NAME   "_teco_XXXXXX"      ///< Template for temp file name
#define TEMP_TYPE   ".tmp"              ///< Temp file type/extension

#define SIZE_NAME   (sizeof(TEMP_NAME) - 1) ///< Size of temp file name
#define SIZE_TYPE   (sizeof(TEMP_TYPE) - 1) ///< Size of temp file type

static glob_t pglob;                    ///< Saved list of wildcard files

static char **next_file;                ///< Next file in pglob


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

                throw(E_SYS, filename);// Unexpected system error
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
///            output, because that might supercede and truncate an existing
///            file. So if a file exists, we create a temporary name to use for
///            the actual open, and then when we close the output stream, we
///            will delete (or, if a backup copy was requested, rename) the
///            original file, and then rename the temporary file. This allows
///            for the situation where the user decides to kill the output file
///            with an EK command, in which we can simply close and delete the
///            temporary file and leave the original intact.
///
///            This function is system-dependent because some operating environ-
///            may have alternative methods of dealing with output files that
///            may need to be deleted, such as versioning on VMS.
///
///  @returns  File pointer to temp file.
///
////////////////////////////////////////////////////////////////////////////////

FILE *open_temp(char **otemp, const char *oname)
{
    assert(otemp != NULL);
    assert(*otemp == NULL);
    assert(oname != NULL);

    struct stat file_stat;

    if (stat(oname, &file_stat) != 0)
    {
        throw(E_SYS, oname);            // Unexpected system call
    }

    uint nbytes = (uint)strlen(oname);
    char outfile[nbytes + 1];           // Copy of output file name

    strcpy(outfile, oname);

    char *dir = dirname(outfile);       // Extract the directory/path
    char tempfile[strlen(dir) + 1 + SIZE_NAME + SIZE_TYPE + 1];

    nbytes = (uint)snprintf(tempfile, (ulong)sizeof(tempfile), "%s/%s%s", dir,
                            TEMP_NAME, TEMP_TYPE);

    int fd = mkstemps(tempfile, (int)SIZE_TYPE);

    if (fd == -1)
    {
        throw(E_SYS, tempfile);         // Unexpected system error
    }

    *otemp = alloc_mem(nbytes + 1);

    strcpy(*otemp, tempfile);

    return fdopen(fd, "w+");
}


///
///  @brief    Read file specification from memory file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void read_memory(char *p, uint len)
{
    assert(p != NULL);

    const char *memory = getenv("TECO_MEMORY");

    if (memory != NULL)
    {
        FILE *fp = fopen(memory, "r");

        if (fp == NULL)
        {
            if (errno != ENOENT && errno != ENODEV)
            {
                printf("%%Can't open memory file '%s'\r\n", memory);
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
    assert(ofile != NULL);

    if (ofile->temp != NULL)
    {
        if (ofile->backup)
        {
            char scratch[strlen(ofile->name) + 1 + 1];

            snprintf(scratch, sizeof(scratch), "%s~", ofile->name);

            if (rename(ofile->name, scratch) != 0)
            {
                throw(E_SYS, ofile->name);// Unexpected system error
            }
        }
        else if (remove(ofile->name) != 0)
        {
            throw(E_SYS, ofile->name);// Unexpected system error
        }

        if (rename(ofile->temp, ofile->name) != 0)
        {
            throw(E_SYS, ofile->name);// Unexpected system error
        }
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
            throw(E_MEM);          // Ran out of memory

        case GLOB_NOMATCH:
            return false;               // No matches

        default:                        // Something unexpected
            throw(E_SYS, filename);// Unexpected system error
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
    assert(file != NULL);

    const char *memory = getenv("TECO_MEMORY");
    FILE *fp;

    if (memory == NULL)
    {
        return;
    }

    if ((fp = fopen(memory, "w")) == NULL)
    {
        printf("%%Can't open memory file '%s'\r\n", memory);

        return;
    }

    fprintf(fp, "%s\n", file);

    fclose(fp);

}
