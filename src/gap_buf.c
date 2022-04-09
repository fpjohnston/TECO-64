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


///  @var     eb
///
///  @brief   Edit buffer data (internal)

static struct
{
    uchar *buf;                 ///< Start of buffer
    uint_t left;                ///< No. of bytes before gap
    uint_t right;               ///< No. of bytes after gap
    uint_t gap;                 ///< No. of bytes in gap
    const uint_t min;           ///< Minimum buffer size (fixed)
    const uint_t max;           ///< Maximum buffer size (fixed)
    struct edit t;              ///< Read/write copies of public variables
} eb =
{
    .buf    = NULL,
    .min    = EDIT_MIN,
    .max    = EDIT_MAX,
    .left   = 0,
    .right  = 0,
    .gap    = EDIT_INIT,
    .t =
    {
        .size   = EDIT_INIT,
        .B      = 0,
        .Z      = 0,
        .dot    = 0,
        .front  = EOF,
        .at     = EOF,
        .back   = EOF,
        .len    = 0,
        .pos    = 0,
        .before = 0,
        .after  = 0,
        .total  = 0,
    },
};

const struct edit *t = &eb.t;       ///< Read-only pointers to public variables


// Local functions

static int count_lines(int_t pos);

static int_t count_prev(uint_t nlines);

static int_t count_next(uint_t nlines);

static void reset_edit(void);

static void shift_left(uint_t nbytes);

static void shift_right(uint_t nbytes);


///
///  @brief    Change character at current position of dot.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void change_dot(int c)
{
    uint_t i = (uint_t)eb.t.dot;

    if (i >= eb.left)
    {
        i += eb.gap;
    }

    eb.buf[i] = eb.t.at = (uchar)c;
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

static int_t count_lines(int n)
{
    int_t start  = (n > 0) ? 0 : -eb.t.dot;
    int_t end    = (n < 0) ? 0 : eb.t.Z;
    int_t nlines = 0;

    for (int_t pos = start; pos < end; ++pos)
    {
        int c = read_edit(pos);

        if (c != EOF && isdelim(c))
        {
            ++nlines;
        }
    }

    return nlines;
}


///
///  @brief    Scan forward nlines in edit buffer.
///
///  @returns  Position following line terminator (relative to dot).
///
////////////////////////////////////////////////////////////////////////////////

static int_t count_next(uint_t nlines)
{
    for (int_t pos = eb.t.dot; pos < eb.t.Z; ++pos)
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

    return eb.t.Z;      // TODO: is this correct?
}


///
///  @brief    Scan backward n lines in edit buffer.
///
///  @returns  Position following line terminator (relative to dot).
///
////////////////////////////////////////////////////////////////////////////////

static int_t count_prev(uint_t nlines)
{
    for (int_t pos = eb.t.dot; pos-- > 0; )
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

    return 0;           // TODO: is this correct?
}


///
///  @brief    Decrement dot by 1.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void dec_dot(void)
{
    if (eb.t.dot > eb.t.B)
    {
        --eb.t.dot;

        eb.t.front = eb.t.at;
        eb.t.at    = eb.t.back;
        eb.t.back  = read_edit(-1);

        if (isdelim(eb.t.at))
        {
            --eb.t.before;
            ++eb.t.after;

            eb.t.pos = eb.t.dot - count_prev(0);
            eb.t.len = eb.t.pos + 1;
        }
        else
        {
            --eb.t.pos;
        }
    }
}


///
///  @brief    Delete n chars relative to current position.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void delete_edit(int_t nbytes)
{
    if (nbytes == 0)
    {
        return;
    }

    if (eb.t.dot == 0 && nbytes == eb.t.Z)    // Special case for HK command
    {
        kill_edit();
    }
    else
    {
        // Buffer is: [left][gap][right], with dot somewhere in [left] or
        // [right]. We shift things so that dot ends up immediately preceding
        // [gap]. Then any positive deletion is at the beginning of [right],
        // and any negative deletion is at the end of [left], which makes it
        // easy to do the required deletion.

        if ((uint_t)eb.t.dot < eb.left)
        {
            shift_right(eb.left - (uint_t)eb.t.dot);
        }
        else if ((uint_t)eb.t.dot > eb.left)
        {
            shift_left((uint_t)eb.t.dot - eb.left);
        }

        if (nbytes < 0)                 // Deleting backwards in [left]
        {
            nbytes = -nbytes;

            assert((uint_t)nbytes <= eb.left);

            eb.left -= (uint_t)nbytes;
            eb.t.dot -= nbytes;         // Backwards delete affects dot

            eb.t.back  = read_edit(-1);

            eb.t.before = count_lines(-1);
            eb.t.total  = eb.t.before + eb.t.after;
        }
        else                            // Deleting forward in [right]
        {
            assert((uint_t)nbytes <= eb.right);

            eb.right -= (uint_t)nbytes;

            eb.t.at    = read_edit(0);
            eb.t.front = read_edit(1);

            eb.t.after  = count_lines(1);
            eb.t.total  = eb.t.before + eb.t.after;
        }

        eb.t.pos = eb.t.dot - count_prev(0);
        eb.t.len = eb.t.pos + count_next(1);

        eb.gap += (uint_t)nbytes;       // Increase the gap
        eb.t.Z -= nbytes;               //  and decrease the total
    }
}


///
///  @brief    Clean up memory before we exit from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exit_edit(void)
{
    free_mem(&eb.buf);
}


///
///  @brief    Move dot to start of buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void first_dot(void)
{
    if (eb.t.dot > eb.t.B)
    {
        eb.t.dot   = eb.t.B;
        eb.t.back  = EOF;
        eb.t.at    = read_edit(0);
        eb.t.front = read_edit(1);

        eb.t.before = 0;
        eb.t.after  = eb.t.total;

        eb.t.len = count_next(1) - eb.t.dot;
        eb.t.pos = 0;
    }
}


///
///  @brief    Add character to edit buffer.
///
///  @returns  true if insert succeeded, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool insert_edit(int c)
{
    assert(eb.buf != NULL);             // Error if no edit buffer

    if (eb.gap == 0)
    {
        return false;                   // Buffer is already full
    }

    uint_t dot = (uint_t)eb.t.dot;

    if (dot < eb.left)
    {
        shift_right(eb.left - dot);
    }
    else if (dot > eb.left)
    {
        shift_left(dot - eb.left);
    }

    eb.buf[eb.left++] = (uchar)c;

    ++eb.t.dot;
    ++eb.t.Z;

    if (isdelim(c))
    {
        ++eb.t.before;
        ++eb.t.total;

        eb.t.pos = 0;
        eb.t.len = count_next(1) - eb.t.dot;
    }
    else
    {
        ++eb.t.pos;
        ++eb.t.len;
    }

    eb.t.back = eb.t.at;
    eb.t.at = c;

    if (--eb.gap < KB)                  // Less than 1 KB of buffer?
    {
        if (eb.t.size < eb.max)         // Yes, can we increase size?
        {
            // Try to make 25% larger

            uint_t size = size_edit(eb.t.size + (eb.t.size / 4));

            print_size(size);
        }

        if (eb.gap == 0)
        {
            return false;
        }
    }

    return true;                        // Insertion was successful
}


///
///  @brief    Increment dot by 1.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void inc_dot(void)
{
    if (eb.t.dot < eb.t.Z)
    {
        ++eb.t.dot;

        if (isdelim(eb.t.at))           // About to move across a line delimiter?
        {
            ++eb.t.before;
            --eb.t.after;

            eb.t.pos = 0;
            eb.t.len = count_next(1) - eb.t.dot;
        }
        else
        {
            ++eb.t.pos;
        }

        eb.t.back  = eb.t.at;
        eb.t.at    = eb.t.front;
        eb.t.front = read_edit(1);
    }
}


///
///  @brief    Initialize edit buffer. All that we need to do here is allocate
///            the memory for the buffer, since the rest of the initialization
///            for the 'eb' and 't' structures is done statically, above.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_edit(void)
{
    assert(eb.buf == NULL);             // Double initialization is an error

    eb.buf = alloc_mem(eb.t.size);

    reset_edit();
}


///
///  @brief    Kill the entire edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void kill_edit(void)
{
    if (eb.t.Z != 0)                    // Anything in buffer?
    {
        reset_edit();
    }
}


///
///  @brief    Move dot to end of buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void last_dot(void)
{
    if (eb.t.dot < eb.t.Z)
    {
        eb.t.dot   = eb.t.Z;
        eb.t.back  = read_edit(-1);
        eb.t.at    = EOF;
        eb.t.front = EOF;

        eb.t.before = eb.t.total;
        eb.t.after  = 0;

        eb.t.pos = eb.t.dot - count_prev(0);
        eb.t.len = eb.t.pos;
    }
}


///
///  @brief    Return length of string between dot and nth line terminator.
///
///  @returns  Number of characters relative to dot (can be plus or minus).
///
////////////////////////////////////////////////////////////////////////////////

int_t len_edit(int_t n)
{
    if (n > 0)
    {
        return count_next((uint_t)n) - eb.t.dot;
    }
    else
    {
        return count_prev((uint_t)-n) - eb.t.dot;
    }
}


///
///  @brief    Move dot to a relative position.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void move_dot(int_t delta)
{
    set_dot(eb.t.dot + delta);
}


///
///  @brief    Get ASCII value of nth character before or after dot.
///
///  @returns  ASCII value, or EOF if character outside of edit buffer.
///
////////////////////////////////////////////////////////////////////////////////

int read_edit(int_t pos)
{
    uint_t i = (uint_t)(eb.t.dot + pos); // Make relative position absolute

    if (i < eb.left + eb.right)
    {
        if (i >= eb.left)
        {
            i += eb.gap;
        }

        return eb.buf[i];
    }

    return EOF;
}


///
///  @brief    Reset buffer variables to initial conditions.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void reset_edit(void)
{
    eb.left     = 0;
    eb.right    = 0;
    eb.gap      = eb.t.size;

    eb.t.Z      = 0;
    eb.t.dot    = 0;
    eb.t.front  = EOF;
    eb.t.at     = EOF;
    eb.t.back   = EOF;
    eb.t.len    = 0;
    eb.t.pos    = 0;
    eb.t.before = 0;
    eb.t.after  = 0;
    eb.t.total  = 0;
}


///
///  @brief    Move dot to an absolute position.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_dot(int_t dot)
{
    assert(dot >= eb.t.B && dot <= eb.t.Z);

    if (eb.t.dot != dot)
    {
        if (dot == eb.t.dot + 1)
        {
            inc_dot();
        }
        else if (dot == eb.t.dot - 1)
        {
            dec_dot();
        }
        else if (dot == eb.t.B)
        {
            first_dot();
        }
        else if (dot == eb.t.Z)
        {
            last_dot();
        }
        else
        {
            eb.t.dot = dot;

            eb.t.back  = read_edit(-1);
            eb.t.at    = read_edit(0);
            eb.t.front = read_edit(1);

            eb.t.before = count_lines(-1);
            eb.t.after  = count_lines(1);

            eb.t.pos  = eb.t.dot - count_prev(0);
            eb.t.len  = count_next(1) - eb.t.dot;
            eb.t.len += eb.t.pos;
        }
    }
// TODO
//    tprint("dot = %d, pos = %d, len = %d, back = %d, at = %d, front = %d\n",
//           t->dot, t->pos, t->len, t->back, t->at, t->front);
}


///
///  @brief    Move characters from right side of gap to left side.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void shift_left(uint_t nbytes)
{
    uchar *src = eb.buf + eb.t.size - eb.right;
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
    uchar *dst = eb.buf + eb.t.size - eb.right;

    memmove(dst, src, (size_t)nbytes);
}


///
///  @brief    Set memory size for edit buffer.
///
///  @returns  New size, or 0 if size didn't change.
///
////////////////////////////////////////////////////////////////////////////////

uint_t size_edit(uint_t size)
{
    if (size > eb.max)
    {
        size = eb.max;
    }
    else if (size < eb.min)
    {
        size = eb.min;
    }

    uint_t runt = size & (KB - 1);

    if (runt != 0)                      // Partial kilobyte?
    {
        size += KB - runt;              // Yes, round up to next kilobyte
    }

    // Return if size is the same as, or is smaller than, the edit buffer.

    if (size == eb.t.size || size <= eb.left + eb.right)
    {
        return 0;
    }

    // We need to temporarily remove the gap before changing buffer size.

    shift_left(eb.right);               // Remove the gap

    if (size < eb.t.size)
    {
        eb.buf = shrink_mem(eb.buf, eb.t.size, eb.t.size - size);
    }
    else
    {
        eb.buf = expand_mem(eb.buf, eb.t.size, size - eb.t.size);
    }

    shift_right(eb.right);              // Restore the gap

    eb.t.size = size;
    eb.gap = eb.t.size - (eb.left + eb.right);

    return size;
}
