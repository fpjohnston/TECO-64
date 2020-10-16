///
///  @file    page_file.c
///  @brief   Paging functions using holding file.
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
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "editbuf.h"
#include "eflags.h"
#include "file.h"
#include "page.h"
#include "term.h"

uint page_count = 0;                ///< Current page number

///  @struct   page
///  @brief    Descriptor for storing a linked list of pages in memory.

struct page
{
    struct page *next;                  ///< Next page in queue
    struct page *prev;                  ///< Previous page in queue
    char *addr;                         ///< Address of page
    uint size;                          ///< Size of page in bytes
    uint cr;                            ///< No. of added CRs in page
    bool ocrlf;                         ///< Copy of f.e3.ocrlf
    bool ff;                            ///< Append form feed to page
};

static struct page *page_head = NULL;   ///< Head of page list
static struct page *page_tail = NULL;   ///< Tail of page list
static struct page *page_stack = NULL;  ///< Saved page stack

// Local functions

static void copy_page(struct page *page);

static void link_page(struct page *page);

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
    uint nbytes = page->size;           // No. of bytes to copy to edit buffer
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

    if (page_head == NULL)
    {
        page_head = page;               // Head -> new page
    }
    else
    {
        page->prev      = page_tail;    // New page -> last page
        page_tail->next = page;         // Last page -> new page
    }

    page_tail = page;                   // Tail -> new page
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

static struct page *make_page(int start, int end, bool ff)
{
    struct page *page = alloc_mem((uint)sizeof(*page));

    page->next  = page->prev = NULL;
    page->size  = (uint)t.Z;             // No. of bytes in edit buffer
    page->cr    = 0;
    page->ocrlf = f.e3.ocrlf;
    page->ff    = ff;
    page->addr  = alloc_mem((uint)page->size);

    char *p  = page->addr;
    char last = NUL;

    for (int i = start; i < end; ++i)
    {
        int c = getchar_ebuf(i);

        assert(c != EOF);

        if (c == LF && last != CR && page->ocrlf)
        {
            ++page->cr;
        }
        else if (ff && c == FF)
        {
            ++page_count;
        }

        *p++ = last = (char)c;
    }

    assert((uint)(p - page->addr) == page->size);

    return page;
}


///
///  @brief    Read in previous page.
///
///  @returns  true if we have a new page, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool page_backward(int count, bool ff)
{
    assert(count < 0);

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
                --page_count;
            }

            return havedata;
        }
    }

    if (page_count > 0)
    {
        --page_count;
    }

    return f.ctrl_e = false;
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

    struct page *page;

    // Write out all pages in queue.

    while ((page = page_head) != NULL)
    {
        page_head = page->next;

        write_page(fp, page);
    }

    while ((page = page_stack) != NULL)
    {
        page_stack = page->next;

        write_page(fp, page);
    }

    page_count = 0;
}


///
///  @brief    Write out current page.
///
///  @returns  true if already have buffer data, false if not.
///
////////////////////////////////////////////////////////////////////////////////

bool page_forward(FILE *unused1, int start, int end, bool ff)
{
    if (start != end)
    {
        struct page *page = make_page(start, end, ff);

        link_page(page);
    }

    ++page_count;

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
    struct page *page = page_stack;

    if (page == NULL)
    {
        return false;
    }

    page_stack = page->next;

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

    page->next = page_stack;

    page_stack = page;
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
        page_head = page->next;

        free_mem(&page->addr);
        free_mem(&page);
    }

    page_tail = NULL;
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

    if ((page = page_tail) == NULL)
    {
        return NULL;
    }

    assert(page->next == NULL);

    if (page->prev == NULL)             // Only page in list?
    {
        page_head = NULL;
        page_tail = NULL;
    }
    else
    {
        page_tail        = page->prev;

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

    char last   = NUL;
    uint nbytes = page->size + page->cr + (page->ff ? 1 : 0);
    char *src   = page->addr;
    char *dst   = alloc_mem(nbytes);
    char *p     = dst;

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

void yank_backward(FILE *unused1)
{
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

    if (page_count > 0)
    {
        --page_count;
    }
}
