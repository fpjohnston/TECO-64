///
///  @file    files.c
///  @brief   File-handling functions.
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

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"

struct ifile ifiles[IFILE_MAX];

struct ofile ofiles[OFILE_MAX];

uint istream = IFILE_PRIMARY;

uint ostream = OFILE_PRIMARY;

char *last_file = NULL;

char scratch[512];


///
///  @brief    Close out file streams.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void free_files(void)
{
    FILE *fp;

    for (int i = 0; i < IFILE_MAX; ++i)
    {
        if ((fp = ifiles[i].fp) != NULL)
        {
            fclose(fp);
        }

        ifiles[i].fp  = NULL;
        ifiles[i].eof = false;
        ifiles[i].cr  = false;
    }

    istream = IFILE_PRIMARY;

    for (int i = 0; i < OFILE_MAX; ++i)
    {
        if ((fp = ofiles[i].fp) != NULL)
        {
            fclose(fp);
        }

        ofiles[i].fp = NULL;
        ofiles[i].backup = false;

        dealloc(&ofiles[i].name);
        dealloc(&ofiles[i].temp);
    }

    ostream = OFILE_PRIMARY;

    dealloc(&last_file);
}


///
///  @brief    Initialize file streams.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_files(void)
{
    if (atexit(free_files) != 0)
    {
        exit(EXIT_FAILURE);
    }
         
    for (int i = 0; i < IFILE_MAX; ++i)
    {
        ifiles[i].fp  = NULL;
        ifiles[i].eof = false;
        ifiles[i].cr  = false;
    }

    istream = IFILE_PRIMARY;

    for (int i = 0; i < OFILE_MAX; ++i)
    {
        ofiles[i].fp     = NULL;
        ofiles[i].name   = NULL;
        ofiles[i].temp   = NULL;
        ofiles[i].backup = false;
    }

    ostream = OFILE_PRIMARY;

    last_file = NULL;
}


///
///  @brief    Open file for input.
///
///  @returns  EXIT_SUCCESS or EXIT_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////

int open_input(struct tstr *text)
{
    assert(text != NULL);
    
    struct ifile *ifile = &ifiles[istream]; // Current input stream
    FILE *fp = ifile->fp;             

    if (fp != NULL)                     // Stream already open?
    {
        fclose(fp);                     // Yes, so close it

        fp = NULL;
    }

    dealloc(&last_file);

    last_file = alloc_new(text->len + 1);

    sprintf(last_file, "%.*s", text->len, text->buf);

    if ((fp = fopen(last_file, "r")) == NULL)
    {
        return EXIT_FAILURE;
    }

    ifiles[istream].fp  = fp;
    ifiles[istream].eof = false;
    ifiles[istream].cr  = false;

    return EXIT_SUCCESS;
}


///
///  @brief    Open file for output.
///
///  @returns  EXIT_SUCCESS or EXIT_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////

int open_output(struct tstr *text, bool backup)
{
    assert(text != NULL);

    struct ofile *ofile = &ofiles[ostream];
    const char *oname;                  // Output file name
    struct stat file_stat;
    FILE *fp;
    uint nbytes = text->len;             // Length of text string
    
    dealloc(&ofile->name);
    dealloc(&ofile->temp);
    dealloc(&last_file);

    last_file = alloc_new(nbytes + 1);

    sprintf(last_file, "%.*s", nbytes, text->buf);

    // TODO: move system-dependent code elsewhere.

    if (access(last_file, F_OK) == 0)   // Does file already exist?
    {
        if (stat(last_file, &file_stat) != 0)
        {
            fatal_err(errno, E_SYS, NULL);
        }

        ofile->name = alloc_new(nbytes + 1);

        memcpy(ofile->name, last_file, nbytes + 1);
        memcpy(scratch, last_file, nbytes + 1);
    
        char *dir = dirname(scratch);
        char tempfile[] = "teco-XXXXXX";
        int fd = mkstemp(tempfile);

        close(fd);

        nbytes = strlen(dir) + 1 + strlen(tempfile);

        ofile->temp = alloc_new(nbytes + 1);

        sprintf(ofile->temp, "%s/%s", dir, tempfile);

        oname = ofile->temp;
    }
    else                                // File does not exist
    {
        ofile->name = alloc_new(nbytes + 1);

        memcpy(ofile->name, last_file, nbytes + 1);

        oname = ofile->name;
    }

    if ((fp = fopen(oname, "w+")) == NULL)
    {
        return EXIT_FAILURE;
    }

    if (ofile->temp != NULL)
    {
        if (chmod(oname, file_stat.st_mode | S_IRUSR | S_IWUSR) != 0)
        {
            fatal_err(errno, E_SYS, NULL);
        }
    }

    ofile->fp = fp;
    ofile->backup = (backup == BACKUP_FILE) ? true : false;

    return EXIT_SUCCESS;
}
