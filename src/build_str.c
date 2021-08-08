///
///  @file    build_str.c
///  @brief   Build a search string.
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

/// @def    getc_src
/// @brief  Get next character from source buffer (error if buffer empty).

#define getc_src(chr, error) if (len-- == 0) throw(error); else chr = *src++

/// @def    putc_dest
/// @brief  Put next character to destination buffer (error if buffer full).

#define putc_dest(c) if (pos == BUILD_MAX - 1) throw(E_MEM); else \
                     string[pos++] = (char)c


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
    uint_t pos = 0;                     // Position to store next character
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
            getc_src(c, E_ISS);

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

            putc_dest(c);
        }
        else if (c == CTRL_E)
        {
            int qname;
            bool qlocal = false;
            int qindex;
            struct qreg *qreg;

            getc_src(c, E_ISS);         // Get character after CTRL/E

            switch (toupper(c))
            {
                case 'Q':               // <CTRL/E>Q
                    getc_src(qname, E_MQN);

                    if (qname == '*')
                    {
                        uint_t nbytes = (uint_t)strlen(last_file);

                        if (pos + nbytes == BUILD_MAX) 
                        {
                            throw(E_MEM);
                        }

                        memcpy(string + pos, src, (size_t)nbytes);
                        pos += nbytes;

                        break;
                    }

                    if (qname == '.')
                    {
                        getc_src(qname, E_MQN);
                        qlocal = true;
                    }

                    if ((qindex = get_qindex(qname, qlocal)) == -1)
                    {
                        throw(E_IQN, qname);
                    }

                    qreg = get_qreg(qindex);

                    if (qreg->text.len != 0)
                    {
                        uint_t nbytes = qreg->text.len;

                        if (pos + nbytes == BUILD_MAX) 
                        {
                            throw(E_MEM);
                        }

                        memcpy(string + pos, qreg->text.data, (size_t)nbytes);
                        pos += nbytes;
                    }

                    break;

                case 'U':               // <CTRL/E>U
                    getc_src(qname, E_MQN);

                    if (qname == '.')
                    {
                        getc_src(qname, E_MQN);
                        qlocal = true;
                    }

                    if ((qindex = get_qindex(qname, qlocal)) == -1)
                    {
                        throw(E_IQN, qname);
                    }

                    qreg = get_qreg(qindex);

                    putc_dest(qreg->n);

                    break;

                default:                // Not a special string building chr.
                    putc_dest(CTRL_E);  // Just save the CTRL/E

                    ++len;              // And redo the next character
                    --src;

                    break;
            }
        }
        else if (c == CTRL_Q || c == CTRL_R)
        {
            getc_src(c, E_ISS);
            putc_dest(c);
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

            putc_dest(c);
        }
    }

    // We terminate the string with a NUL character for the benefit of file
    // functions that use the string for C library functions, but we don't
    // want to count the NUL if we were called by any search functions.

    string[pos] = NUL;                  // Ensure it's NUL-terminated

    // Copy result to scratch buffer

    memcpy(scratch, string, (size_t)pos + 1);

    tstring result = { .data = scratch, .len = pos };

    last_len = pos;

    return result;
}
