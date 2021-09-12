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

int term_pos = 0;

static const char *table_8bit[] =       ///< 8-bit characters
{
    "[80]",  "[81]",  "[82]",  "[83]",  "[84]",  "[85]",  "[86]",  "[87]",
    "[88]",  "[89]",  "[8A]",  "[8B]",  "[8C]",  "[8D]",  "[8E]",  "[8F]",
    "[90]",  "[91]",  "[92]",  "[93]",  "[94]",  "[95]",  "[96]",  "[97]",
    "[98]",  "[99]",  "[9A]",  "[9B]",  "[9C]",  "[9D]",  "[9E]",  "[9F]",
    "[A0]",  "<!!>",  "<C/>",  "<L->",  "[A4]",  "<Y->",  "[A6]",  "<S0>",
    "<X0>",  "<C0>",  "<a_>",  "<<<>",  "[AC]",  "[AD]",  "[AE]",  "[AF]",
    "<0^>",  "<+->",  "<2^>",  "<3^>",  "[B4]",  "</U>",  "<P!>",  "<.^>",
    "[B8]",  "<1^>",  "<o_>",  "<>>>",  "<14>",  "<12>",  "[BE]",  "<\?\?>",
    "<A`>",  "<A'>",  "<A^>",  "<A~>",  "<A\">", "<A*>",  "<AE>",  "<C,>",
    "<E`>",  "<E'>",  "<E^>",  "<E\">", "<I`>",  "<I'>",  "<I^>",  "<I\">",
    "[D0]",  "<N~>",  "<O`>",  "<O'>",  "<O^>",  "<O~>",  "<O\">", "<OE>",
    "<O/>",  "<U`>",  "<U'>",  "<U^>",  "<U\">", "<Y\">", "[DE]",  "<ss>",
    "<a`>",  "<a'>",  "<a^>",  "<a~>",  "<a\">", "<a*>",  "<ae>",  "<c,>",
    "<e`>",  "<e'>",  "<e^>",  "<e\">", "<i`>",  "<i'>",  "<i^>",  "<i\">",
    "[F0]",  "<n~>",  "<o`>",  "<o'>",  "<o^>",  "<o~>",  "<o\">", "<oe>",
    "<o/>",  "<u`>",  "<u'>",  "<u^>",  "<u\">", "<y\">", "[FE]",  "[FF]",
};

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
            case CTRL_C:
                if (f.et.rubout && !f.e0.display)
                {
                    tprint("\n\e[7m Cancel \e[0m\n");
                }
                else
                {
                    tputc(c, true);
                    tputc(LF, true);
                }

                break;
                
            case LF:
                tputc(CR, true);
                //lint -fallthrough

            case BS:
            case HT:
            case CR:
                tputc(c, true);

                break;

            case VT:
                for (uint i = 0; i < VT_LINES; ++i)
                {
                    tputc(CR, true);
                    tputc(LF, true);
                }

                break;

            case FF:
                for (uint i = 0; i < FF_LINES; ++i)
                {
                    tputc(CR, true);
                    tputc(LF, true);
                }

                break;

            case DEL:
                break;

            case ESC:

#if     !defined(DOLLAR_ESC)

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
        tprint("%s", table_8bit[c & 0x7f]);
    }
}


///
///  @brief    Called to print one or more lines after a successful search, or
///            before a prompt.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_flag(int_t flag)
{
    // Don't print anything if we're in a loop or a macro.

    if (check_loop() || check_macro())
    {
        return;
    }

    int_t m, n;
    int mark;

    if (flag == -1)
    {
        m = getdelta_ebuf((int_t)0);
        n = getdelta_ebuf((int_t)1);
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
            mark = (int)n;
        }
        else
        {
            mark = -1;
        }

        if (m == 0)
        {
            n = getdelta_ebuf((int_t)1);
            m = getdelta_ebuf((int_t)0);
        }
        else
        {
            n = getdelta_ebuf(m);
            m = getdelta_ebuf(1 - m);
        }
    }

    int last = NUL;

    for (int_t i = m; i < n; ++i)
    {
        if (i == 0 && mark != -1)
        {
            tputc(mark, false);
        }

        int c = getchar_ebuf(i);

        if (c == EOF)
        {
            break;
        }
        else if (c == LF && last != CR)
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
        tprint("%s", "\n");
    }

    tprint("%s", teco_prompt);
}


///
///  @brief    Output NUL-terminated string to terminal, and possibly also to
///            log file.
///
///  @returns  No. of characters output (like printf).
///
////////////////////////////////////////////////////////////////////////////////

int tprint(
    const char *format,                 ///< printf() format string
    ...)                                ///< Remaining arguments for printf()
{
    assert(format != NULL);             // Error if no format

    char buf[KB + 1];                   // Allow 1 KB for string

    va_list argptr;
    va_start(argptr, format);
    int nbytes = vsnprintf(buf, sizeof(buf), format, argptr);

    va_end(argptr);

    assert((uint_t)(uint)nbytes <= KB); // Sanity check

    if (nbytes != 0)
    {
        for (int i = 0; i < nbytes; ++i)
        {
            if (buf[i] == '\n')
            {
                tputc('\r', false);
            }

            tputc(buf[i], false);
        }
    }

    return nbytes;
}


///
///  @brief    Output character to terminal or display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void tputc(int c, int input)
{
    if (isdelim(c) || c == CR)
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

#if     defined(DISPLAY_MODE)

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

#if     defined(EU_COMMAND)

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
        int nspaces;

        switch (c)
        {
            case HT:
                nspaces = TAB_WIDTH - (term_pos % TAB_WIDTH);

                for (int i = 0; i < nspaces; ++i)
                {
                    tputc(' ', false);
                }

                break;

            case LF:
                if (!f.et.image && !f.e0.o_redir)
                {
                    tputc(CR, false);   // Convert LF to CR/LF
                }
                //lint -fallthrough

            case BS:
            case VT:
            case FF:
            case CR:
                tputc(c, false);

                break;

            case DEL:
                break;

            case ESC:

#if     !defined(DOLLAR_ESC)

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
        tprint("%s", table_8bit[c & 0x7f]);
    }
}
