///
///  @file    errors.c
///  @brief   TECO error messages and error functions.
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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "display.h"
#include "eflags.h"
#include "errcodes.h"
#include "term.h"

#include "cbuf.h"
#include "errtables.h"


#define ERR_BUF_SIZE    64          ///< Size of error buffer

#define MAX_LINES       20          ///< Max. lines in error message

int last_error = E_NUL;             ///< Last error encountered

static char *last_command;          ///< Command string for last error


// Local functions

static void convert(char *buf, uint bufsize, const char *err_str, uint len);


///
///  @brief    Convert string to canonical format by making control characters
///            visible.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void convert(char *buf, uint bufsize, const char *err_str, uint len)
{
    assert(buf != NULL);
    assert(err_str != NULL);

    while (bufsize-- > 0 && len-- > 0)
    {
        int c = *err_str++;

        if (isprint(c))                 // Printable character?
        {
            *buf++ = (char)c;
        }
        else if (c == TAB)
        {
            buf += sprintf(buf, "%s", "<TAB>");
        }
        else if (c == LF)
        {
            buf += sprintf(buf, "<LF>");
        }
        else if (c == VT)
        {
            buf += sprintf(buf, "<VT>");
        }
        else if (c == FF)
        {
            buf += sprintf(buf, "<FF>");
        }
        else if (c == CR)
        {
            buf += sprintf(buf, "<CR>");
        }
        else if (c == ESC)
        {
            buf += sprintf(buf, "<ESC>");
        }
        else if (c >= DEL)              // DEL or 8-bit character?
        {
            buf += sprintf(buf, "[%02x]", c);
        }
        else
        {
            buf += sprintf(buf, "<^%c>", c + 'A' - 1);
        }
    }

    *buf = NUL;
}


///
///  @brief    Free up any memory we may have allocated.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exit_error(void)
{
    if (last_command != NULL)
    {
        free_mem(&last_command);
    }
}


///
///  @brief    Print last command string that caused an error.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_error(void)
{
    if (last_command != NULL)
    {
        const char *p = last_command;

        for (;;)
        {
            int c = *p++;

            if (c == NUL)
            {
                break;
            }

            type_out(c);
        }
    }
}


///
///  @brief    Print verbose error message after immediate action "/" command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_verbose(int error)
{
    if (error <= 0 || (uint)error > countof(errhelp))
    {
        return;
    }

    const char *help = errhelp[error];

    if (help == NULL)
    {
        return;
    }

    const char *start = help;
    const char *end = start;
    uint pos = 0;
    uint maxlines = MAX_LINES;

    while (maxlines > 0)
    {
        const char *next = strchr(end + pos, ' ');
        uint len;

        if (next != NULL)
        {
            len = (uint)(next - start);

            if (len > (uint)w.width - 4)
            {
                tprint("    %.*s", (end - start) - 1, start);
                type_out(LF);

                --maxlines;

                start = end;
                pos = 0;
            }
            else
            {
                end = next + 1;
            }
        }
        else
        {
            len = (uint)strlen(start);

            tprint("    %.*s", (int)len, start);
            type_out(LF);

            break;
        }
    }
}


///
///  @brief    TECO exception handler. Can be called for one of the following
///            conditions:
///
///            1. A bad command (e.g., an invalid Q-register name, or more than
///               two numeric arguments).
///
///            2. A command which could not be successfully executed (e.g.,
///               search string not found, or requested input file with no read
///               permissions).
///
///            3. An unexpected event (e.g., out of memory).
///
///            Note that this function is not used for processor errors such as
///            dereferencing an invalid pointer.
///
///            The specific error code is used to determine what arguments (if
///            any) have also been passed by the caller. There are four cases
///            possible:
///
///            1. Errors that include a single character (e.g., E_POP) that is
///               interpolated in the error message. If the character is a 7-bit
///               printable character, it is output as is; if it is an 8-bit
///               character, it is output as [xx]; if it is a control character,
///               it is output as ^x.
///
///            3. The E_SYS error code, which includes a string that contains
///               the name of a file (or which is NULL) that is appended after
///               the error message.
///
///            3. Other error codes that include a string that is interpolated
///               in the error message.
///
///            4. Error codes that do not require any special processing.
///
///            In addition to those four cases, E_XAB is treated specially if
///            a command is not currently being executed, and just causes a
///            return to main program level without any message being printed.
///
///  @returns  n/a (longjmp back to main program).
///
////////////////////////////////////////////////////////////////////////////////

noreturn void throw(int error, ...)
{
    const char *file_str = NULL;
    const char *err_str;
    char err_buf[ERR_BUF_SIZE];
    char c[1];

    va_list args;

    va_start(args, error);

    switch (error)
    {
        case E_IEC:
        case E_IFC:
        case E_IFN:
        case E_ILL:
        case E_IQN:
        case E_IUC:
        case E_POP:
            c[0] = (char)va_arg(args, int);
            convert(err_buf, (uint)sizeof(err_buf), c, 1);
            err_str = err_buf;

            break;

        case E_SYS:
            err_str = strerror(errno);  // Convert errno to string
            file_str = va_arg(args, const char *);

            break;

        case E_BAT:
        case E_DUP:
        case E_FIL:
        case E_FNF:
        case E_INI:
        case E_KEY:
        case E_SRH:
        case E_TAG:
            err_str = va_arg(args, const char *);
            convert(err_buf, (uint)sizeof(err_buf), err_str,
                    (uint)strlen(err_str));
            err_str = err_buf;

            break;

        default:
            if ((uint)error > countof(errlist))
            {
                error = E_NUL;
            }

            err_str = NULL;

            break;
    }

    va_end(args);

    const char *code = errlist[error].code;
    const char *text = errlist[error].text;

    // Save copy of current command string, up to point of error.

    if (error != E_XAB)
    {
        free_mem(&last_command);
        last_command = alloc_mem(cbuf->pos + 1);
        sprintf(last_command, "%.*s", (int)cbuf->pos, cbuf->data);
    }

    // If CTRL/C and we're not executing a command, don't print error.

    if (error != E_XAB || f.e0.exec)
    {
        tprint("?%s", code);            // Always print code

        last_error = error;

        if (f.eh.verbose != 1)          // Need to print more?
        {
            tprint("   ");
            tprint(text, err_str);

            if (error == E_SYS && file_str != NULL)
            {
                tprint(" for '%s'", file_str);
            }

            type_out(LF);

            if (f.eh.verbose == 3)
            {
                print_verbose(last_error);
            }
        }
        else
        {
            type_out(LF);
        }

        if (f.eh.command)
        {
            echo_tbuf(0);
        }
    }

    if (f.et.abort)                     // Abort on error?
    {
        reset();                        // Clean things up

        exit(EXIT_FAILURE);
    }

    longjmp(jump_main, MAIN_ERROR);     // Back to the shadows again!
}
