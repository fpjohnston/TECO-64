///
///  @file    term_out.c
///  @brief   System-independent functions to output to user's terminal.
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
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "file.h"
#include "term.h"
#include "window.h"


// Local functions

static void display(int c);

static void echo_chr(int c, void (*print)(int c));


///
///  @brief    Display character on terminal or in window.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void display(int c)
{
    if (putc_win(c))
    {
        return;
    }

    fputc(c, stdout);
}


///
///  @brief    Echo character in a printable form, either as c, ^c, or [c].
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void echo_in(int c)
{
    echo_chr(c, print_echo);
}


///
///  @brief    Echo character in a printable form, either as c, ^c, or [c].
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void echo_out(int c)
{
    echo_chr(c, print_chr);
}


///
///  @brief    Echo character in a printable form, either as c, ^c, or [c].
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void echo_chr(int c, void (*print)(int c))
{
    assert(print != NULL);

    if (c == ESC)
    {
        if (f.et.accent)
        {
            c = '`';
        }
        else if (f.ee != NUL)
        {
            c = f.ee;
        }
    }

    if (isprint(c))
    {
        (print)(c);
    }
    else if (!isascii(c))               // 8-bit character?
    {
        if (f.et.eightbit)              // Can terminal display it?
        {
            (print)(c);
        }
        else                            // No, make it printable
        {
            char chrbuf[5];             // [xx] + NUL

            uint nbytes = (uint)snprintf(chrbuf, sizeof(chrbuf), "[%02x]", c);

            assert(nbytes < sizeof(chrbuf));
        }
    }
    else                                // Must be a control character
    {
        switch (c)
        {
            case BS:
            case TAB:
            case LF:
            case CR:
                (print)(c);

                break;

            case DEL:
                break;

            case ESC:
                (print)('$');

                break;

            case FF:
//                (print)('\r');
                //lint -fallthrough

            case VT:
//                (print)('\n');
//                (print)('\n');
//                (print)('\n');
//                (print)('\n');

                //lint -fallthrough

            case CTRL_G:
//                (print)(CTRL_G);
//                break;
                //lint -fallthrough

            default:                    // Display as +^c
//                (print)(c);
                (print)('^');
                (print)(c + 'A' - 1);

                break;
         }
    }
}


///
///  @brief    Output character to terminal, and possibly to log file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_chr(int c)
{
    if (c == LF && !f.et.image)
    {
        print_chr(CR);
    }
    else if (c == CRLF)
    {
        print_chr(CR);

        c = LF;
    }
    else if (f.eu != -1)
    {
        if ((f.eu == 0 && islower(c)) || (f.eu == 1 && isupper(c)))
        {
            display('\'');
        }

        c = toupper(c);
    }

    display(c);

    if (!f.e2.noout)
    {
        FILE *fp = ofiles[OFILE_LOG].fp;

        if (fp != NULL)
        {
            fputc(c, fp);
        }
    }
}


///
///  @brief    Echo character to terminal, and possibly to log file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_echo(int c)
{
    if (c == LF && !f.et.image)
    {
        print_echo(CR);
    }
    else if (c == CRLF)
    {
        print_echo(CR);
        print_echo(LF);

        return;
    }

    if (!putc_win(c))
    {
        fputc(c, stdout);
    }

    if (!f.e2.noin)
    {
        FILE *fp = ofiles[OFILE_LOG].fp;

        if (fp != NULL)
        {
            fputc(c, fp);
        }
    }
}


///
///  @brief    Output NUL-terminated string to terminal, and possibly also to
///            log file. Note that this function is not used to output anything
///            to the text buffer window.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_str(const char *fmt, ...)
{
    assert(fmt != NULL);

    char buf[256];

    va_list argptr;
    va_start(argptr, fmt);

    (void)vsnprintf(buf, sizeof(buf), fmt, argptr);

    va_end(argptr);

    if (!puts_win(buf))
    {
        fputs(buf, stdout);
    }

    if (!f.e2.noout)
    {
        FILE *fp = ofiles[OFILE_LOG].fp;

        if (fp != NULL)
        {
            va_start(argptr, fmt);
            (void)vfprintf(fp, fmt, argptr);
            va_end(argptr);
        }
    }
}
