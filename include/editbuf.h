///
///  @file    textbuf.h
///  @brief   Edit buffer interface.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
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

#if     !defined(_TEXTBUF_H)
#define _TEXTBUF_H

#if     !defined(_STDBOOL_H)
#include <stdbool.h>
#endif

#if     !defined(_SYS_TYPES_H)
#include <sys/types.h>
#endif

/// @brief  Return status when inserting characters in buffer.

typedef enum
{
    EDIT_OK,
    EDIT_WARN,
    EDIT_FULL,
    EDIT_ERROR
} estatus;


///  @struct  text
///
///  @brief   Text buffer

struct text
{
    int B;                      ///< First position in buffer
    int Z;                      ///< Last position in buffer
    int dot;                    ///< Current position in buffer
    int size;                   ///< Size of buffer in bytes
};

extern struct text t;

extern bool ebuf_changed;

// Add one character to buffer at current position of dot.
//
// Returns: EDIT_OK    - Insertion was successful.
//          EDIT_WARN  - Insertion was successful, but buffer is getting full.
//          EDIT_FULL  - Insertion was successful, but buffer just became full.
//          EDIT_ERROR - Insertion was unsuccessful. Buffer is already full.

extern int add_ebuf(int c);

//  Delete nbytes at dot. Argument can be positive or negative.

extern void delete_ebuf(int n);

// Get ASCII value of character in buffer at position relative to dot.
//
// Examples of values of n:
//
//  0 -> character at current position of dot.
//  1 -> character one position ahead of dot.
// -1 -> character one position behind dot.
//
// Returns: character found, or EOF (-1) if attempt was made to go beyond the
//          beginning or end of buffer.

extern int getchar_ebuf(int n);

//  Get the number of chars between current dot and nth line terminator.

extern int getdelta_ebuf(int n);

//  Get the number of lines between current dot and start/end of text buffer.

extern int getlines_ebuf(int n);

//
//  Initialize buffer.
//
//  minsize -  Initial and minimum size of buffer, in bytes.
//
//  maxsize -  Maximum size of buffer, in bytes.
//
//  stepsize - No. of bytes to increment buffer when warning level encountered.
//             If 0, buffer is never increased.
//
//  warn     - The percentage (0-100%) of how full the buffer can be before
//             trying to increase its size.
//

extern void init_ebuf(int minsize, int maxsize, int stepsize, int warn);

// Replace ASCII value of character in buffer at position relative to dot.
//
// Returns: original character, or EOF (-1) if attempt was made to go beyond
//          the beginning or end of buffer.

extern int putchar_ebuf(int n, int c);

// Set buffer position.

extern void setpos_ebuf(int n);

// Set maximum memory size, in K bytes.

extern int setsize_ebuf(int n);

#endif
