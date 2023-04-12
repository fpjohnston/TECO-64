///
///  @file    build_str.c
///  @brief   Build a search string.
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
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errcodes.h"
#include "file.h"
#include "qreg.h"


#define BUILD_MAX       (PATH_MAX)      ///< Maximum build string length


// Local functions

static int getc_src(const char *src, uint_t len, int error);

static char putc_dest(uint_t pos, int c);



///
///  @brief    Build a string, allowing for the following special characters
///            (these may be specified as ^x, unless f.ed.caret is set, or as
///            literal control characters):
///
///            ^Q   - Use next character literally.
///            ^R   - Same as CTRL/Q.
///            ^V   - Convert next character to lower case.
///            ^V^V - Convert all characters to lower case until end of string
///            ^W   - Convert next character to upper case.
///            ^W^W - Convert all characters to upper case until end of string
///                   or ^V^V.
///            ^EQq - Insert string from Q-register q.
///            ^EUq - Insert character whose ASCII code is the same as that
///                   which would be returned by Qq.
///
///  @returns  TECO string.
///
////////////////////////////////////////////////////////////////////////////////

tstring build_string(const char *src, uint_t len)
{
    assert(src != NULL);                // Error if no source string

    char string[BUILD_MAX];             // Allow 4K buffer for build string
    char *next = string;                // Address to store next character
    uint_t pos = 0;                     // No. of characters output
    bool lower_next = false;            // Convert next chr. to lower case
    bool upper_next = false;            // Convert next chr. to upper case
    bool lower_all = false;             // Convert all chrs. to lower case
    bool upper_all = false;             // Convert all chrs. to upper case

    // Loop for all characters in source string.

    while (len-- > 0)
    {
        int c = *src++;

        // If allowed, convert ^x to CTRL/x

        if (c == '^' && !f.ed.caret)
        {
            c = getc_src(src, len, E_ISS);
            ++src, --len;

            int c1 = toupper(c);

            if (c1 < '@' || c1 > '_')
            {
                throw(E_IUC, c);        // Invalid uparrow character
            }

            c &= 0x1f;                  // Convert to control character
        }

        // If we don't have any special string building characters, just
        // take care of those first.

        if (!iscntrl(c))
        {
            // Note that we check for lower_next and upper_next before
            // lower_all and upper_all because CTRL/V can be used to
            // temporarily override the case set by a double CTRL/W,
            // and the same is true of a CTRL/W and a double CTRL/V.
            // So the code below really can't be simplified.

            if (lower_next)
            {
                c = tolower(c);
            }
            else if (upper_next)
            {
                c = toupper(c);
            }
            else if (lower_all || f.e0.lower)
            {
                c = tolower(c);
            }
            else if (upper_all || f.e0.upper)
            {
                c = toupper(c);
            }

            lower_next = upper_next = false;

            *next++ = putc_dest(pos++, c);
        }
        else if (c == CTRL_E)
        {
            int qname;
            bool qlocal = false;
            int qindex;
            struct qreg *qreg;

            c = getc_src(src, len, E_ISS); // Get character after CTRL/E
            ++src, --len;

            switch (toupper(c))
            {
                case 'Q':               // <CTRL/E>Q
                    qname = getc_src(src, len, E_MQN);
                     ++src, --len;

                    if (qname == '*')
                    {
                        uint_t nbytes = (uint_t)strlen(last_file);

                        if ((uint_t)(uint)(next - string) + nbytes == BUILD_MAX)
                        {
                            throw(E_MAX); // Internal program limit reached
                        }

                        memcpy(next, last_file, (size_t)nbytes);

                        next += nbytes, pos += nbytes;

                        break;
                    }

                    if (qname == '.')
                    {
                        qlocal = true;
                        qname = getc_src(src, len, E_MQN);
                        ++src, --len;
                    }

                    if ((qindex = get_qindex(qname, qlocal)) == -1)
                    {
                        throw(E_IQN, qname);
                    }

                    qreg = get_qreg(qindex);

                    if (qreg->text.len != 0)
                    {
                        uint_t nbytes = qreg->text.len;

                        if ((uint_t)(uint)(next - string) + nbytes == BUILD_MAX)
                        {
                            throw(E_MAX); // Internal program limit reached
                        }

                        memcpy(next, qreg->text.data, (size_t)nbytes);

                        next += nbytes, pos += nbytes;
                    }

                    break;

                case 'U':               // <CTRL/E>U
                    qname = getc_src(src, len, E_MQN);
                    ++src, --len;

                    if (qname == '.')
                    {
                        qlocal = true;
                        qname = getc_src(src, len, E_MQN);
                        ++src, --len;
                    }

                    if ((qindex = get_qindex(qname, qlocal)) == -1)
                    {
                        throw(E_IQN, qname);
                    }

                    qreg = get_qreg(qindex);

                    *next++ = putc_dest(pos++, (int)qreg->n);

                    break;

                default:                // Not a special string building chr.
                    *next++ = putc_dest(pos++, CTRL_E);
                    --src, ++len;       // Re-do the next character

                    break;
            }
        }
        else if (c == CTRL_Q || c == CTRL_R)
        {
            c = getc_src(src, len, E_ISS);
            ++src, --len;
            *next++ = putc_dest(pos++, c);
        }
        else if (c == CTRL_V)
        {
            if (lower_next)             // <CTRL/V><CTRL/V>?
            {
                lower_all = true;
                upper_all = lower_next = upper_next = false;
            }
            else
            {
                lower_next = true;
            }
        }
        else if (c == CTRL_W)
        {
            if (upper_next)             // <CTRL/W><CTRL/W>?
            {
                upper_all = true;
                lower_all = upper_next = lower_next = false;
            }
            else
            {
                upper_next = true;
            }
        }
        else                            // Non-special control character
        {
            lower_next = upper_next = false;

            *next++ = putc_dest(pos++, c);
        }
    }

    // We terminate the string with a NUL character for the benefit of file
    // functions that use the string for C library functions, but we don't
    // want to count the NUL if we were called by any search functions.

    *next = NUL;                        // Ensure it's NUL-terminated

    // Copy result to scratch buffer

    tstring result = { .data = scratch, .len = pos };

    memcpy(result.data, string, (size_t)result.len + 1);

    return result;
}


///
///  @brief    Build a trimmed string (with leading and trailing whitespace
///            removed).
///
///  @returns  Pointer to string.
///
////////////////////////////////////////////////////////////////////////////////

const char *build_trimmed(const char *src, uint_t len)
{
    tstring result = build_string(src, len);
    char *p = result.data;
    const char *white = "\t\n\v\f\r ";  // Characters to trim

    p += strspn(p, white);              // Skip past leading whitespace
    len = (uint_t)strcspn(p, white);    // Find start of trailing whitespace
    p[len] = NUL;                       // And trim it

    return p;
}


///
///  @brief    Get next character from source buffer (error if buffer empty).
///
///  @returns  Next character.
///
////////////////////////////////////////////////////////////////////////////////

static int getc_src(const char *src, uint_t len, int error)
{
    assert(src != NULL);

    if (len == 0)
    {
        throw(error);
    }

    return *src;
}


///
// / @brief  Put next character to destination buffer (error if buffer full).
///
///  @returns  Next character.
///
////////////////////////////////////////////////////////////////////////////////

static char putc_dest(uint_t pos, int c)
{
    if (pos == BUILD_MAX - 1)
    {
        throw(E_MAX);
    }

    return (char)c;
}
