///
///  @file    editbuf.h
///  @brief   Edit buffer interface.
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

#if     !defined(_EDITBUF_H)

#define _EDITBUF_H

#include <stdbool.h>            //lint !e451
#include <sys/types.h>          //lint !e451


/// @brief  Return status when inserting characters in buffer.

typedef enum
{
    EDIT_OK    = -1,
    EDIT_WARN  = -2,
    EDIT_FULL  = -3,
    EDIT_ERROR = -4
} estatus;


///  @struct  edit
///
///  @brief   Edit buffer

struct edit
{
    const int_t B;                ///< First position in buffer
    int_t Z;                      ///< Last position in buffer
    int_t dot;                    ///< Current position in buffer
};

extern struct edit t;

#if     defined(DISPLAY_MODE)

extern bool dot_changed;

extern bool ebuf_changed;

#endif

// Add one character to buffer at current position of dot.
//
// Returns: EDIT_OK    - Insertion was successful.
//          EDIT_WARN  - Insertion was successful, but buffer is getting full.
//          EDIT_FULL  - Insertion was successful, but buffer just became full.
//          EDIT_ERROR - Insertion was unsuccessful. Buffer is already full.

extern estatus add_ebuf(int c);

//  Delete nbytes at dot. Argument can be positive or negative.

extern void delete_ebuf(int_t nbytes);

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

extern int getchar_ebuf(int_t relpos);

//  Get the number of chars between current dot and nth line terminator.

extern int_t getdelta_ebuf(int_t nlines);

//  Get the number of lines between current dot and start/end of edit buffer.

extern int_t getlines_ebuf(int relpos);

//  Get the size of the edit buffer.

extern uint_t getsize_ebuf(void);

//  Initialize buffer.

extern void init_ebuf(void);

//  Delete all of the text in the edit buffer.

extern void kill_ebuf(void);

// Set buffer position.

extern void setpos_ebuf(int_t n);

// Set maximum memory size.

extern void setsize_ebuf(uint_t nbytes);

#endif  // !defined(_EDITBUF_H)
