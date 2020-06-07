///
///  @file    page_vm.c
///  @brief   Paging functions using virtual memory.
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
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void page_forward(FILE *fp, int start, int end, bool ff)
{
    assert(fp != NULL);

    struct page page;
    int last = NUL;

    page.size = 0;                      // No. of bytes in output page

    // First pass - calculate how many characters we'll need to output

    for (int i = start; i < end; ++i)
    {
        int c = getchar_ebuf(i);

        // Translate LF to CR/LF if needed, unless last chr. was CR

        if (c == LF && last != CR && f.e2.ocrlf)
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

    page.addr = alloc_mem((uint)page.size); // Allocate memory for page

    char *p = page.addr;

    for (int i = start; i < end; ++i)
    {
        int c = getchar_ebuf(i);

        // Translate LF to CR/LF if needed, unless last chr. was CR

        if (c == LF && last != CR && f.e2.ocrlf)
        {
            *p++ = CR;
        }

        *p++ = (char)c;
    }

    if (ff)                             // Add a form feed if necessary
    {
        *p++ = FF;
    }

    assert((uint)(p - page.addr) == page.size);

    fwrite(page.addr, (size_t)page.size, 1uL, fp);

    free_mem(&page.addr);
}
