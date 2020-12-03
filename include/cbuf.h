///
///  @file    cbuf.h
///  @brief   Header file for command buffer definitions.
///
///  @copyright 2020 Franklin P. Johnston / Nowwith Treble Software
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

#if     !defined(_CBUF_H)

#define _CBUF_H

#include <stdio.h>              //lint !e451 !e537


// Command buffer variable

extern struct buffer *cbuf;

// Command buffer functions

extern noreturn void abort_cbuf(void);

extern void init_cbuf(void);

extern void reset_cbuf(void);

extern void store_cbuf(int c);


// *** Note that the following functions are inline as an optimization. ***


///
///  @brief    Fetch next character from command string.
///
///  @returns  Next character, or EOF if at end of string.
///
////////////////////////////////////////////////////////////////////////////////

static inline int fetch_cbuf(void)
{
    if (cbuf->pos == cbuf->len)
    {
        return EOF;
    }

    return cbuf->data[cbuf->pos++];
}


///
///  @brief    Skip past current current in command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static inline void next_cbuf(void)
{
    if (cbuf->pos < cbuf->len)
    {
        ++cbuf->pos;
    }
}


///
///  @brief    Peek at next character in command string.
///
///  @returns  Next character, or EOF if at end of string.
///
////////////////////////////////////////////////////////////////////////////////

static inline int peek_cbuf(void)
{
    if (cbuf->pos == cbuf->len)
    {
        return EOF;
    }

    return cbuf->data[cbuf->pos];
}    


///
///  @brief    Fetch next character from command string (like fetch_cbuf(), but
///            throws exception if no more characters).
///
///  @returns  Next character, or error if at end of string.
///
////////////////////////////////////////////////////////////////////////////////

static inline int require_cbuf(void)
{
    if (cbuf->pos == cbuf->len)
    {
        abort_cbuf();
    }

    return cbuf->data[cbuf->pos++];
}


#endif  // !defined(_CBUF_H)
