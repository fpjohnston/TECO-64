///
///  @file    page_file.c
///  @brief   Paging functions using holding file.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include "term.h"

///  @struct   page
///  @brief    Descriptor for storing a linked list of pages in memory.

struct page
{
    struct page *next;                  ///< Next page in queue
    struct page *prev;                  ///< Previous page in queue
    char *addr;                         ///< Address of page
    uint size;                          ///< Size of page in bytes
    bool ff;                            ///< Append form feed to page
};

static struct page *page_head = NULL;   ///< Head of page list
static struct page *page_tail = NULL;   ///< Tail of page list


///
///  @brief    Read in previous page.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void page_backward(FILE *fp, int count)
{
    assert(fp != NULL);

    // Save current page in queue, retrieve previous page.
}


///
///  @brief    Flush out remaining pages.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void page_flush(FILE *fp)
{
    assert(fp != NULL);

    struct page *page;

    // Write out all pages in queue.

    while ((page = page_head) != NULL)
    {
        print_str("writing page at %p with %d bytes\r\n", page->addr, page->size);

        page_head = page->next;
        fwrite(page->addr, page->size, 1, fp);
        free_mem(&page->addr);
        free_mem(&page);
    }
}


///
///  @brief    Write out current page.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void page_forward(FILE *fp, int start, int end, bool ff)
{
    assert(fp != NULL);

    if (start == end)
    {
        return;
    }

    struct page *page = alloc_mem(sizeof(*page));
    int last = NUL;

    page->next = page->prev = NULL;
    page->size = 0;                      // No. of bytes in output page
    page->ff = ff;

    // First pass - calculate how many characters we'll need to output

    for (int i = start; i < end; ++i)
    {
        int c = getchar_ebuf(i);

        // Translate LF to CR/LF if needed, unless last chr. was CR

        if (c == LF && last != CR && f.e3.ocrlf)
        {
            ++page->size;
        }

        ++page->size;

        last = c;
    }

    if (page->ff)                       // Add a form feed if necessary
    {
        ++page->size;
    }

    // Second pass - fill page and then output it

    page->addr = alloc_mem((uint)page->size); // Allocate memory for page

    char *p = page->addr;

    last = NUL;

    for (int i = start; i < end; ++i)
    {
        int c = getchar_ebuf(i);

        // Translate LF to CR/LF if needed, unless last chr. was CR

        if (c == LF && last != CR && f.e3.ocrlf)
        {
            *p++ = CR;
        }

        *p++ = (char)c;

        last = c;
    }

    if (page->ff)                       // Add a form feed if necessary
    {
        *p++ = FF;
    }

    assert((uint)(p - page->addr) == page->size);

    print_str("storing page at %p with %d bytes\r\n", page->addr, page->size);

    if (page_head == NULL)
    {
        page_head = page_tail = page;
    }
    else
    {
        page->prev = page_tail;         // New page -> last page
        page_tail->next = page;         // Last page -> new page
        page_tail = page;               // Tail -> new page
    }
}


///
///  @brief    Reset all pages (used by EK command).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_pages(void)
{
    struct page *page;

    while ((page = page_head) != NULL)
    {
        print_str("resetting page at %p with %d bytes\r\n", page->addr, page->size);

        page_head = page->next;
        free_mem(&page->addr);
        free_mem(&page);
    }

    page_tail = NULL;
}


///
///  @brief    Read in previous page, discarding current page.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void yank_backward(FILE *fp)
{
    assert(fp != NULL);

    // Discard current page, retrieve previous page from queue.
}
