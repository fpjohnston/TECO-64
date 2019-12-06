///
///  @file       memory.c
///  @brief      Memory allocation & deallocation functions.
///
///  @bug        No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
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
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "errors.h"


///
///  @brief    Get new memory.
///
///  @returns  Nothing (error if memory allocation fails).
///
////////////////////////////////////////////////////////////////////////////////

void *alloc_mem(uint size)
{
    void *ptr = calloc(1uL, (size_t)size);

    if (ptr == NULL)
    {
        print_err(E_MEM);               // Memory overflow
    }

    return ptr;
}


///
///  @brief    Get more memory.
///
///  @returns  Nothing (error if memory allocation fails).
///
////////////////////////////////////////////////////////////////////////////////

void *expand_mem(void *ptr, uint oldsize, uint newsize)
{
    assert(ptr != NULL);
    assert(oldsize != newsize);
    assert(oldsize < newsize);

    void *newptr = realloc(ptr, (size_t)newsize);

    if (newptr == NULL)
    {
        print_err(E_MEM);               // Memory overflow
    }

    return newptr;
}


///
///  @brief    Deallocate memory.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void free_mem(char **ptr)
{
    assert(ptr != NULL);                // Make sure pointer to pointer is real

    if (*ptr != NULL)
    {
        free(*ptr);

        *ptr = NULL;                    // Make sure we don't use this again
    }
}


///
///  @brief    Shrink memory.
///
///  @returns  Nothing (error if memory allocation fails).
///
////////////////////////////////////////////////////////////////////////////////

void *shrink_mem(void *ptr, uint oldsize, uint newsize)
{
    assert(ptr != NULL);
    assert(oldsize != newsize);
    assert(oldsize > newsize);

    void *newptr = realloc(ptr, (size_t)newsize);

    if (newptr == NULL)
    {
        print_err(E_MEM);               // Memory overflow
    }

    return newptr;
}
