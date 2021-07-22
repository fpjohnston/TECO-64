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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errcodes.h"
#include "file.h"
#include "qreg.h"


#define BUILD_INIT      128             ///< Initial size of string

#define BUILD_EXPAND    128             ///< Incremental size of string

static char *build_str;                 ///< Build string

static uint_t build_size;               ///< Allocated size of build string

static uint_t build_len;                ///< Current length of build string

// Local functions

static void store_chr(int c);

static void store_str(const char *buf, uint_t len);


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
///  @returns  No. of characters in dynamically-allocated string.
///
////////////////////////////////////////////////////////////////////////////////

uint_t build_string(char **dest, const char *src, uint_t len)
{
    assert(dest != NULL);               // Error if no destination string
    assert(src != NULL);                // Error if no source string

    bool literal    = false;
    bool lower_next = false;
    bool upper_next = false;

    build_str = alloc_mem(build_size = BUILD_INIT);
    build_len = 0;

    while (len-- > 0)
    {
        int c = *src++;

        if (literal)
        {
            literal = false;

            store_chr((int)c);

            continue;
        }

        // If allowed, convert ^x to CTRL/x

        if (c == '^' && !f.ed.caret)
        {
            if (len-- == 0)
            {
                throw(E_ISS);           // Invalid search string
            }

            int orig = *src++;

            c = toupper(orig);

            if (c < '@' || c > '_')
            {
                throw(E_IUC, orig);     // Invalid uparrow character
            }

            c &= 0x1f;
        }

        if (c == CTRL_Q || c == CTRL_R)
        {
            literal = true;

            continue;
        }

        if (c == CTRL_V)
        {
            if (lower_next)             // <CTRL/V><CTRL/V>?
            {
                f.e0.lower = true;
                f.e0.upper = false;

                lower_next = upper_next = false;
            }
            else
            {
                lower_next = true;
            }

            continue;
        }

        if (c == CTRL_W)
        {
            if (upper_next)             // <CTRL/W><CTRL/W>?
            {
                f.e0.upper = true;
                f.e0.lower = false;

                upper_next = lower_next = false;
            }
            else
            {
                upper_next = true;
            }

            continue;
        }

        // Check for <CTRL/E>Qq and <CTRL/E>Uq

        if (c == CTRL_E)
        {
            if (len-- == 0)             // Any more characters?
            {
                throw(E_ISS);           // Invalid search string
            }

            int orig = *src++;

            c = toupper(orig);

            // If neither <CTRL/E>Q nor <CTRL/E>U, then it's probably a match
            // control construct; in any case, just copy it and continue.

            if (c != 'Q' && c != 'U')
            {
                store_chr(CTRL_E);
                store_chr(orig);

                continue;
            }

            // Here for <CTRL/E>Q or <CTRL/E>U

            if (len-- == 0)             // Q-register specified?
            {
                throw(E_MQN);           // Missing Q-register name
            }

            int qname = *src++;
            bool qlocal = false;

            if (qname == '*' && c == 'Q')
            {
                store_str(last_file, (uint_t)strlen(last_file));

                continue;
            }

            if (qname == '.')           // Local Q-register?
            {
                qlocal = true;

                if (len-- == 0)         // Q-register specified?
                {
                    throw(E_MQN);       // Missing Q-register name
                }

                qname = *src++;
            }

            int qindex = get_qindex(qname, qlocal);

            if (qindex == -1)
            {
                throw(E_IQN, qname);    // Invalid Q-register name
            }

            struct qreg *qreg = get_qreg(qindex);

            if (c == 'Q')               // <CTRL/E>Qq
            {
                if (qreg->text.len != 0)
                {
                    store_str(qreg->text.data, qreg->text.len);
                }
            }
            else if (c == 'U')          // <CTRL/E>Uq
            {
                store_chr((int)qreg->n);
            }

            continue;
        }

        // Note that we check for lower_next and upper_next before lower_all and
        // upper_all because CTRL/V can be used to temporarily override the case
        // set by a double CTRL/W, and the same is true of a CTRL/W and a double
        // CTRL/V. So the code below really can't be simplified.

        if (lower_next)
        {
            c = tolower(c);
        }
        else if (upper_next)
        {
            c = toupper(c);
        }
        else if (f.e0.lower)
        {
            c = tolower(c);
        }
        else if (f.e0.upper)
        {
            c = toupper(c);
        }

        lower_next = upper_next = false;

        store_chr(c);
    }

    // We terminate the string with a NUL character for the benefit of file
    // functions that use the string for C library functions, but we don't
    // want to count the NUL if we were called by any search functions.

    store_chr(NUL);                     // Ensure it's NUL-terminated

    --build_len;                        // But don't count the NUL

    *dest = build_str;

    last_len = build_len;

    return build_len;
}


///
///  @brief    Store a character in build string. Note that this may result in
///            reallocating memory for a larger build string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void store_chr(int c)
{
    if (build_len == build_size)
    {
        uint_t oldsize = build_size;

        build_size += BUILD_EXPAND;

        build_str = expand_mem(build_str, oldsize, build_size);
    }

    build_str[build_len++] = (char)c;
}


///
///  @brief    Copy a string to build string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void store_str(const char *buf, uint_t len)
{
    assert(buf != NULL);                // Error if no input string

    while (len-- > 0)
    {
        store_chr(*buf++);
    }
}
