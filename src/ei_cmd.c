///
///  @file    ei_cmd.c
///  @brief   Execute EI command.
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
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "exec.h"

// Local functions

static int open_indirect(bool colon_set, bool default_type);


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

    if (open_indirect(cmd->colon_set, false) == EXIT_FAILURE
        && open_indirect(cmd->colon_set, true) == EXIT_FAILURE)
    {
        if (cmd->colon_set)
        {
            push_expr(OPEN_FAILURE, EXPR_VALUE);
        }

        prints_err(E_FNF, filename_buf);
    }
    else if (cmd->colon_set)
    {
        push_expr(OPEN_SUCCESS, EXPR_VALUE);
    }
}


///
///  @brief    Open indirect file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static int open_indirect(bool colon_set, bool default_type)
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
    struct ifile *stream = &ifiles[IFILE_INDIRECT];

    if (stream->fp == NULL)             // Is indirect file open?
    {
        return false;                   // No
    }

    int c;

    last_in = NUL;

    while ((c = fgetc(stream->fp)) != EOF)
    {
        store_buf(c);

        if (c == ESC)                   // Escape?
        {
            if (last_in == ESC)         // Yes; 2nd in a row?
            {
                close_indirect();

                return true;
            }
        }

        last_in = c;
    }        

    close_indirect();

    return false;
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
}


    
