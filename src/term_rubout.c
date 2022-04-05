///
///  @file    term_rubout.c
///  @brief   System-independent functions to handle RUBOUT on user's terminal.
///
///  @copyright 2022 Franklin P. Johnston / Nowwith Treble Software
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

#include <ctype.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "display.h"
#include "eflags.h"
#include "term.h"


// Local functions

static void rubout_chrs(uint n);

static void rubout_CR(void);

static void rubout_FF(void);

static void rubout_HT(void);

static void rubout_LF(void);

static void rubout_VT(void);


///
///  @brief    Rubout single echoed character.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void rubout_chr(int c)
{
    // Echoed input is normally only a single character, but control characters
    // may require more (or fewer) RUBOUTs.

    if (f.e0.display)
    {
        rubout_key(c);
    }
    else if (iscntrl(c))
    {
        switch (c)
        {
            case LF:
                rubout_LF();

                return;

            case CR:
                rubout_CR();

                return;

            case FF:
                rubout_FF();

                return;

            case VT:
                rubout_VT();

                return;

            case HT:
                rubout_HT();

                return;

            case ESC:
                rubout_chrs(1);         // ESC echoes as $ or `

                return;

            case BS:                    // BS isn't printed, so no echo
            case DEL:                   // DEL isn't printed, so no echo
                return;

            default:                    // Generic control sequence
                rubout_chrs(2);         // Control chrs. are printed as ^X

                return;
        }
    }
    else if (!isascii(c))
    {
        rubout_chrs(4);                 // 8-bit chrs. are printed as [xx]

        return;
    }
    else
    {
        rubout_chrs(1);                 // Default is 1 echoed chr.
    }
}


///
///  @brief    Rubout multiple echoed characters.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void rubout_chrs(uint n)
{
    while (n-- > 0)
    {
        echo_in(BS);
        echo_in(SPACE);
        echo_in(BS);
    }
}


///
///  @brief    Rubout carriage return.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void rubout_CR(void)
{
    if (f.et.rubout)
    {
        if (f.e3.CR_in)
        {
            tprint("\e[K");             // Clear to end of line
            retype_line(start_tbuf());  // Retype current line
        }
    }
}


///
///  @brief    Rubout form feed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void rubout_FF(void)
{
    if (f.et.rubout)
    {
        tprint("\e[%dF", FF_LINES);     // Move up 8 lines
        retype_line(start_tbuf());      // Retype current line
    }
}


///
///  @brief    Rubout horizontal tab.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void rubout_HT(void)
{
    if (f.et.rubout)
    {
        tprint("\r\e[K");               // Go to start of line, then clear it
        retype_line(start_tbuf());      // Retype current line
    }
}


///
///  @brief    Rubout line feed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void rubout_LF(void)
{
    if (f.et.rubout)
    {
        tprint("\e[F");                 // Move up 1 line

        if (!f.e3.CR_in)
        {
            tprint("\e[K");             // Clear to end of line
            retype_line(start_tbuf());  // Retype current line
        }
    }
}


///
///  @brief    Rubout entire line (including the prompt).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void rubout_line(void)
{
    int c;

    while ((c = delete_tbuf()) != EOF)
    {
        if (isdelim(c))                 // Delimiter for previous line?
        {
            store_tbuf(c);              // Yes, put it back

            break;
        }

        rubout_chr(c);
    }

    uint n = (uint)strlen(teco_prompt);

    rubout_chrs(n);
}


///
///  @brief    Rubout vertical tab.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void rubout_VT(void)
{
    if (f.et.rubout)
    {
        tprint("\e[%dF", VT_LINES);     // Move up 4 lines
        retype_line(start_tbuf());      // Retype current line
    }
}
