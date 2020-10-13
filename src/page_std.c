///
///  @file    page_std.c
///  @brief   Standard paging functions (write pages to file immediately, no
///           backwards paging allowed).
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include <stdio.h>

#include "teco.h"
#include "ascii.h"
#include "editbuf.h"
#include "eflags.h"
#include "errors.h"
#include "page.h"
#include "term.h"

uint page_count = 1;                ///< Current page number

///
///  @brief    Read in previous page (invalid for standard paging).
///
///  @returns  Throws exception.
///
////////////////////////////////////////////////////////////////////////////////

bool page_backward(int unused1, bool unused2)
{
    throw(E_NPA);                       // P argument cannot be negative
}


///
///  @brief    Flush out remaining pages (no-op for standard paging).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void page_flush(FILE *unused1)
{
    page_count = 0;
}


///
///  @brief    Write out current page.
///
///  @returns  false (edit buffer does not already have data).
///
////////////////////////////////////////////////////////////////////////////////

bool page_forward(FILE *fp, int start, int end, bool ff)
{
    assert(fp != NULL);                 // Error if no file block

    int last = NUL;

    // First pass - calculate how many characters we'll need to output

    for (int i = start; i < end; ++i)
    {
        int c = getchar_ebuf(i);

        // Translate LF to CR/LF if needed, unless last chr. was CR

        if (c == LF && last != CR && f.e3.ocrlf)
        {
            fputc(CR, fp);
        }

        fputc(c, fp);

        last = c;
    }

    if (ff)                             // Add a form feed if necessary
    {
        fputc(FF, fp);
    }

    ++page_count;

    return false;
}


///
///  @brief    Reset all pages (no-op for standard paging)./
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_pages(void)
{
}


///
///  @brief    Read in previous page, discarding current page (invalid for
///            standard paging).
///
///  @returns  Throws exception.
///
////////////////////////////////////////////////////////////////////////////////

void yank_backward(FILE *unused1)
{
    throw(E_NYA);                       // Numeric argument with Y
}
