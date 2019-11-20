///
///  @file    eflags.h
///  @brief   Definition of various bit flag structures used in TECO.
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

#if     !defined(_EFLAGS_H)

#define _EFLAGS_H

#if     !defined(_STDINT_H)
#include <stdint.h>
#endif

// ED flags - these affect edit operations.

union ed_flag
{
    int flag;                   // Entire ED flag

    struct
    {
        uint caret     : 1;     // Allow caret (^) in search strings
        uint yank      : 1;     // Allow all Y and _ commands
        uint memory    : 1;     // Expand memory as much as possible
        uint           : 1;     // (reserved for TECO-8)
        uint keepdot   : 1;     // Preserve dot if search fails
        uint escape    : 1;     // Enable immediate escape-sequence commands
        uint movedot   : 1;     // Move dot by one on multiple occurrence searches
        uint refresh   : 1;     // Automatic refresh inhibit
    };
};

// Help flags - these affect output of help messages.

union eh_flag
{
    int flag;                   // Entire EH flag

    struct
    {
        uint verbose   : 2;     // Error message verbosity
        uint command   : 1;     // Print failing command if error
    };
};

// EI flags - these are only used internally and cannot be set by the user.

struct ei_flag
{
    uint strict    : 1;         // Strictly enforce command syntax
    uint scan      : 1;         // Scanning command
    uint exec      : 1;         // Executing command
    uint lf        : 1;         // Discard next character if LF
    uint ctrl_c    : 1;         // CTRL/C seen
//    uint comma     : 1;         // Comma seen
//    uint colon     : 1;         // Colon seen
//    uint dcolon    : 1;         // Double colon seen
//    uint atsign    : 1;         // At sign seen
};

// ET flags - these affect terminal behavior.

union et_flag
{
    int flag;                   // Entire ET flag

    struct
    {
        uint image     : 1;     // Type out in image mode
        uint rubout    : 1;     // Process DEL and ^U in scope mode
        uint lower     : 1;     // Read lower case
        uint noecho    : 1;     // Disable echo for ^T commands
        uint cancel    : 1;     // Cancel ^O on type out
        uint nowait    : 1;     // Read with no wait
        uint detach    : 1;     // Detach flag
        uint abort     : 1;     // Abort-on-error bit
        uint truncate  : 1;     // Truncate output lines to terminal width
        uint scope     : 1;     // Scope type terminal
        uint refresh   : 1;     // Refresh scope watch feature exists
        uint           : 1;     // (reserved for TECO-8)
        uint eightbit  : 1;     // Can handle 8-bit characters
        uint accent    : 1;     // Accent grave is ESCAPE surrogate
        uint vt200     : 1;     // Special VT200 mode
        uint ctrl_c    : 1;     // CTRL/C detection control
    };
};

// EZ flags - These provide additional capabilities settable by the user.

union ez_flag
{
    int flag;                   // Entire EZ flag

    struct
    {
        uint noversion : 1;     // No VMS-style versions
        uint           : 1;     // (unused)
        uint           : 1;     // (unused)
        uint arrow     : 1;     // ???
        uint beep      : 1;     // Audio beep or flash (CURSES)
        uint line      : 1;     // Line between windows
        uint           : 1;     // (unused)
        uint formfeed  : 1;     // Do not stop read on FF
        uint unixnl    : 1;     // Use Unix-style newline terminators
        uint vt100     : 1;     // Use VT100 graphics
        uint           : 1;     // (unused)
        uint btee      : 1;     // Use BTEE instead of DIAMOND (???)
        uint           : 1;     // (unused)
        uint hidecr    : 1;     // Hide CR in scope
        uint nostrip   : 1;     // Don't strip filename extension
    };
};

// Now define our master flag structure.

struct flags
{
    union  ed_flag ed;          // Edit level flags
    int            ee;          // ESCape surrogate
    union  eh_flag eh;          // Help message flags
    struct ei_flag ei;          // Internal flags
    int            ej;          // Operating system type
    int            eo;          // TECO version number
    int            es;          // Search verification flag
    union  et_flag et;          // Terminal flags
    int            eu;          // Upper/lower case flags
    int            ev;          // Edit verify flags
    union  ez_flag ez;          // Additional external flags
};

// Note that the fact that this variable is only a single character is not a
// problem, because it will be never by itself, but always in reference to its
// structure members (e.g., f.eh.verbose, f.et.lower, f.eu).

extern struct flags f;

#endif  // !defined(_EXEC_H)
