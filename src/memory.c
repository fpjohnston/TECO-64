///
///  @file    memory.c
///  @brief   Memory allocation & deallocation functions.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "errcodes.h"
#include "exec.h"
#include "term.h"


// The following conditional code is used to check for memory leaks when we
// exit. It is an early warning system to alert the user that there is a bug
// that needs to be investigated and resolved, possibly with better tools such
// as Valgrind.

#if     defined(MEMCHECK)

#define plural(x) ((x) == 1) ? "" : "s" ///< Check for plural/non-plural no.

///  @struct mblock
///
///  This structure defines a block that is used in a doubly-linked list to
///  keep track of TECO memory allocations and deallocations. The mroot vari-
///  able points to the first block in the list, which is also always the most
///  recently added block. And the msize variable is the sum of the sizes of
///  all currently allocated memory (excluding the blocks for this list). At
///  program exit, the list should be empty, but if it is not, we will use the
///  information in each block to print an error message with the address and
///  size of the undeallocated memory.

struct mblock
{
    struct mblock *prev;                ///< Previous block in linked list
    struct mblock *next;                ///< Next block in linked list
    const char *addr;                   ///< calloc'd memory block
    unsigned int size;                  ///< Size of block in bytes
};

static struct mblock *mroot = NULL;     ///< Root of memory block list

static unsigned int msize = 0;          ///< Total memory allocated, in bytes

static unsigned int nblocks = 0;        ///< No. of blocks currently allocated

static unsigned int maxblocks = 0;      ///< High-water mark for allocated blocks

// Local functions

static void add_mblock(void *p1, uint size);

static void delete_mblock(void *p1);

#endif


///
///  @brief    Add memory block.
///
///  @returns  Nothing (error if memory allocation fails).
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(MEMCHECK)

static void add_mblock(void *p1, uint size)
{
    assert(p1 != NULL);                 // Error if no memory block

    // Note: We don't call alloc_mem() here, since it calls us.

    struct mblock *mblock = calloc(1ul, sizeof(*mblock));

    assert(mblock != NULL);             // Error if calloc() failed

    mblock->prev = NULL;
    mblock->next = mroot;

    if (mroot != NULL)
    {
        mroot->prev = mblock;
    }

    mblock->addr = p1;
    mblock->size = size;
    msize += size;
    mroot = mblock;

    if (maxblocks < ++nblocks)
    {
        maxblocks = nblocks;
    }
}

#endif


///
///  @brief    Get new memory.
///
///  @returns  Pointer to new memory.
///
////////////////////////////////////////////////////////////////////////////////

void *alloc_mem(uint size)
{
    void *p1 = calloc(1uL, (size_t)size);

    if (p1 == NULL)
    {
        throw(E_MEM);                   // Memory overflow
    }

#if     defined(MEMCHECK)

    add_mblock(p1, size);

#endif

    return p1;
}


///
///  @brief    Delete memory block.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(MEMCHECK)

static void delete_mblock(void *p1)
{
    assert(p1 != NULL);                 // Error if NULL memory block

    struct mblock *p = mroot;

    while (p != NULL)
    {
        if (p1 == p->addr)
        {
            msize -= p->size;

            if (p->prev != NULL)
            {
                p->prev->next = p->next;
            }
            else
            {
                mroot = p->next;
            }

            if (p->next != NULL)
            {
                p->next->prev = p->prev;
            }

            p->next = p->prev = NULL;
            p->addr = NULL;
            p->size = 0;

            // Note: We don't call free_mem() here, since it calls us.

            free(p);

            --nblocks;

            return;
        }

        p = p->next;
    }

    tprint("?Can't find memory block: %p\r\n", p1);
}

#endif


///
///  @brief    Verify that all memory was deallocated before we exit from TECO.
///
///  @returns  Nothing (error if memory allocation fails).
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(MEMCHECK)

void exit_mem(void)
{
    // We free this memory here because exit_EG() has to be the last function
    // called before exiting TECO, which means we have to run our memory check
    // before that, and if we didn't free it here, then the code below would
    // assume that there's a memory leak.

    free_mem(&eg_result);

    tprint("%s(): maximum of %u allocated block%s\r\n", __func__,
           maxblocks, plural(maxblocks));

    struct mblock *p = mroot;
    struct mblock *next;

    if (msize != 0)
    {
        tprint("%s(): not deallocated: %u total byte%s in %u block%s\r\n",
               __func__, msize, plural(msize), nblocks, plural(nblocks));
    }

    uint i = 0;

    while (p != NULL)
    {
        tprint("%s(): allocation #%u at %p, %u byte%s\r\n", __func__, ++i,
               p->addr, p->size, plural(p->size));

        next = p->next;
        msize -= p->size;
        p->next = p->prev = NULL;
        p->addr = NULL;
        p->size = 0;

        free(p);

        p = next;
    }
}

#endif


///
///  @brief    Get more memory.
///
///  @returns  Nothing (error if memory allocation fails).
///
////////////////////////////////////////////////////////////////////////////////

void *expand_mem(void *p1, uint oldsize, uint newsize)
{
    assert(p1 != NULL);                 // Error if NULL memory block
    assert(oldsize != 0);               // Error if old size is 0
    assert(oldsize != newsize);         // Error if size didn't change
    assert(oldsize < newsize);          // Error if trying to make smaller

#if     defined(MEMCHECK)

    delete_mblock(p1);

#endif

    char *p2 = realloc(p1, (size_t)newsize);

    if (p2 == NULL)
    {
        throw(E_MEM);                   // Memory overflow
    }

#if     defined(MEMCHECK)

    add_mblock(p2, newsize);

#endif

    // Initialize the extra memory we just allocated.

    memset(p2 + oldsize, '\0', (size_t)(newsize - oldsize));

    return p2;
}


///
///  @brief    Deallocate memory.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void free_mem(void *p1)
{
    assert(p1 != NULL);                 // Error if NULL pointer

    char **p2 = p1;                     // Make it something we can dereference

    if (*p2 != NULL)
    {
#if     defined(MEMCHECK)

        delete_mblock(*p2);

#endif

        free(*p2);

        *p2 = NULL;                     // Make sure we don't use this again
    }
}


///
///  @brief    Shrink memory.
///
///  @returns  Nothing (error if memory allocation fails).
///
////////////////////////////////////////////////////////////////////////////////

void *shrink_mem(void *p1, uint oldsize, uint newsize)
{
    assert(p1 != NULL);                 // Error if NULL memory block
    assert(oldsize != newsize);         // Error if size didn't change
    assert(oldsize > newsize);          // Error if trying to make larger
    assert(newsize != 0);               // Error if new size is 0

#if     defined(MEMCHECK)

    delete_mblock(p1);

#endif

    void *p2 = realloc(p1, (size_t)newsize);

    if (p2 == NULL)
    {
        throw(E_MEM);                   // Memory overflow
    }

#if     defined(MEMCHECK)

    add_mblock(p2, newsize);

#endif

    return p2;
}


///
///  @brief    Duplicate string. Performs the same function as strdup(), but
///            ensures we add a tracking block if we're debugging memory
///            allocations.
///
///  @returns  Pointer to new memory.
///
////////////////////////////////////////////////////////////////////////////////

char *strdup_mem(const char *p)
{
    void *p1 = strdup(p);

    if (p1 == NULL)
    {
        throw(E_MEM);                   // Memory overflow
    }

#if     defined(MEMCHECK)

    add_mblock(p1, (uint)strlen(p1) + 1);

#endif

    return p1;
}
