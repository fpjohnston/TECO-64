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
#include "eflags.h"
#include "term.h"

#if     defined(DISPLAY_MODE)

#include <ncurses.h>

#endif

// Local functions

static void rubout_chrs(uint n);

static bool rubout_CR(void);

static bool rubout_FF(void);

static bool rubout_HT(void);

static bool rubout_LF(void);

static bool rubout_VT(void);


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

    uint n = 1;

#if     defined(DISPLAY_MODE)

    if (f.e0.display)
    {
        n = (uint)strlen(unctrl((uint)c));
    }
    else

#endif

    if (iscntrl(c))
    {
        switch (c)
        {
            case LF:
                if (rubout_LF())
                {
                    return;
                }

                break;

            case CR:
                if (rubout_CR())
                {
                    return;
                }

                break;

            case FF:
                if (rubout_FF())
                {
                    return;
                }

                break;

            case VT:
                if (rubout_VT())
                {
                    return;
                }

                break;

            case HT:
                if (rubout_HT())
                {
                    return;
                }

                break;

            case BS:
            case ESC:
                break;

            case DEL:                   // DEL doesn't echo
                return;

            default:                    // Generic control sequence
                ++n;

                break;
        }
    }
    else if (!isascii(c))
    {
        n += 3;                         // 8-bit chrs. are printed as [xx]
    }

    rubout_chrs(n);
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
///  @returns  true if rubout required special handling, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool rubout_CR(void)
{
    if (!f.et.rubout)
    {
        return false;
    }
    else if (!f.e0.display)
    {
        if (f.e3.CR_in)
        {
            tprint("\e[K");             // Clear to end of line
            retype_line(start_tbuf());  // Retype current line
        }

        return true;
    }
    else
    {
        return false;                   // TODO: add handling for ncurses
    }
}


///
///  @brief    Rubout form feed.
///
///  @returns  true if rubout required special handling, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool rubout_FF(void)
{
    if (!f.et.rubout)
    {
        return false;
    }
    else if (!f.e0.display)
    {
        tprint("\e[%dF", FF_LINES);     // Move up 8 lines
        retype_line(start_tbuf());      // Retype current line

        return true;
    }
    else
    {
        return false;                   // TODO: add handling for ncurses
    }
}


///
///  @brief    Rubout horizontal tab.
///
///  @returns  true if rubout required special handling, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool rubout_HT(void)
{
    if (!f.et.rubout)
    {
        return false;
    }
    else if (!f.e0.display)
    {
        tprint("\r\e[K");               // Go to start of line, then clear it
        retype_line(start_tbuf());      // Retype current line

        return true;
    }
    else
    {
        return false;                   // TODO: add handling for ncurses
    }
}


///
///  @brief    Rubout line feed.
///
///  @returns  true if rubout required special handling, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool rubout_LF(void)
{
    if (!f.et.rubout)
    {
        return false;
    }
    else if (!f.e0.display)
    {
        tprint("\e[F");                 // Move up 1 line

        if (!f.e3.CR_in)
        {
            tprint("\e[K");             // Clear to end of line
            retype_line(start_tbuf());  // Retype current line

            return true;
        }

        return true;
    }
    else
    {
        return false;                   // TODO: add handling for ncurses
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
///  @returns  true if rubout required special handling, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool rubout_VT(void)
{
    if (!f.et.rubout)
    {
        return false;
    }
    else if (!f.e0.display)
    {
        tprint("\e[%dF", VT_LINES);     // Move up 4 lines
        retype_line(start_tbuf());      // Retype current line

        return true;
    }
    else
    {
        return false;                   // TODO: add handling for ncurses
    }
}
