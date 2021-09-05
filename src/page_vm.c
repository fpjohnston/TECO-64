///
///  @file    page_vm.c
///  @brief   Paging functions using virtual memory.
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
#include <stddef.h>
#include <stdio.h>
#include <string.h>

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
    uint_t size;                        ///< Size of page in bytes
    uint_t cr;                          ///< No. of added CRs in page
    bool ocrlf;                         ///< Copy of f.e3.ocrlf
    bool ff;                            ///< Append form feed to page
};

struct page_table
{
    uint count;                         ///< Current page number
    struct page *head;                  ///< Head of page list
    struct page *tail;                  ///< Tail of page list
    struct page *stack;                 ///< Saved page stack
};

static struct page_table ptable[] =
{
    { .count = 0, .head = NULL, .tail = NULL, .stack = NULL },
    { .count = 0, .head = NULL, .tail = NULL, .stack = NULL },
};

// Local functions

static void copy_page(struct page *page);

static void link_page(struct page *page);

static struct page *make_page(int_t start, int_t end, bool ff);

static bool pop_page(void);

static void push_page(struct page *page);

static struct page *unlink_page(void);

static void write_page(FILE *fp, struct page *page);


///
///  @brief    Copy data in page to edit buffer, and then delete it.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void copy_page(struct page *page)
{
    assert(page != NULL);

    kill_ebuf();                        // Delete all data in edit buffer

    // If there is a form feed in the page (because the user added it while
    // editing), then we have to treat it as an end of page marker, and only
    // return the data after the form feed. We also reduce the count for the
    // current page and add it back onto the list.

    bool split = false;                 // true if we split the page
    uint_t nbytes = page->size;         // No. of bytes to copy to edit buffer
    char *p;

    if (!f.e3.nopage && (p = strrchr(page->addr, '\f')) != NULL)
    {
        split       = true;
        *p++        = NUL;              // Make sure we don't match next time
        nbytes     -= (uint)(p - page->addr);
        page->size -= nbytes + 1;
        page->ff    = true;
    }
    else
    {
        p = page->addr;
    }

    // Copy page data to edit buffer. Since this data originated in the edit
    // buffer, we assume it will fit, and therefore don't bother to check for
    // warnings or errors.

    while (nbytes-- != 0)
    {
        (void)add_ebuf(*p++);
    }

    setpos_ebuf(t.B);                   // Reset to start of buffer

    if (split)
    {
        link_page(page);
    }
    else
    {
        f.ctrl_e = page->ff;

        free_mem(&page->addr);
        free_mem(&page);
    }
}


///
///  @brief    Add page to tail of linked list.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void link_page(struct page *page)
{
    assert(page != NULL);

    if (ptable[ostream].head == NULL)
    {
        ptable[ostream].head = page;       // Head -> new page
    }
    else
    {
        page->prev = ptable[ostream].tail; // New page -> last page
        ptable[ostream].tail->next = page; // Last page -> new page
    }

    ptable[ostream].tail = page;           // Tail -> new page
}


///
///  @brief    Create page with data from edit buffer. Note that if we're
///            treating form feeds as a page delimiter, then we have to adjust
///            the page count for any form feeds that the user may have added
///            to the current page. This is to handle the situation where the
///            user subsequently executes -P commands.
///
///  @returns  Pointer to page we created.
///
////////////////////////////////////////////////////////////////////////////////

static struct page *make_page(int_t start, int_t end, bool ff)
{
    struct page *page = alloc_mem((uint_t)sizeof(*page));

    page->next  = page->prev = NULL;
    page->size  = (uint)(end - start);
    page->cr    = 0;
    page->ocrlf = f.e3.ocrlf;
    page->ff    = ff;
    page->addr  = alloc_mem(page->size);

    char *p  = page->addr;
    char last = NUL;

    for (int_t i = start; i < end; ++i)
    {
        int c = getchar_ebuf(i);

        if (c == EOF)
        {
            break;
        }

        if (c == LF && last != CR && page->ocrlf)
        {
            ++page->cr;
        }
        else if (ff && c == FF)
        {
            ++ptable[ostream].count;
        }

        *p++ = last = (char)c;
    }

    assert(p - page->addr == (ptrdiff_t)page->size);

    return page;
}


///
///  @brief    Read in previous page.
///
///  @returns  true if we have a new page, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool page_backward(int_t count, bool ff)
{
    assert(count < 0);
    assert(ostream == OFILE_PRIMARY || ostream == OFILE_SECONDARY);

    // Create a new page with data from edit buffer and push it on the stack.

    struct page *page;

    if (t.Z != 0)
    {
        setpos_ebuf(t.B);

        page = make_page(t.B, t.Z, ff);

        kill_ebuf();

        push_page(page);
    }

    // Now unlink pages from linked list and push them on the stack, until we
    // find the one we want (which will then be popped off the stack).

    while (count++ < 0)
    {
        if ((page = unlink_page()) == NULL)
        {
            break;
        }

        push_page(page);                // Then push it on stack

        if (count == 0)
        {
            bool havedata = pop_page();

            if (havedata)
            {
                --ptable[ostream].count;
            }

            return havedata;
        }
    }

    if (ptable[ostream].count > 0)
    {
        --ptable[ostream].count;
    }

    return f.ctrl_e = false;
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
///  @brief    Flush out remaining pages.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void page_flush(FILE *fp)
{
    assert(fp != NULL);                 // Error if no file block
    assert(ostream == OFILE_PRIMARY || ostream == OFILE_SECONDARY);

    struct page *page;

    // Write out all pages in queue.

    while ((page = ptable[ostream].head) != NULL)
    {
        ptable[ostream].head = page->next;

        write_page(fp, page);
    }

    while ((page = ptable[ostream].stack) != NULL)
    {
        ptable[ostream].stack = page->next;

        write_page(fp, page);
    }

    ptable[ostream].count = 0;
}


///
///  @brief    Write out current page.
///
///  @returns  true if already have buffer data, false if not.
///
////////////////////////////////////////////////////////////////////////////////

bool page_forward(FILE *unused, int_t start, int_t end, bool ff)
{
    assert(ostream == OFILE_PRIMARY || ostream == OFILE_SECONDARY);

    if (start != end)
    {
        struct page *page = make_page(start, end, ff);

        link_page(page);
    }

    ++ptable[ostream].count;

    return pop_page();
}


///
///  @brief    Pop page from stack, and copy to edit buffer.
///
///  @returns  true if there was a page on stack, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool pop_page(void)
{
    struct page *page = ptable[ostream].stack;

    if (page == NULL)
    {
        return false;
    }

    ptable[ostream].stack = page->next;

    copy_page(page);

    return true;
}


///
///  @brief    Push page onto stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void push_page(struct page *page)
{
    assert(page != NULL);

    page->next = ptable[ostream].stack;

    ptable[ostream].stack = page;
}


///
///  @brief    Reset all pages (used by EK command).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_pages(uint stream)
{
    assert(stream == OFILE_PRIMARY || stream == OFILE_SECONDARY);

    struct page *page;

    while ((page = ptable[stream].head) != NULL)
    {
        ptable[stream].head = page->next;

        free_mem(&page->addr);
        free_mem(&page);
    }

    ptable[stream].tail = NULL;
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
///  @brief    Unlink page from end of linked list.
///
///  @returns  Returned page, or NULL if list is empty.
///
////////////////////////////////////////////////////////////////////////////////

static struct page *unlink_page(void)
{
    struct page *page;

    if ((page = ptable[ostream].tail) == NULL)
    {
        return NULL;
    }

    assert(page->next == NULL);

    if (page->prev == NULL)             // Only page in list?
    {
        ptable[ostream].head = NULL;
        ptable[ostream].tail = NULL;
    }
    else
    {
        ptable[ostream].tail        = page->prev;

        page->prev->next = NULL;
        page->prev       = NULL;
        page->next       = NULL;
    }

    return page;
}


///
///  @brief    Write page to file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void write_page(FILE *fp, struct page *page)
{
    assert(fp != NULL);
    assert(page != NULL);

    char   last   = NUL;
    uint_t nbytes = page->size + page->cr + (page->ff ? 1 : 0);
    char   *src   = page->addr;
    char   *dst   = alloc_mem(nbytes);
    char   *p     = dst;

    while (page->size-- > 0)
    {
        char c = *src++;

        if (c == LF && last != CR && page->ocrlf)
        {
            *p++ = CR;
        }

        *p++ = last = c;
    }

    if (page->ff)
    {
        *p++ = FF;
    }

    fwrite(dst, (ulong)nbytes, 1uL, fp);

    free_mem(&dst);
    free_mem(&page->addr);
    free_mem(&page);
}


///
///  @brief    Read in previous page, discarding current page.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void yank_backward(FILE *unused)
{
    assert(ostream == OFILE_PRIMARY || ostream == OFILE_SECONDARY);

    struct page *page;

    if (!pop_page())
    {
        if ((page = unlink_page()) == NULL)
        {
            kill_ebuf();
        }
        else
        {
            copy_page(page);
        }
    }

    if (ptable[ostream].count > 0)
    {
        --ptable[ostream].count;
    }
}
