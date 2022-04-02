///
///  @file    gap_buf.c
///  @brief   Text buffer functions.
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "display.h"
#include "editbuf.h"
#include "eflags.h"
#include "page.h"
#include "term.h"


#if     !defined(EDIT_MAX)

#if     INT_T == 64

#if     defined(PAGE_VM)
#define EDIT_MAX    (GB * 16)       ///< Maximum size is 16 GB (w/ VM)
#else
#define EDIT_MAX    (MB)            ///< Maximum size is 1 MB (w/o VM)
#endif

#elif   INT_T == 32

#if     defined(PAGE_VM)
#define EDIT_MAX    (GB)            ///< Maximum size is 1 GB (w/ VM)
#else
#define EDIT_MAX    (MB)            ///< Maximum size is 1 MB (w/o VM)
#endif

// The following is reserved for a possible future implementation
// of TECO for a 16-bit environment.

//#elif   INT_T == 16

//#define EDIT_MAX    (KB * 32)       ///< Maximum size is 32 KB (w/ VM)

#else

#error  Invalid integer size: expected 32, or 64

#endif

#endif

#if     !defined(EDIT_INIT)
#if     defined(PAGE_VM)

#define EDIT_INIT   (KB * 64)       ///< Initial size is 64 KB

#else

#define EDIT_INIT   (KB * 8)        ///< Initial size is 8 KB (w/o VM)

#endif
#endif

#define EDIT_MIN    (KB)            ///< Minimum size is 1 KB


///  @var    t
///
///  @brief  Edit buffer (external)

struct edit t =
{
    .B   = 0,
    .Z   = 0,
    .dot = 0,
};

///  @var     eb
///
///  @brief   Edit buffer data (internal)

static struct
{
    uchar *buf;                 ///< Start of buffer
    const uint_t min;           ///< Minimum buffer size (fixed)
    const uint_t max;           ///< Maximum buffer size (fixed)
    uint_t size;                ///< Current size of buffer, in bytes
    uint_t left;                ///< No. of bytes before gap
    uint_t right;               ///< No. of bytes after gap
    uint_t gap;                 ///< No. of bytes in gap
} eb =
{
    .buf   = NULL,
    .min   = EDIT_MIN,
    .max   = EDIT_MAX,
    .size  = EDIT_INIT,
    .left  = 0,
    .right = 0,
    .gap   = EDIT_INIT,
};


// Local functions

static int_t last_delim(uint_t nlines);

static int_t next_delim(uint_t nlines);

static void shift_left(uint_t nbytes);

static void shift_right(uint_t nbytes);


///
///  @brief    Add character to edit buffer.
///
///  @returns  EDIT_OK, EDIT_WARN, EDIT_FULL, or EDIT_ERROR.
///
////////////////////////////////////////////////////////////////////////////////

estatus add_ebuf(int c)
{
    assert(eb.buf != NULL);             // Error if no edit buffer

    if (eb.gap == 0)
    {
        return EDIT_ERROR;              // Buffer is already full
    }

    uint_t dot = (uint_t)t.dot;

    if (dot < eb.left)
    {
        shift_right(eb.left - dot);
    }
    else if (dot > eb.left)
    {
        shift_left(dot - eb.left);
    }

    eb.buf[eb.left++] = (uchar)c;

    // If we have no data in buffer, then we're on page 0, but
    // as soon as we add a character, then we're on page 1.

    if (page_count() == 0)
    {
        set_page(1);
    }

    mark_ebuf();

    ++t.dot;
    ++t.Z;

    if (--eb.gap < KB)                  // Less than 1 KB of buffer?
    {
        if (eb.size < eb.max)           // Yes, can we increase size?
        {
            uint_t newsize = (uint_t)eb.size;

            newsize += newsize / 4;

            setsize_ebuf(newsize);      // Try to make buffer 25% bigger
        }

        if (eb.gap == 0)
        {
            return EDIT_FULL;           // Buffer just filled up
        }
        else if (eb.gap < KB)           // Unable to increase size?
        {
            return EDIT_WARN;           // Buffer is getting full
        }
    }

    return EDIT_OK;                     // Insertion was successful
}


///
///  @brief    Delete n chars relative to current position.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void delete_ebuf(int_t nbytes)
{
    if (nbytes == 0)
    {
        return;
    }

    if (t.dot == 0 && nbytes == t.Z)    // Special case for HK command
    {
        eb.left = eb.right = t.Z = 0;
        eb.gap = eb.size;
    }
    else
    {
        // Buffer is: [left][gap][right], with dot somewhere in [left] or
        // [right]. We shift things so that dot ends up immediately preceding
        // [gap]. Then any positive deletion is at the beginning of [right],
        // and any negative deletion is at the end of [left], which makes it
        // easy to do any required deletion.

        if ((uint_t)t.dot < eb.left)
        {
            shift_right(eb.left - (uint_t)t.dot);
        }
        else if ((uint_t)t.dot > eb.left)
        {
            shift_left((uint_t)t.dot - eb.left);
        }

        if (nbytes < 0)                 // Deleting backwards in [left]
        {
            nbytes = -nbytes;

            assert((uint_t)nbytes <= eb.left);

            eb.left -= (uint_t)nbytes;
            t.dot   -= nbytes;          // Backwards delete affects dot
        }
        else                            // Deleting forward in [right]
        {
            assert((uint_t)nbytes <= eb.right);

            eb.right -= (uint_t)nbytes;
        }

        eb.gap += (uint_t)nbytes;       // Increase the gap
        t.Z    -= nbytes;               //  and decrease the total
    }

    mark_ebuf();
}


///
///  @brief    Clean up memory before we exit from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exit_ebuf(void)
{
    free_mem(&eb.buf);
}


///
///  @brief    Get ASCII value of nth character before or after dot.
///
///  @returns  ASCII value, or EOF if character outside of edit buffer.
///
////////////////////////////////////////////////////////////////////////////////

int getchar_ebuf(int_t n)
{
    uint_t pos = (uint_t)(t.dot + n);

    if (pos < eb.left + eb.right)
    {
        uint_t i = pos;

        if (pos >= eb.left)
        {
            i += eb.gap;
        }

        return eb.buf[i];
    }

    return EOF;
}


///
///  @brief    Return number of characters between dot and nth line terminator.
///
///  @returns  Number of characters relative to dot (can be plus or minus).
///
////////////////////////////////////////////////////////////////////////////////

int_t getdelta_ebuf(int_t n)
{
    if (n > 0)
    {
        return next_delim((uint_t)n) - t.dot;
    }
    else
    {
        return last_delim((uint_t)-n)- t.dot;
    }
}


///
///  @brief    Count no. of lines in buffer, as follows:
///
///            n < 0 -> No. of lines preceding current position.
///            n = 0 -> Total no. of lines in buffer.
///            n < 0 -> No. of lines following current position.
///
///  @returns  No. of total/following/preceding lines.
///
////////////////////////////////////////////////////////////////////////////////

int_t getlines_ebuf(int n)
{
    int_t start  = (n > 0) ? 0 : -t.dot;
    int_t end    = (n < 0) ? 0 : t.Z;
    int_t nlines = 0;

    for (int_t pos = start; pos < end; ++pos)
    {
        int c = getchar_ebuf(pos);

        if (c != EOF && isdelim(c))
        {
            ++nlines;
        }
    }

    return nlines;
}


///
///  @brief    Get size of edit buffer.
///
///  @returns  Size of edit buffer, in bytes.
///
////////////////////////////////////////////////////////////////////////////////

uint_t getsize_ebuf(void)
{
    return (uint_t)eb.size;
}


///
///  @brief    Initialize edit buffer. All that we need to do here is allocate
///            the memory for the buffer, since the rest of the initialization
///            for the 'eb' and 't' structures is done statically, above.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_ebuf(void)
{
    assert(eb.buf == NULL);             // Double initialization is an error

    eb.buf = alloc_mem(eb.size);
}


///
///  @brief    Kill the entire edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void kill_ebuf(void)
{
    setpos_ebuf(t.B);
    delete_ebuf(t.Z);
}


///
///  @brief    Scan backward n lines in edit buffer.
///
///  @returns  Position following line terminator (relative to dot).
///
////////////////////////////////////////////////////////////////////////////////

static int_t last_delim(uint_t nlines)
{
    for (int_t pos = t.dot; pos-- > 0; )
    {
        int_t i = pos;

        if ((uint_t)pos >= eb.left)     // Is position on right side of gap?
        {
            i += (int_t)eb.gap;         // Yes, so add bias
        }

        int c = eb.buf[i];

        if (isdelim(c) && nlines-- == 0)
        {
            return ++pos;
        }
    }

    // There aren't n lines preceding the current position, so just back up to
    // the beginning of the buffer.

    return 0;
}


///
///  @brief    Scan forward nlines in edit buffer.
///
///  @returns  Position following line terminator (relative to dot).
///
////////////////////////////////////////////////////////////////////////////////

static int_t next_delim(uint_t nlines)
{
    for (int_t pos = t.dot; pos < t.Z; ++pos)
    {
        int_t i = pos;

        if ((uint_t)pos >= eb.left)     // Is position on right side of gap?
        {
            i += (int_t)eb.gap;         // Yes, so add bias
        }

        int c = eb.buf[i];

        if (isdelim(c) && --nlines == 0)
        {
            return ++pos;
        }
    }

    return t.Z;
}


///
///  @brief    Set buffer position.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void setpos_ebuf(int_t pos)
{
    if ((uint_t)pos <= eb.left + eb.right)
    {
        unmark_dot();                   // Tell display that 'dot' changed

        t.dot = pos;
    }
}


///
///  @brief    Set memory size for edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void setsize_ebuf(uint_t nbytes)
{
    uint_t newsize = (uint_t)nbytes;

    if (newsize > eb.max)
    {
        newsize = eb.max;
    }
    else
    {
        if (newsize < eb.min)
        {
            newsize = eb.min;
        }

        // Round up to K boundary

        newsize += KB - 1;
        newsize /= KB;
        newsize *= KB;
    }

    // Nothing to do if no change, or requested size is smaller than what's
    // in the edit buffer.

    if (newsize == eb.size || newsize <= eb.left + eb.right)
    {
        return;
    }

    // We need to temporarily remove the gap before changing buffer size.

    shift_left(eb.right);               // Remove the gap

    if (newsize < eb.size)
    {
        eb.buf = shrink_mem(eb.buf, eb.size, eb.size - newsize);
    }
    else
    {
        eb.buf = expand_mem(eb.buf, eb.size, newsize - eb.size);
    }

    shift_right(eb.right);              // Restore the gap

    eb.size = newsize;
    eb.gap  = eb.size - (eb.left + eb.right);

    if (f.e0.display || f.et.abort)     // Display mode on or abort bit set?
    {
        return;                         // Yes, don't print messages then
    }

    if (newsize >= GB)
    {
        tprint("[%uG bytes]\n", (uint)(newsize / GB));
    }
    else if (newsize >= MB)
    {
        tprint("[%uM bytes]\n", (uint)(newsize / MB));
    }
    else
    {
        tprint("[%uK bytes]\n", (uint)(newsize / KB));
    }
}


///
///  @brief    Move characters from right side of gap to left side.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void shift_left(uint_t nbytes)
{
    uchar *src = eb.buf + eb.size - eb.right;
    uchar *dst = eb.buf + eb.left;

    eb.left  += nbytes;
    eb.right -= nbytes;

    memmove(dst, src, (size_t)nbytes);
}


///
///  @brief    Move characters from left side of gap to right side.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void shift_right(uint_t nbytes)
{
    eb.left  -= nbytes;
    eb.right += nbytes;

    uchar *src = eb.buf + eb.left;
    uchar *dst = eb.buf + eb.size - eb.right;

    memmove(dst, src, (size_t)nbytes);
}
