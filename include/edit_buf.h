///
///  @file    edit_buf.h
///  @brief   Edit buffer interface.
///
///  @bug     No known bugs.
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

#define EDIT_SHRINK     (bool)true      ///< Allow edit buffer to shrink

#define EDIT_NOSHRINK   (bool)false     ///< Don't allow edit buffer to shrink


// Add one character to buffer at current position of dot.
//
// Returns: EDIT_OK    - Insertion was successful.
//          EDIT_WARN  - Insertion was successful, but buffer is getting full.
//          EDIT_FULL  - Insertion was successful, but buffer just became full.
//          EDIT_ERROR - Insertion was unsuccessful. Buffer is already full.

extern estatus add_edit(int c);

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

extern int char_edit(int n);

// Get beginning position in buffer (always returns 0).

#define get_B()         0

// Get current dot in buffer.

extern uint get_dot(void);

// Get ending position in buffer. Used to return value of TECO variable Z, as
// as to determine whether the buffer is currently empty.

extern uint get_Z(void);

// Initialize buffer.
//
// start -  Initial size of buffer, in bytes.
//
// plus  -  No. of bytes to increment buffer when warning level encountered.
//          If 0, buffer is never increased.
//
// warn  -  Number from 0-100 indicating the percentage of how full the buffer
//          can be before trying to increase its size.
//
// shrink - true if should decrease buffer to initial size whenever it is
//          emptied by HK command.

extern void init_edit(uint minsize, uint maxsize, uint stepsize, uint warn, bool shrink);

// Jump to absolute position in buffer.
//
// Returns: true if position was in buffer, false if not.

extern bool jump_edit(uint n);

// Delete entire buffer (used by HK). This will cause the buffer to shrink back
// its original size, if the shrink parameter in init_edit() was true.

extern void kill_edit(bool shrink);

// Move to position in buffer relative to dot.
//
// Returns: true if position was in buffer, false if it was not.

extern bool move_edit(int n);

// Print contents of buffer between lines m and n relative to dot.

extern void print_edit(int m, int n, void (*printc)(int c));

// Move to line relative to current position.

extern void step_edit(int n);

// Type contents of buffer between absolute positions m and n.
//
// Returns: true if both positions were within buffer, false if not.

extern bool type_edit(uint m, uint n, void (*printc)(int c));

// Write out contents of buffer from position m through n.

extern void write_edit(uint m, uint n, void (*write_cb)(const char *buf, uint len));

