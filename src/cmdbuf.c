///
///  @file    cmdbuf.c
///  @brief   Functions that affect command buffer.
///
///  @author  Nowwith Treble Software
///
///  @bug     No known bugs.
///
///  @copyright  tbd
///
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files (the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
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

// Define command buffer. This is dynamically allocated, and can be resized as
// necessary. It consists of a pointer to a memory block, the size of the block
// in bytes, and two counters, one for storing new characters, and one for
// removing characters as the buffer is read.

static struct
{
    char *p;            // Start of memory block
    uint size;          // Size of memory block
    uint put;           // Index of next character to store
    uint get;           // Index of next character to fetch
} buf =
{
    .p    = NULL,
    .size = 0,
    .put  = 0,
    .get  = 0,
};


// Local functions

static void free_cmd(void);


///
///  @brief    Search for next argument in command and return a copy to it.
///
///  @returns  New string.
///
////////////////////////////////////////////////////////////////////////////////

char *clone_cmd(int delim)
{
    if (buf.p == NULL)
    {
        return NULL;
    }

    const char *start = buf.p + buf.get;
    int len = 0;
    int c;

    while ((c = fetch_cmd()) != delim)
    {
        if (c == EOF)
        {
            return NULL;
        }

        ++len;
    }

    char *p = malloc(len + 1);

    assert(p != NULL);

    memcpy(p, start, len);

    p[len] = NUL;

    return p;
}


///
///  @brief    Copy command to new string and return it.
///
///  @returns  New string.
///
////////////////////////////////////////////////////////////////////////////////

char *copy_cmd(void)
{
    assert(buf.size != 0);

    char *p = malloc((ulong)buf.size + 1);

    assert(p != NULL);

    memcpy(p, buf.p, (ulong)buf.size + 1);

    p[buf.size] = NUL;
   
    return p;
}


///
///  @brief    Return number of characters in command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

int count_cmd(void)
{
    return buf.put - buf.get;
}


///
///  @brief    Delete last character from command buffer and return it.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

int delete_cmd(void)
{
    if (buf.put == 0)                   // Anything in buffer?
    {
        return EOF;                     // No
    }

    return buf.p[--buf.put];            // Delete character and return it
}


///
///  @brief    Echo all characters in command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void echo_cmd(int pos)
{
    assert((uint)pos <= buf.put);

    // Just echo everything we're supposed to print. Note that this is not the
    // same as typing out what's in a buffer, so things such as the settings of
    // the EU flag don't matter here.

    for (uint i = (uint)pos; i < buf.put; ++i)
    {
        echo_chr(buf.p[i]);
    }
}


///
///  @brief    Determine if command buffer is empty.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

bool empty_cmd(void)
{
    return ((buf.put - buf.get) == 0);
}


///
///  @brief    Fetch next character from command string buffer.
///
///  @returns  Character fetched.
///
////////////////////////////////////////////////////////////////////////////////

int fetch_cmd(void)
{
    if (buf.get == buf.put)
    {
        buf.get = buf.put = 0;

        return EOF;
    }

    return buf.p[buf.get++];
}


///
///  @brief    Free up memory on exit.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void free_cmd(void)
{
    print_term("<Command buffer deallocated>");

    if (buf.p != NULL)
    {
        free(buf.p);

        buf.p = NULL;
        buf.size = buf.get = buf.put = 0;
    }
}


///
///  @brief    Initialize command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_cmd(void)
{
    assert(buf.size == 0);              // Allow only one call

    buf.size = CMDBUFSIZ;

    if ((buf.p = malloc(buf.size)) == NULL)
    {
        exit(EXIT_FAILURE);             // Clean up, reset, and exit
    }

    buf.get = buf.put = 0;

    if (atexit(free_cmd) != 0)          // Ensure we clean up on exit
    {
        exit(EXIT_FAILURE);
    }
}


///
///  @brief    Get last character from command buffer (without removing it).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

int last_cmd(void)
{
    if ((buf.put - buf.get) == 0)       // Anything in buffer?
    {
        return EOF;                     // No, treat it as end of file
    }

    return buf.p[buf.put - 1];          // Return last character in buffer
}


///
///  @brief    See if beginning of command matches passed string.
///
///  @returns  true if match, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool match_cmd(const char *str)
{
    assert(str != NULL);

    uint len = strlen(str);

    if (strncasecmp(buf.p, str, len))
    {
        return false;
    }
    else
    {
        return true;
    }
}


///
///  @brief    Get pointer to next character in command bufffer.
///
///  @returns  Pointer to next character.
///
////////////////////////////////////////////////////////////////////////////////

const char *next_cmd(void)
{
    return buf.p + buf.get;
}


///
///  @brief    Reset command buffer (discard any saved characters).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_cmd(void)
{
    buf.get = buf.put = 0;
}


///
///  @brief    Scan command buffer for end of string.
///
///  @returns  Length and start of string.
///
////////////////////////////////////////////////////////////////////////////////

int scan_cmd(int delim)
{
    if (f.ei.atsign)                    // Modified by @?
    {
        if ((delim = fetch_cmd()) == EOF) // Yes, get alternate delimiter
        {
            return EOF;
        }
    }

    int c;
    int len = 0;

    while ((c = fetch_cmd()) != delim)
    {
        if (c == EOF)
        {
            return EOF;
        }

        ++len;
    }

    return len;
}


///
///  @brief    Get start of line.
///
///  @returns  Index of start of line.
///
////////////////////////////////////////////////////////////////////////////////

int start_cmd(void)
{
    int i = buf.put;

    while (i > 0)
    {
        // Back up on line until we find a line terminator.

        int c = buf.p[i];

        if (c == LF || c == VT || c == FF)
        {
            break;
        }

        --i;
    }

    return i;                           // Return index of start of line
}


///
///  @brief    Store new character in command string buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_cmd(int c)
{
    // If we run out of room for the command string, try to increase it by
    // calling realloc(). Note that this may move the block, so we have to
    // reinitialize all of our pointers.

    if (buf.put == buf.size)            // Has buffer filled up?
    {
        // If size is 0, then init_cmd() hasn't been called yet. realloc() will
        // function like malloc() if its first argument is zero, but it's a bad
        // coding practice, so we don't allow it.

        assert(buf.size != 0);

        int newsize = buf.size + CMDBUFSIZ;
        char *newp = realloc(buf.p, newsize);

        if (newp == NULL)               // Can't get more memory
        {
            exit(EXIT_FAILURE);         // Clean up, reset, and exit
        }

        buf.size = newsize;
        buf.p  = newp;
    }

    buf.p[buf.put++] = c;
    buf.p[buf.put] = NUL;
}


///
///  @brief    Type out command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void type_cmd(const char *p, int len)
{
    assert(p != NULL);

    while (len-- > 0)
    {
        int c = *p++;                   // Next character to output

#if    0                                // TODO: should this be here?

        // If EU flag is 0 and lower case, flag character with quote.
        // If EU flag is 1 and upper case, flag character with quote.
        // In either situation, always convert to upper case.

        if (   (f.eu == 0 && islower(c))
            || (f.eu == 1 && isupper(c)))
        {
            putc_term('\'');
            putc_term(toupper(c));

            continue;
        }
#endif
        
        echo_chr(c);
    }
}


///
///  @brief    Puts character back at beginning of command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void unfetch_cmd(int c)
{
    if (buf.get != 0)
    {
        buf.p[--buf.get] = c;
    }
}
