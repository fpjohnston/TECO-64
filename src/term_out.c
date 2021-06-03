///
///  @file    term_out.c
///  @brief   System-independent functions to output to user's terminal.
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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "ascii.h"
#include "display.h"
#include "editbuf.h"
#include "eflags.h"
#include "file.h"
#include "term.h"


///  @var    term_pos
///
///  @brief  This is used to determine whether to truncate an output line, and
///          also whether to output a CR/LF before printing TECO's prompt.

static int term_pos = 0;

// Local functions

static void tputc(int c, int input);


///
///  @brief    Echo input character.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void echo_in(int c)
{
    c &= 0xFF;                          // Ensure we only print 8 bits

    if (c == f.ee && c != NUL)          // Alternative delimiter?
    {
        c = '`';                        // Yes, always echo as accent grave
    }

    if (isprint(c))
    {
        tputc(c, true);
    }
    else if (iscntrl(c))
    {
        switch (c)
        {
            case LF:
                tputc(CR, true);
                //lint -fallthrough

            case BS:
            case TAB:
            case CR:
                tputc(c, true);

                break;

            case VT:
                for (uint i = 0; i < 4; ++i)
                {
                    tputc(CR, true);
                    tputc(LF, true);
                }

                break;

            case FF:
                for (uint i = 0; i < 8; ++i)
                {
                    tputc(CR, true);
                    tputc(LF, true);
                }

                break;

            case DEL:
                break;

            case ESC:

#if     !defined(CONFIG_DOLLAR)

                if (f.et.accent || f.ee != NUL)
                {
                    tputc('`', false);
                }
                else

#endif
                {
                    tputc('$', false);
                }

                break;

            default:                    // Display as ^c
                tputc('^', true);
                tputc(c + 'A' - 1, true);

                break;
         }
    }
    else
    {
        tprint("[%02X]");
    }
}


///
///  @brief    Called to print one or more lines after a successful search, or
///            before a prompt.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_flag(int flag)
{
    // Don't print anything if we're in a loop or a macro.

    if (check_loop() || check_macro())
    {
        return;
    }

    int m, n;
    int mark;

    if (flag == -1)
    {
        m = getdelta_ebuf(0);
        n = getdelta_ebuf(1);
        mark = -1;
    }
    else
    {
        m = flag / 256;
        n = flag % 256;

        if (n == 0)
        {
            return;
        }
        else if (n > NUL && n < SPACE)
        {
            mark = LF;
        }
        else if (n >= SPACE && n < DEL)
        {
            mark = n;
        }
        else
        {
            mark = -1;
        }

        if (m == 0)
        {
            n = getdelta_ebuf(1);
            m = getdelta_ebuf(0);
        }
        else
        {
            n = getdelta_ebuf(m);
            m = getdelta_ebuf(1 - m);
        }
    }

    int last = NUL;

    for (int i = m; i < n; ++i)
    {
        if (i == 0 && mark != -1)
        {
            type_out(mark);
        }

        int c = getchar_ebuf(i);

        if (c == LF && last != CR)
        {
            type_out(CR);
        }

        type_out(c);

        last = c;
    }
}


///
///  @brief    Print the TECO prompt (this may be the standard asterisk, or
///            something else specified by the user).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_prompt(void)
{
    if (f.e1.prompt && term_pos != 0)
    {
        tprint("%s", "\r\n");
    }

    tprint("%s", teco_prompt);
}


///
///  @brief    Output NUL-terminated string to terminal, and possibly also to
///            log file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void tprint(
    const char *format,                 ///< printf() format string
    ...)                                ///< Remaining arguments for printf()
{
    assert(format != NULL);             // Error if no format

    char buf[STR_SIZE_INIT];

    va_list argptr;
    va_start(argptr, format);
    int nbytes = vsnprintf(buf, sizeof(buf), format, argptr);

    va_end(argptr);

    if (nbytes == 0)
    {
        return;
    }

    for (int i = 0; i < nbytes; ++i)
    {
        tputc(buf[i], false);
    }
}


///
///  @brief    Output character to terminal or display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void tputc(int c, int input)
{
    if (isdelim(c))
    {
        term_pos = 0;
    }
    else if (c == CR)
    {
        term_pos = 0;
    }
    else if (c != BS)
    {
        ++term_pos;
    }
    else if (term_pos > 0)
    {
        --term_pos;
    }

#if     defined(TECO_DISPLAY)

    if (putc_dpy(c))
    {
        return;
    }

#endif

    if (!f.et.truncate || term_pos < w.width)
    {
        fputc(c, stdout);

        FILE *fp = ofiles[OFILE_LOG].fp;

        if (fp != NULL && ((input && !f.e3.noin) || (!input && !f.e3.noout)))
        {
            fputc(c, fp);
        }
    }
}


///
///  @brief    Type output character.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void type_out(int c)
{
    c &= 0xFF;                          // Ensure we only print 8 bits

    if (f.et.image)
    {
        tputc(c, false);
    }

#if     defined(CONFIG_EU)

    else if (islower(c) && f.eu != -1)
    {
        if (f.eu == 0)
        {
            tputc('\'', false);
        }

        tputc(toupper(c), false);
    }
    else if (isupper(c) && f.eu == 1)
    {
        tputc('\'', false);
        tputc(c, false);
    }

#endif

    else if (isprint(c))
    {
        tputc(c, false);
    }
    else if (iscntrl(c))                // ASCII character?
    {
        switch (c)
        {
            case LF:
                if (!f.et.image)
                {
                    tputc(CR, false);   // Convert LF to CR/LF
                }
                //lint -fallthrough

            case BS:
            case TAB:
            case VT:
            case FF:
            case CR:
                tputc(c, false);

                break;

            case DEL:
                break;

            case ESC:

#if     !defined(CONFIG_DOLLAR)

                if (f.et.accent || f.ee != NUL)
                {
                    tputc('`', false);
                }
                else

#endif
                {
                    tputc('$', false);
                }

                break;

            default:                    // Display as ^c
                tputc('^', false);
                tputc(c + 'A' - 1, false);

                break;
         }
    }
    else if (f.et.eightbit)             // Can terminal display it?
    {
        tputc(c, false);
    }
    else                                // No, make it printable
    {
        tprint("[%02X]");
    }
}
