///
///  @file    eflags.h
///  @brief   Definition of various bit flag structures used in TECO.
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

#if     !defined(_EFLAGS_H)

#define _EFLAGS_H

#if     !defined(_STDINT_H)
#include <stdint.h>
#endif

///  @struct  e0_flag
///  @brief   Definition of flags are used internally, and which cannot be read
///           or set by the user.

struct e0_flag
{
    uint exec   : 1;            ///< Executing command
    uint ctrl_c : 1;            ///< CTRL/C seen
    uint dryrun : 1;            ///< Parse commands w/o executing them
    uint lower  : 1;            ///< Force string arguments to lower case
    uint upper  : 1;            ///< Force string arguments to upper case
    uint exit   : 1;            ///< Exit when MUNG command is done
    uint window : 1;            ///< Use window display
};

///  @struct  e1_flag
///  @brief   Definition of debugging flags.

union e1_flag
{
    uint flag;                  ///< Entire E1 flag

    struct
    {
        uint strict  : 1;       ///< Strictly enforce command syntax
        uint bottext : 1;       ///< Put text window under command window
    };
};

///  @struct  e2_flag
///  @brief   Definition of compatibility features.

union e2_flag
{
    uint flag;                  ///< Entire E2 flag

    struct
    {
        uint dollar : 1;        ///< $ is a valid symbol character
        uint ubar   : 1;        ///< _ is a valid symbol character
        uint add_cr : 1;        ///< Add CR to LF on output
        uint no_cr  : 1;        ///< Strip CR on input
        uint no_ff  : 1;        ///< FF is normal character on input
    };
};

///  @struct  e3_flag
///  @brief   Definition of extended features.

union e3_flag
{
    uint flag;                  ///< Entire E3 flag

    struct
    {
        uint brace  : 1;        ///< Enable braced expressions
        uint tilde  : 1;        ///< Enable tilde commands
        uint msec   : 1;        ///< Return time in milliseconds
    };
};

///  @struct  e4_flag
///  @brief   Definition of file options.

union e4_flag
{
    uint flag;                  ///< Entire E4 flag

    struct
    {
        uint noin   : 1;        ///< Don't type input to log file
        uint noout  : 1;        ///< Don't type output to log file
    };
};

///  @union   ed_flag
///  @brief   Definition of flags that affect edit operations.

union ed_flag
{
    uint flag;                  ///< Entire ED flag

    struct
    {
        uint caret     : 1;     ///< Allow caret (^) in search strings
        uint yank      : 1;     ///< Allow all Y and _ commands
        uint memory    : 1;     ///< Expand memory as much as possible
        uint           : 1;     ///< (unused)
        uint keepdot   : 1;     ///< Preserve dot if search fails
        uint escape    : 1;     ///< Enable immediate escape-sequence commands
        uint movedot   : 1;     ///< Move dot by one on multiple occurrence searches
        uint norfrsh   : 1;     ///< Automatic refresh inhibit
    };
};

///  @union   eh_flag
///  @brief   Definition of flags that affect output of help messages.

union eh_flag
{
    uint flag;                  ///< Entire EH flag

    struct
    {
        uint verbose   : 2;     ///< Error message verbosity
        uint command   : 1;     ///< Print failing command if error
    };
};

///  @union   et_flag
///  @brief   Definition of flags that affect terminal input and output.

union et_flag
{
    uint flag;                  ///< Entire ET flag

    struct
    {
        uint image    : 1;      ///< Type out in image mode
        uint rubout   : 1;      ///< Process DEL and ^U in scope mode
        uint lower    : 1;      ///< Read lower case
        uint noecho   : 1;      ///< Disable echo for ^T commands
        uint cancel   : 1;      ///< Cancel ^O on type out
        uint nowait   : 1;      ///< Read with no wait
        uint detach   : 1;      ///< Detach flag
        uint abort    : 1;      ///< Abort-on-error bit
        uint truncate : 1;      ///< Truncate output lines to terminal width
        uint scope    : 1;      ///< Terminal is a scope type
        uint rscope   : 1;      ///< Terminal is a refresh scope
        uint          : 1;      ///< (unused)
        uint eightbit : 1;      ///< Can handle 8-bit characters
        uint accent   : 1;      ///< Accent grave is ESCAPE surrogate
        uint vt200    : 1;      ///< Special VT200 mode
        uint ctrl_c   : 1;      ///< CTRL/C detection control
    };
};

///  @union   ez_flag
///  @brief   Definition of additional flags settable by the user.

union ez_flag
{
    uint flag;                  ///< Entire EZ flag

    struct
    {
        uint noversion : 1;     ///< No VMS-style versions
        uint           : 1;     ///
        uint           : 1;     ///
        uint arrow     : 1;     ///< (TODO: what's this for?)
        uint beep      : 1;     ///< Audio beep instead of flash
        uint winline   : 1;     ///< Line between windows
        uint           : 1;     ///
        uint formfeed  : 1;     ///< Do not stop read on FF
        uint unixnl    : 1;     ///< Use Unix-style newline terminators
        uint vt100     : 1;     ///< Use VT100 graphics
        uint           : 1;     ///
        uint btee      : 1;     ///< Use BTEE instead of DIAMOND (???)
        uint           : 1;     ///
        uint hidecr    : 1;     ///< Hide CR in scope
        uint nostrip   : 1;     ///< Don't strip filename extension
    };
};

///  @struct  flags
///  @brief   Master flag structure.

struct flags
{
    int        ctrl_x;          ///< Search mode flag
    struct e0_flag e0;          ///< Internal flags (not settable by user)
    union  e1_flag e1;          ///< Debugging flags
    union  e2_flag e2;          ///< Compatibility features
    union  e3_flag e3;          ///< Extended features
    union  e4_flag e4;          ///< File options
    int            e5;          ///< Comment bypass flag
    union  ed_flag ed;          ///< Edit level flags
    int            ee;          ///< ESCape surrogate
    union  eh_flag eh;          ///< Help message flags
    int            ej;          ///< Operating system type
    int            eo;          ///< TECO version number
    int            es;          ///< Search verification flag
    union  et_flag et;          ///< Terminal flags
    int            eu;          ///< Upper/lower case flag
    int            ev;          ///< Edit verify flag
    union  ez_flag ez;          ///< Additional external flags
};

///  @var    f
///  @brief  Master flag used to access all other flag variables. Note that
///          the fact that this variable is only a single character is not
///          a problem, because it will never be referenced by itself, but
///          always in conjunction with its members (e.g., f.eh.verbose,
///          f.et.lower, f.eu).

extern struct flags f;

#endif  // !defined(_EXEC_H)
