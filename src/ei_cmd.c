///
///  @file    ei_cmd.c
///  @brief   Execute EI command.
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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "exec.h"


static struct buffer *file_buf = NULL;  ///< Command buffer for indirect file


// Local functions

static void free_indirect(void);


///
///  @brief    Execute EI command: read TECO command file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EI(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len == 0)            // EN` command?
    {
        close_indirect();               // Close any open file

        return;
    }

    create_filename(&cmd->text1);

    if (open_indirect((bool)false) == EXIT_FAILURE &&
        open_indirect((bool)true)  == EXIT_FAILURE)
    {
        if (cmd->colon_set)
        {
            push_expr(TECO_FAILURE, EXPR_VALUE);
        }

        prints_err(E_FNF, filename_buf);
    }
    else if (cmd->colon_set)
    {
        push_expr(TECO_SUCCESS, EXPR_VALUE);
    }
}


///
///  @brief    Check for input from indirect file.
///
///  @returns  true if need to execute command string, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool check_indirect(void)
{
    if (mung_file == NULL)
    {
        return read_indirect();
    }

    // Here if user wants to "mung" a file (that is, there was a command-line
    // argument that specified that a file to process just like an EI command).

    filename_buf = alloc_mem((uint)strlen(mung_file) + 1);

    strcpy(filename_buf, mung_file);

    mung_file = NULL;                   // Only do this once

    if (open_indirect((bool)false) == EXIT_FAILURE &&
        open_indirect((bool)true)  == EXIT_FAILURE)
    {
        prints_err(E_FNF, filename_buf);
    }

    return true;
}


///
///  @brief    Close indirect file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void close_indirect(void)
{
    struct ifile *stream = &ifiles[IFILE_INDIRECT];

    if (stream->fp != NULL)             // Is indirect file open?
    {
        fclose(stream->fp);
        
        stream->fp  = NULL;
        stream->eof = true;
        stream->cr  = false;
    }

    free_indirect();

    reset_buf();
}


///
///  @brief    Free indirect command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void free_indirect(void)
{
    if (file_buf != NULL)
    {
        free_mem(&file_buf->buf);
        free_mem(&file_buf);
    }
}


///
///  @brief    Open indirect file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

int open_indirect(bool default_type)
{
    if (default_type)
    {
        if (strchr(filename_buf, '.') == NULL)
        {
            strcat(filename_buf, ".tec");
        }
    }

    if (open_input(filename_buf, IFILE_INDIRECT) == EXIT_SUCCESS)
    {
        free_indirect();

        file_buf = alloc_mem((uint)sizeof(struct buffer));

        file_buf->len  = 0;
        file_buf->pos  = 0;
        file_buf->size = STR_SIZE_INIT;
        file_buf->buf  = alloc_mem(file_buf->size);

        set_buf(file_buf);

        return EXIT_SUCCESS;
    }

    if (errno == ENOENT || errno == ENODEV)
    {
        return EXIT_FAILURE;
    }

    // Here if error was something other than a simple file not found.

    prints_err(E_FNF, filename_buf);
}


///
///  @brief    Read input from indirect file if one is open.
///
///  @returns  true if need to execute command string, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool read_indirect(void)
{
    static bool esc_1 = false;
    static bool esc_2 = false;

    struct ifile *stream = &ifiles[IFILE_INDIRECT];

    if (stream->fp == NULL)             // Is indirect file open?
    {
        esc_1 = esc_2 = false;

        return false;                   // No
    }

    if ((esc_1 && esc_2) || feof(stream->fp)) // Are we at end of file?
    {
        close_indirect();               // Yes, just close file

        return true;                    // TODO: return false here?
    }

    int c;

    while ((c = fgetc(stream->fp)) != EOF)
    {
        store_buf(c);

        if (c != ESC)
        {
            esc_1 = esc_2 = false;
        }
        else
        {
            esc_2 = esc_1;
            esc_1 = true;

            if (esc_2)
            {
                return true;
            }
        }
    }        

    if (esc_1 && esc_2)                 // Did it end with <ESC><ESC>?
    {
        return true;                    // If so, execute it
    }
    else
    {
        return false;                   // Else wait until user terminates it
    }
}


///
///  @brief    Test to see if we have an indirect file open.
///
///  @returns  true if indirect file is open, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool test_indirect(void)
{
    struct ifile *stream = &ifiles[IFILE_INDIRECT];

    return (stream->fp != NULL);
}
