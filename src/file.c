///
///  @file    file.c
///  @brief   File-handling functions.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


struct ifile ifiles[IFILE_MAX];         ///< Input file descriptors

struct ofile ofiles[OFILE_MAX];         ///< Output file descriptors

uint istream = IFILE_PRIMARY;           ///< Current input stream

uint ostream = OFILE_PRIMARY;           ///< Current output stream

char *last_file = NULL;                 ///< Last file opened

char *filename_buf;                     ///< Allocated space for filename

// Local functions

static void file_exit(void);


///
///  @brief    Close output file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void close_output(uint stream)
{
    struct ofile *ofile = &ofiles[stream];
    FILE *fp;

    if ((fp = ofile->fp) != NULL)
    {
        fclose(fp);

        ofile->fp = NULL;
    }

    free_mem(&ofile->name);
    free_mem(&ofile->temp);

    ofile->backup = false;
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

void create_filename(const struct tstring *text)
{
    assert(text != NULL);
    assert(filename_buf != NULL);

    char *from = text->buf;
    char *to = filename_buf;

    for (uint i = 0; i < text->len; ++i)
    {
        if (isgraph(*from))
        {
            *to++ = *from++;
        }
        else
        {
            ++from;
        }
    }

    *to = NUL;
}


///
///  @brief    Close out file streams.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void file_exit(void)
{
    free_mem(&filename_buf);

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

        free_mem(&ofiles[i].name);
        free_mem(&ofiles[i].temp);
    }

    ostream = OFILE_PRIMARY;

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
    }

    ostream = OFILE_PRIMARY;

    last_file = NULL;

    if ((filename_buf = alloc_mem(FILENAME_MAX + 1)) == NULL)
    {
        exit(EXIT_FAILURE);
    }

    if (atexit(file_exit) != 0)
    {
        exit(EXIT_FAILURE);
    }
}


///
///  @brief    Open file for input.
///
///  @returns  EXIT_SUCCESS or EXIT_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////

int open_input(const char *filespec, uint stream)
{
    assert(filespec != NULL);
    
    struct ifile *ifile = &ifiles[stream];
    FILE *fp = ifile->fp;             

    if (fp != NULL)                     // Stream already open?
    {
        fclose(fp);                     // Yes, so close it
    }

//    free_mem(&last_file);

//    last_file = alloc_mem(text->len + 1);

//    sprintf(last_file, "%.*s", (int)text->len, text->buf);

    if ((fp = fopen(filename_buf, "r")) == NULL)
    {
        return EXIT_FAILURE;
    }

    ifiles[stream].fp  = fp;
    ifiles[stream].eof = false;
    ifiles[stream].cr  = false;

    return EXIT_SUCCESS;
}


///
///  @brief    Open file for output.
///
///  @returns  EXIT_SUCCESS or EXIT_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////

int open_output(const struct cmd *cmd, uint stream)
{
    assert(cmd != NULL);

    struct ofile *ofile = &ofiles[stream];

    if (ofile->fp != NULL)
    {
        if (stream == OFILE_LOG)
        {
            close_output(stream);
        }
        else
        {
            print_err(E_OFO);           // Output file is already open
        }
    }
    else
    {
        free_mem(&ofile->name);
        free_mem(&ofile->temp);
        free_mem(&last_file);
    }

    uint nbytes = cmd->text1.len;       // Length of text string
    
    last_file = alloc_mem(nbytes + 1);

    sprintf(last_file, "%.*s", (int)nbytes, cmd->text1.buf);

    bool exists = (access(last_file, F_OK) == 0);

    const char *oname = get_oname(ofile, nbytes, exists);

    // Issue warnings about existing files for EW and EL commands.

    if (exists)
    {
        if (toupper(cmd->c2) == 'W')    // EW command
        {
            if (f.e4.append)
            {
                printf("%s", "%Appending to existing file\r\n");
            }
            else
            {
                printf("%s", "%Superseding existing file\r\n");
            }
        }
        else if (toupper(cmd->c2) == 'L') // EL command
        {
            printf("%s", "%Appending to existing file\r\n");
        }
    }
    
    const char *mode = f.e4.append ? "a" : "w";
    
    if ((ofile->fp = fopen(oname, mode)) == NULL)
    {
        return EXIT_FAILURE;
    }

    ofile->backup = (toupper(cmd->c2) == 'B') ? true : false;

    return EXIT_SUCCESS;
}
