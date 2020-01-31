///
///  @file    search.c
///  @brief   Search utility functions.
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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"
#include "qreg.h"
#include "textbuf.h"

///   @var    last_search
///   @brief  Last string searched for

struct tstring last_search = { .len = 0 };

// Local functions

static int isblankx(int c, struct search *s);

static int isqreg(int c, struct search *s);

static int issymbol(int c);

static bool match_chr(int c, struct search *s);

static bool match_str(struct search *s);

static void reset_search(void);


///
///  @brief    Initialize search string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_search(void)
{
    if (atexit(reset_search) != 0)
    {
        fatal_err(errno, E_UIT, NULL);
    }
}


///
///  @brief    Check for multiple blanks (spaces or tabs) at current position.
///
///            Note that we return 1/0 instead of true/false for compatibility
///            with the ANSI isxxx() functions.
///
///  @returns  1 if one or more blanks found, else 0.
///
////////////////////////////////////////////////////////////////////////////////

static int isblankx(int c, struct search *s)
{
    assert(s != NULL);

    if (!isblank(c))
    {
        return 0;
    }

    while (s->text_pos < s->text_end)
    {
        c = getchar_tbuf(s->text_pos++);

        if (!isblank(c))
        {
            --s->text_pos;

            break;
        }
    }

    return 1;
}


///
///  @brief    Check for a match with one of the characters in a Q-register.
///
///            Note that we return 1/0 instead of true/false for compatibility
///            with the ANSI isxxx() functions.
///
///  @returns  1 if a match found, else 0.
///
////////////////////////////////////////////////////////////////////////////////

static int isqreg(int c, struct search *s)
{
    assert(s != NULL);

    int qname;
    bool qlocal = false;            
    
    if (s->match_len-- == 0)
    {
        print_err(E_IQN);
    }

    if ((qname = *s->match_buf++) == '.')
    {
        qlocal = true;

        if (s->match_len-- == 0)
        {
            print_err(E_IQN);
        }

        qname = *s->match_buf++;
    }

    struct qreg *qreg = get_qreg(qname, qlocal);

    for (uint i = 0; i < qreg->text.len; ++i)
    {
        if (c == qreg->text.buf[i])
        {
            return 1;
        }
    }

    return 0;
}


///
///  @brief    Check for a match on a symbol constituent: alphanumeric, period,
///            maybe dollar sign or underscore, depending on operating environ-
///            ment.
///
///            Note that we return 1/0 instead of true/false for compatibility
///            with the ANSI isxxx() functions.
///
///  @returns  1 if a match found, else 0.
///
////////////////////////////////////////////////////////////////////////////////

static int issymbol(int c)
{
    if (isalnum(c)   || c == '.' ||
        (f.e2.dollar && c == '$') ||
        (f.e2.ubar   && c == '_'))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


///
///  @brief    Check for a match on the current character in the text buffer,
///            allowing for the use of match control constructs in the search
///            string. Note that we can be called recursively.
///
///  @returns  true if a match found, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool match_chr(int c, struct search *s)
{
    assert(s != NULL);

    if (s->match_len-- == 0)
    {
        print_err(E_ISS);
    }

    int match = *s->match_buf++;

    if (match == CTRL_E)
    {
        if (s->match_len-- == 0)
        {
            print_err(E_ISS);
        }

        match = toupper(*s->match_buf++);

        if ((match == 'A' && isalpha(c))     ||
            (match == 'B' && isblankx(c, s)) ||
            (match == 'C' && issymbol(c))    ||
            (match == 'D' && isdigit(c))     ||
            (match == 'G' && isqreg(c, s))   ||
            (match == 'L' && isdelim(c))     ||
            (match == 'R' && isalnum(c))     ||
            (match == 'S' && isblankx(c, s)) ||
            (match == 'V' && islower(c))     ||
            (match == 'W' && isupper(c))     ||
            (match == 'X'))
        {
            return true;
        }

        // <CTRL/E>nnn matches character whose octal value is nnn.

        if (match >= '0' && match <= '7')
        {
            int n = match - '0';

            // Loop until we run out of octal digits

            while (s->match_len > 0)
            {
                if (*s->match_buf < '0' && *s->match_buf > '7')
                {
                    break;
                }

                --s->match_len;

                n *= 8;                 // Shift digit over
                n += *s->match_buf++ - '0'; // Add in new digit
            }

            return (c == n) ? true : false;
        }

        // TODO: maybe process ^EM, and ^E[a,b,c...]?

        return false;
    }
    else if (match == CTRL_N)
    {
        return !match_chr(match, s);
    }
    else if ((match == CTRL_S && !isalnum(c)) || match == CTRL_X)
    {
        return true;
    }
    else if ((f.ctrl_x == 0 && tolower(c) == tolower(match)))
    {
        return true;
    }
    else if (c == match)
    {
        return true;
    }

    return false;
}


///
///  @brief    Check to see if text string matches search string.
///
///  @returns  true if match, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool match_str(struct search *s)
{
    assert(s != NULL);

    while (s->match_len > 0)
    {
        int c = getchar_tbuf(s->text_pos++);

        if (!match_chr(c, s))
        {
            return false;
        }
    }

    return true;
}


///
///  @brief    Reset search string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void reset_search(void)
{
    free_mem(&last_search.buf);
}


///
///  @brief    Search backward through text buffer to find next instance of
///            string in search buffer.
///
///  @returns  true if string found (text_pos will contain the buffer position
///            relative to dot), and false if not found.
///
////////////////////////////////////////////////////////////////////////////////

bool search_backward(struct search *s)
{
    assert(s != NULL);

    // Start search at current position and see if we can get a match. If not,
    // decrement position by one, and try again. If we reach the end of the
    // text buffer without a match, then return failure, otherwise update our
    // position and return success.

    while (s->text_start > s->text_end) // Search to beginning of buffer
    {
        s->text_pos  = s->text_start--; // Start at current position
        s->match_len = last_search.len; // No. of characters left to match
        s->match_buf = last_search.buf; // Start of match characters

        if (match_str(s))
        {
            return true;
        }
    }

    return false;
}


///
///  @brief    Search forward through text buffer to find next instance of
///            string in search buffer.
///
///  @returns  true if string found (text_pos will contain the buffer position
///            relative to dot), and false if not found.
///
////////////////////////////////////////////////////////////////////////////////

bool search_forward(struct search *s)
{
    assert(s != NULL);

    // Start search at current position and see if we can get a match. If not,
    // increment position by one, and try again. If we reach the end of the
    // text buffer without a match, then return failure, otherwise update our
    // position and return success.

    while (s->text_start < s->text_end) // Search to end of buffer
    {
        s->text_pos  = s->text_start++; // Start at current position
        s->match_len = last_search.len; // No. of characters left to match
        s->match_buf = last_search.buf; // Start of match characters

        if (match_str(s))
        {
            // The following affects how much we move dot on multiple occurrence
            // searches. Normally we skip over the whole matched string when
            // proceeding to the nth search match. But if movedot is set, then
            // only increment by one character. This is only significant if the
            // first character of the string occurs again in the middle of it.

            if (f.ed.movedot)
            {
                ++s->text_start;
            }
            else
            {
                s->text_start = s->text_pos;
            }

            return true;
        }
    }

    return false;
}


///
///  @brief    Search forward through text buffer to find next instance of
///            string in search buffer.
///
///  @returns  true if string found (text_pos will contain the buffer position
///            relative to dot, and false if not found.
///
////////////////////////////////////////////////////////////////////////////////

bool search_loop(struct search *s)
{
    assert(s != NULL);

    struct ifile *ifile = &ifiles[istream];
    struct ofile *ofile = &ofiles[ostream];

    // Start search at current position and see if we can get a match. If not,
    // increment position by one, and try again. If we reach the end of the
    // text buffer without a match, then return failure, otherwise update our
    // position and return success.

    while (s->count > 0)
    {
        if ((*s->search)(s))            // Successful search?
        {
            --s->count;                 // Yes, count down occurrence
        }
        else
        {
            switch (s->type)
            {
                case SEARCH_N:
                    if (ofile->fp == NULL)
                    {
                        print_err(E_NFO); // No file for output
                    }       

                    if (!next_page(0, getsize_tbuf(), v.ff, (bool)true))
                    {
                        return false;
                    }

                    break;

                case SEARCH_U:
                    if (!f.ed.yank)
                    {
                        print_err(E_YCA); // Y command aborted
                    }
                    //lint -fallthrough

                case SEARCH_E:
                    if (ifile->fp == NULL)
                    {
                        print_err(E_NFI); // No file for input
                    }       

                    if (!next_yank())
                    {
                        return false;
                    }

                    setpos_tbuf(0);

                    break;

                default:
                case SEARCH_S:
                    return false;
            }

            // Here with a new page, so reinitialize pointers

            s->text_start = 0;
            s->text_end   = (int)getsize_tbuf();
        }
    }

    int dot = (int)getpos_tbuf() + s->text_pos;

    setpos_tbuf((uint)dot);

    return true;
}


///
///  @brief    Check to see if we need to print anything after a successful
///            search.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void search_print(void)
{
    int m    = 0;
    int n    = f.es;
    int flag = 0;         

    // Never print anything if we're in a loop or a macro.

    if (check_loop() || check_macro())
    {
        return;
    }

    if (n == 0)
    {
        return;
    }
    else
    {
        m = f.es / 256;
        n = f.es % 256;
    }

    if (n > NUL && n < SPACE)
    {
        flag = LF;
    }
    else if (n >= SPACE && n < DEL)
    {
        flag = n;
    }
    else if (n == -1)
    {
        flag = -1;
    }
    else
    {
        return;
    }

    if (m == 0)
    {
        n = getdelta_tbuf(1);
        m = getdelta_tbuf(0);
    }
    else
    {
        n = getdelta_tbuf(m);
        m = getdelta_tbuf(1 - m);
    }

    for (int i = m; i < n; ++i)
    {
        if (i == 0 && flag != -1)
        {
            echo_out(flag);
        }

        int c = getchar_tbuf(i);

        if (c == LF)
        {
            echo_out(CR);
        }

        echo_out(c);
    }
}
