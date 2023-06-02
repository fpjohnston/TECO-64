///
///  @file    gap_buf.c
///  @brief   Text buffer functions.
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

#if     !defined(NDEBUG)

#include <ctype.h>

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "editbuf.h"
#include "eflags.h"
#include "page.h"


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
        .nextc  = EOF,
        .c      = EOF,
        .lastc  = EOF,
        .len    = 0,
        .pos    = 0,
        .line   = 0,
        .nlines = 0,
    },
};

const struct edit *t = &eb.t;       ///< Read-only pointers to public variables


// Local functions

static void end_insert(uint_t nbytes);

static int_t next_line(uint_t nlines);

static int_t prev_line(uint_t nlines);

static void reset_edit(void);

static void shift_left(uint_t nbytes);

static void shift_right(uint_t nbytes);

static bool start_insert(uint_t size);


///
///  @brief    Append to edit buffer. Similar to insert_edit(), but adds an
///            entire file to the buffer.
///
///  @returns  true if we can continue reading lines, else false (because we
///            encountered either an EOF or a FF).
///
////////////////////////////////////////////////////////////////////////////////

bool append_edit(struct ifile *ifile, bool single)
{
    assert(ifile != NULL);

    if (!start_insert(ifile->size))
    {
        return false;
    }

    int c;
    int next;
    uchar *p = eb.buf + eb.left;
    int ndelims = 0;

    // Read characters until end of file or end of page

    while ((c = getc(ifile->fp)) != EOF)
    {
        if (c == LF)
        {
            if (!ifile->LF)             // First LF?
            {
                ifile->LF = true;

                if (f.e3.smart)         // In smart mode?
                {
                    f.e3.CR_in  = false; // Terminate input lines with LF
                    f.e3.CR_out = false; // Terminate output lines with LF
                }
            }
        }
        else if (c == CR)               // Check for CR followed by LF
        {
            if ((next = fgetc(ifile->fp)) != LF)
            {
                ungetc(next, ifile->fp); // Save non-LF for next read
            }
            else if (!ifile->LF)        // First LF?
            {
                ifile->LF = true;

                if (f.e3.smart)         // In smart mode?
                {
                    f.e3.CR_in  = true; // Terminate input lines with CR/LF
                    f.e3.CR_out = true; // Terminate output lines with CR/LF
                }
            }

            // If input lines can be terminated with CR/LF, then we save
            // both characters; if they can only be terminated with LF,
            // then we ignore the CR.

            if (f.e3.CR_in)             // If CR/LF is okay, save CR here
            {
                *p++ = (uchar)c;
            }

            c = LF;                     // Now save the LF
        }
        else if (c == FF && !f.e3.nopage)
        {
            ++ndelims;

            f.ctrl_e = true;            // Flag FF, but don't store it

            break;
        }
        else if (c == NUL && !f.e3.keepNUL)
        {
            continue;
        }

        *p++ = (uchar)c;

        if (isdelim(c))
        {
            ++ndelims;

            if (single)                 // If just appending single line,
            {
                break;                  //  then we're done
            }
        }
    }

    eb.t.nlines += ndelims;

    uint_t nbytes = (uint_t)(p - (eb.buf + eb.left));

    if (nbytes != 0)
    {
        end_insert(nbytes);
    }

    return (c == EOF) ? false : true;
}


///
///  @brief    Change case of character at current position of dot. Since this
///            will never add or delete any delimiters, it won't affect our
///            line number, or the total number of lines in the buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void change_dot(int c)
{
    assert(isalpha(c));

    uint_t i = (uint_t)eb.t.dot;

    if (i >= eb.left)
    {
        i += eb.gap;
    }

    eb.buf[i] = eb.t.c = (uchar)c;

    f.e0.window = true;                 // Window refresh needed
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

    if (eb.t.dot == 0 && nbytes == eb.t.Z)    // Killing entire buffer?
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

            assert(nbytes <= eb.t.dot);

            if (f.e0.display)
            {
                int ndelims = 0;
                uchar *p = eb.buf + eb.left;

                for (int i = 0; i < nbytes; ++i)
                {
                    int c = *--p;

                    if (isdelim(c))
                    {
                        ++ndelims;
                    }
                }

                eb.t.nlines -= ndelims;
                eb.t.line -= ndelims;
            }

            assert((uint_t)nbytes <= eb.left);

            eb.left -= (uint_t)nbytes;
            eb.t.dot -= nbytes;         // Backwards delete affects dot

            eb.t.lastc  = read_edit(-1);
        }
        else                            // Deleting forward in [right]
        {
            assert(nbytes <= eb.t.Z - eb.t.dot);

            if (f.e0.display)
            {
                int ndelims = 0;
                uchar *p = eb.buf + eb.left + eb.gap;

                for (int i = 0; i < nbytes; ++i)
                {
                    int c = *p++;

                    if (isdelim(c))
                    {
                        ++ndelims;
                    }
                }

                eb.t.nlines -= ndelims;
            }

            assert((uint_t)nbytes <= eb.right);

            eb.right -= (uint_t)nbytes;

            eb.t.c     = read_edit(0);
            eb.t.nextc = read_edit(1);
        }

        eb.gap += (uint_t)nbytes;       // Increase the gap
        eb.t.Z -= nbytes;               //  and decrease the total

        int_t prev = prev_line(0);      // Position of start of line

        eb.t.pos = eb.t.dot - prev;
        eb.t.len = next_line(1) - prev;

        f.e0.window = true;             // Window refresh needed
    }
}


///
///  @brief    Finish insertion into buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void end_insert(uint_t nbytes)
{
    assert(nbytes != 0);

    // Now fix up some variables

    eb.left  += nbytes;
    eb.gap   -= nbytes;
    eb.t.dot += (int_t)nbytes;
    eb.t.Z   += (int_t)nbytes;

    int_t prev = prev_line(0);          // Position of start of line

    eb.t.pos  = eb.t.dot - prev;
    eb.t.len  = next_line(1) - prev;

    if (eb.t.dot == 0)
    {
        eb.t.lastc = EOF;
    }
    else
    {
        eb.t.lastc = eb.buf[eb.left - 1];
    }

    eb.t.c = eb.buf[eb.left];

    //eb.t.nextc = ...                  // Next character doesn't change

    if (eb.t.Z != 0 && page_count() == 0)
    {
        set_page(1);
    }

    f.e0.window = true;                 // Window refresh needed
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
///  @brief    Insert string in edit buffer.
///
///  @returns  true if insert succeeded, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool insert_edit(const char *buf, size_t nbytes)
{
    assert(buf != NULL);
    assert(eb.buf != NULL);             // Error if no edit buffer

    if (!start_insert((uint_t)nbytes))
    {
        return false;
    }

    memcpy(eb.buf + eb.left, buf, nbytes);

    if (f.e0.display)
    {
        int ndelims = 0;
        const char *p = buf;

        for (uint i = 0; i < nbytes; ++i)
        {
            int c = *p++;

            if (isdelim(c))
            {
                ++ndelims;
            }
        }

        eb.t.nlines += ndelims;
    }

    end_insert((uint_t)nbytes);

    return true;                        // Insertion was successful
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

        f.e0.window = true;             // Window refresh needed
    }
}


///
///  @brief    Return number of bytes between dot and nth line terminator.
///
///  @returns  Number of characters relative to dot (can be plus or minus).
///
////////////////////////////////////////////////////////////////////////////////

int_t len_edit(int_t n)
{
    if (n > 0)
    {
        return next_line((uint_t)n) - eb.t.dot;
    }
    else
    {
        return prev_line((uint_t)-n) - eb.t.dot;
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
///  @brief    Scan forward nlines in edit buffer.
///
///  @returns  Position following line terminator (relative to dot).
///
////////////////////////////////////////////////////////////////////////////////

static int_t next_line(uint_t nlines)
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

    // There aren't n lines following the current position, so just return Z.

    return eb.t.Z;
}


///
///  @brief    Scan backward n lines in edit buffer.
///
///  @returns  Position following line terminator (relative to dot).
///
////////////////////////////////////////////////////////////////////////////////

static int_t prev_line(uint_t nlines)
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

    // There aren't n lines preceding the current position, so just return B.

    return 0;
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
    eb.t.nextc  = EOF;
    eb.t.c      = EOF;
    eb.t.lastc  = EOF;
    eb.t.len    = 0;
    eb.t.pos    = 0;
    eb.t.line   = 0;
    eb.t.nlines = 0;
}


///
///  @brief    Move dot to an absolute position.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_dot(int_t dot)
{
    if (dot < eb.t.B)
    {
        dot = eb.t.B;                   // Can't move before start of buffer
    }
    else if (dot > eb.t.Z)
    {
        dot = eb.t.Z;                   // Can't move after end of buffer
    }

    if (eb.t.dot == dot)
    {
        return;                         // Nothing to do if no change
    }

    // Here if position within edit buffer has changed.

    f.e0.cursor = true;                 // Tell display to update cursor

    if (dot == eb.t.B)                  // Moving to start of buffer
    {
        eb.t.dot   = dot;
        eb.t.lastc = EOF;
        eb.t.c     = read_edit(0);
        eb.t.nextc = read_edit(1);
        eb.t.pos   = 0;
        eb.t.len   = next_line(1);
        eb.t.line  = 0;
    }
    else if (dot == eb.t.Z)             // Moving to end of buffer
    {
        eb.t.dot   = dot;
        eb.t.lastc = read_edit(-1);
        eb.t.c     = EOF;
        eb.t.nextc = EOF;
        eb.t.pos   = eb.t.dot - prev_line(0);
        eb.t.len   = eb.t.pos;
        eb.t.line  = eb.t.nlines;
    }
    else
    {
        int delta = dot - eb.t.dot;     // How much are we moving?

        if (delta == 1)                 // Moving one character forward?
        {
            ++eb.t.dot;

            eb.t.lastc = eb.t.c;
            eb.t.c     = eb.t.nextc;
            eb.t.nextc = read_edit(1);

            if (isdelim(eb.t.lastc))    // Moving to next line?
            {
                eb.t.pos = 0;
                eb.t.len = next_line(1) - prev_line(0);

                ++eb.t.line;
            }
            else
            {
                ++eb.t.pos;
            }
        }
        else if (delta == -1)           // Moving one character backward?
        {
            --eb.t.dot;

            eb.t.nextc = eb.t.c;
            eb.t.c     = eb.t.lastc;
            eb.t.lastc = read_edit(-1);

            if (isdelim(eb.t.c))        // Moving to previous line?
            {
                eb.t.pos = eb.t.dot - prev_line(0);
                eb.t.len = eb.t.pos + 1;

                --eb.t.line;
            }
            else
            {
                --eb.t.pos;
            }
        }
        else                            // Moving more than one character
        {
            eb.t.dot   = dot;
            eb.t.lastc = read_edit(-1);
            eb.t.c     = read_edit(0);
            eb.t.nextc = read_edit(1);
            eb.t.pos   += delta;

            //  If we moved to a new line, recalculate line position and length.

            if (eb.t.pos < 0 || eb.t.pos >= eb.t.len)
            {
                int_t prev = prev_line(0);

                eb.t.pos = eb.t.dot - prev;
                eb.t.len = next_line(1) - prev;

                if (f.e0.display)
                {
                    if (delta < 0)
                    {
                        for (int i = 0; i < -delta; ++i)
                        {
                            int c = read_edit(i);

                            if (isdelim(c))
                            {
                                --eb.t.line;
                            }
                        }
                    }
                    else
                    {
                        for (int i = -delta; i < 0; ++i)
                        {
                            int c = read_edit(i);

                            if (isdelim(c))
                            {
                                ++eb.t.line;
                            }
                        }
                    }
                }
            }
        }
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


///
///  @brief    Initialize buffer for adding characters.
///
///  @returns  true if initialized succeeded, false if it didn't.
///
////////////////////////////////////////////////////////////////////////////////

static bool start_insert(uint_t nbytes)
{
    if (nbytes == 0)
    {
        return false;
    }

    // Make sure data can fit in the space we have. If not, increase by 50%.

    while (eb.gap < nbytes)
    {
        uint_t size = (eb.t.size * 3) / 2;

        if (size_edit(size) == 0)
        {
            return false;
        }

        print_size(size);
    }

    // Ensure dot is at start of the gap

    uint_t dot = (uint_t)eb.t.dot;

    if (dot < eb.left)
    {
        shift_right(eb.left - dot);
    }
    else if (dot > eb.left)
    {
        shift_left(dot - eb.left);
    }

    return true;
}
