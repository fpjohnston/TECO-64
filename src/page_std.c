///
///  @file    page_std.c
///  @brief   Standard paging functions (write pages to file immediately, no
///           backwards paging allowed).
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
#include <stdio.h>

#include "teco.h"
#include "ascii.h"
#include "editbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "file.h"
#include "page.h"
#include "term.h"

//
//  Note: the following definitions are more complicated than they need to be,
//        but this was done in order to allow the page_count() and set_page()
//        functions to be defined identically to those used in page_vm.c.
//

///  @struct   page_table
///  @brief    Description of page counts for output streams.

struct page_table
{
    uint count;                         ///< Current page number
};

///  @var      ptable
///  @brief    Stored data for primary and secondary output streams.

static struct page_table ptable[] =
{
    { .count = 0 },
    { .count = 0 },
};


///
///  @brief    Read in previous page (invalid for standard paging).
///
///  @returns  Throws exception.
///
////////////////////////////////////////////////////////////////////////////////

bool page_backward(int_t unused1, bool unused2)
{
    throw(E_NPA);                       // P argument cannot be negative
}


///
///  @brief    Get page count for current page.
///
///  @returns  Page number (0 if no data in buffer).
///
////////////////////////////////////////////////////////////////////////////////

uint page_count(void)
{
    assert(ostream == OFILE_PRIMARY || ostream == OFILE_SECONDARY);

    return ptable[ostream].count;
}


///
///  @brief    Flush out remaining pages (no-op for standard paging).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void page_flush(FILE *unused)
{
    // Nothing to do
}


///
///  @brief    Write out current page.
///
///  @returns  false (edit buffer does not already have data).
///
////////////////////////////////////////////////////////////////////////////////

bool page_forward(FILE *fp, int_t start, int_t end, bool ff)
{
    assert(fp != NULL);                 // Error if no file block

    int last = NUL;

    // First pass - calculate how many characters we'll need to output

    for (int_t i = start; i < end; ++i)
    {
        int c = getchar_ebuf(i);

        if (c == EOF)
        {
            break;
        }

        // Translate LF to CR/LF if needed, unless last chr. was CR

        if (c == LF && last != CR && f.e3.CR_out)
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

    assert(ostream == OFILE_PRIMARY || ostream == OFILE_SECONDARY);

    ++ptable[ostream].count;

    return false;
}


///
///  @brief    Reset all pages (no-op for standard paging).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_pages(uint unused)
{
}


///
///  @brief    Set page count for current page.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_page(uint page)
{
    assert(ostream == OFILE_PRIMARY || ostream == OFILE_SECONDARY);

    ptable[ostream].count = page;
}


///
///  @brief    Read in previous page, discarding current page (invalid for
///            standard paging).
///
///  @returns  Throws exception.
///
////////////////////////////////////////////////////////////////////////////////

void yank_backward(FILE *unused)
{
    throw(E_NYA);                       // Numeric argument with Y
}
