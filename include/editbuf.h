///
///  @file    editbuf.h
///  @brief   Edit buffer interface.
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

#if     !defined(_EDITBUF_H)

#define _EDITBUF_H

#include <stdbool.h>            //lint !e451

#include "file.h"

///  @struct  edit
///
///  @brief   Edit buffer variables

struct edit
{
    uint_t size;                ///< Size of edit buffer in bytes
    const int_t B;              ///< First position in buffer
    int_t Z;                    ///< Last position in buffer
    int_t dot;                  ///< Current position in buffer
    int c;                      ///< Current character (or EOF)
    int lastc;                  ///< Previous character (or EOF)
    int nextc;                  ///< Next character (or EOF)
    int len;                    ///< Length of current line in bytes
    int pos;                    ///< Position in line
};

extern const struct edit *t;

// Get no. of lines after dot.

extern int_t after_dot(void);

// Append file to buffer.

extern bool append_edit(struct ifile *ifile, bool single);

// Get no. of lines before dot.

extern int_t before_dot(void);

// Change character at dot.

extern void change_dot(int c);

//  Delete nbytes at dot. Argument can be positive or negative.

extern void delete_edit(int_t nbytes);

//  Initialize edit buffer.

extern void init_edit(void);

// Insert a character in buffer at current position of dot.

extern bool insert_edit(const char *c, size_t nbytes);

//  Delete all of the text in the edit buffer.

extern void kill_edit(void);

//  Get the number of chars between current dot and nth line terminator.

extern int_t len_edit(int_t nlines);

// Set dot to relative position.

extern void move_dot(int_t delta);

// Read ASCII value of character in buffer at position relative to dot.
//
// Example values:
//
//  0 -> character at current position of dot.
//  1 -> character one position ahead of dot.
// -1 -> character one position behind dot.
//
// Returns: character found, or EOF (-1) if attempt was made to go beyond the
//          beginning or end of buffer.

extern int read_edit(int_t relpos);

// Set dot to absolute position.

extern void set_dot(int_t pos);

// Set maximum memory size.

extern uint_t size_edit(uint_t size);

#endif  // !defined(_EDITBUF_H)
