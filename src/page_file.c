///
///  @file    page_file.c
///  @brief   Paging functions using holding file.
///
///  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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
#include "file.h"
#include "page.h"


///
///  @brief    Write out current page.
///
///  @returns  true if already have buffer data, false if not.
///
////////////////////////////////////////////////////////////////////////////////

bool page_forward(FILE *fp, int_t start, int_t end, bool ff)
{
    assert(fp != NULL);                 // Error if no file block

    struct page page;
    int last = NUL;

    page.size = 0;                      // No. of bytes in output page

    // First pass - calculate how many characters we'll need to output

    for (int_t i = start; i < end; ++i)
    {
        int c = read_edit(i);

        if (c == EOF)
        {
            break;
        }

        // Translate LF to CR/LF if needed, unless last chr. was CR

        if (c == LF && last != CR && f.e3.CR_out)
        {
            ++page.size;
        }

        ++page.size;

        last = c;
    }

    if (ff)                             // Add a form feed if necessary
    {
        ++page.size;
    }

    // Second pass - fill page and then output it

    page.addr = alloc_mem((uint_t)page.size); // Allocate memory for page

    char *p = page.addr;

    for (int i = start; i < end; ++i)
    {
        int c = read_edit(i);

        if (c == EOF)
        {
            break;
        }

        // Translate LF to CR/LF if needed, unless last chr. was CR

        if (c == LF && last != CR && f.e3.CR_out)
        {
            *p++ = CR;
        }

        *p++ = (char)c;
    }

    if (ff)                             // Add a form feed if necessary
    {
        *p++ = FF;
    }

    assert((uint)(p - page.addr) == page.size); // Verify no. of output bytes

    fwrite(page.addr, (size_t)page.size, 1uL, fp);

    free_mem(&page.addr);
}
