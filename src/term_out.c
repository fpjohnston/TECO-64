///
///  @file    term_out.c
///  @brief   System-independent functions to output to user's terminal.
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

const char *table_8bit[] =          ///< 8-bit characters
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
                if (!f.et.rubout || f.e0.display)
                {
                    tputc(c, true);
                    tputc(LF, true);
                }
                else
                {
                    print_alert("Cancel");
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
        m = t->pos;
        n = t->len - m;
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
            m = t->pos;
            n = t->len - m;
        }
        else
        {
            n = len_edit(m);
            m = len_edit(1 - m);
        }
    }

    int last = NUL;

    for (int_t i = m; i < n; ++i)
    {
        if (i == 0 && mark != -1)
        {
            tputc(mark, false);
        }

        int c = read_edit(i);

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
            if (buf[i] == LF)
            {
                tputc(CR, false);
            }

            tputc(buf[i], false);
        }
    }

    return nbytes;
}


///
///  @brief    Output character to terminal or display. Note that ALL output is
///            done here, other than output specific to display mode, such as a
///            status line. This is necessary because ncurses will get messed up
///            if we do output both with its functions and with Standard C func-
///            tions such as printf(). Also, having one lowest-level function
///            for function makes it easier to handle such things as updating
///            the terminal position, checking for truncation, and handling any
///            output to a possible log file.
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

    if (putc_cmd(c))
    {
        return;
    }

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
    if (c == NL)
    {
        if (f.e3.CR_type)
        {
            tputc(CR, false);
        }

        tputc(LF, false);

        return;
    }

    c &= 0xFF;                          // Ensure we only print 8 bits

    if (f.et.image)
    {
        tputc(c, false);
    }
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

            case BS:
            case LF:
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
    else                                // Must be 8-bit character
    {
        // This table is used to ensure we print 8-bit characters like TECO-32.

        static const char convert_table[] =
        {
            // 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            // 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            // A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF
                1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1,
            // B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF
                0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,
            // C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            // D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF
                1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
            // E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            // F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF
                1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        };

        // If the entry in the table above for the 8-bit character we want to
        // output is non-zero, then that means we will print it in the form
        // specified in table_8bit[].

        bool convert_chr = convert_table[c & 0x7f] ? true : false;

#if     !defined(__DECC)

        // For any non-VMS environment, we will additionally use the ET flag to
        // determine whether to do any special output.

        convert_chr = convert_chr || !f.et.eightbit;

#endif

        if (convert_chr)
        {
            tprint("%s", table_8bit[c & 0x7f]);
        }
        else
        {
            tputc(c, false);
        }
    }
}
