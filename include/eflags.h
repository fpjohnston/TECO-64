///
///  @file    eflags.h
///  @brief   Definition of various bit flag structures used in TECO.
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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

#include <stdbool.h>            //lint !e451 !e537
#include <sys/types.h>          //lint !e451 !e537


///  @struct  e0_flag
///
///  @brief   Definition of flags are used internally, and which generally
///           cannot be read or set by the user.

struct e0_flag
{
    uint exec    : 1;           ///< Executing command
    uint error   : 1;           ///< Last command caused error
    uint ctrl_c  : 1;           ///< CTRL/C seen
    uint lower   : 1;           ///< Force string arguments to lower case
    uint upper   : 1;           ///< Force string arguments to upper case
    uint display : 1;           ///< Display mode is active
    uint trace   : 1;           ///< Trace mode is active
    uint init    : 1;           ///< TECO is initializing
};

///  @struct  e1_flag
///
///  @brief   Definitions of extended features

union e1_flag
{
    int flag;                   ///< Entire E1 flag

    struct
    {
        uint xoper  : 1;        ///< Enable extended operators
        uint text   : 1;        ///< Enable extended text strings
        uint dollar : 1;        ///< $ is a valid symbol character
        uint ubar   : 1;        ///< _ is a valid symbol character
        uint new_ei : 1;        ///< Use new-style EI commands
        uint bang   : 1;        ///< !! starts end-of-line comment
        uint prompt : 1;        ///< Add CR/LF before prompt if needed
    };
};

///  @struct  e2_flag
///
///  @brief   Definition of command restrictions.

union e2_flag
{
    int flag;                   ///< Entire E2 flag

    struct
    {
        uint zero   : 1;        ///< Division by zero
        uint oper   : 1;        ///< Double operators in expressions
        uint atsign : 1;        ///< Invalid @, or more than one @
        uint colon  : 1;        ///< Invalid :, or more than two colons
        uint comma  : 1;        ///< No argument before ,
        uint m_arg  : 1;        ///< Invalid m argument
        uint n_arg  : 1;        ///< Invalid n argument
        uint loop   : 1;        ///< Loop not complete within conditional
        uint quote  : 1;        ///< Conditional not complete within loop
        uint page   : 1;        ///< m,n:P or H:P or :PW
        uint args   : 1;        ///< Too many arguments for command
    };
};

///  @struct  e3_flag
///
///  @brief   Features affecting I/O operations and file contents.

union e3_flag
{
    int flag;                   ///< Entire E3 flag

    struct
    {
        uint nopage : 1;        ///< Don't use FF as page delimiter
        uint smart  : 1;        ///< Smart mode for line terminators
        uint icrlf  : 1;        ///< Use CR/LF for input lines
        uint ocrlf  : 1;        ///< Use CR/LF for output lines
        uint noin   : 1;        ///< Don't type input to log file
        uint noout  : 1;        ///< Don't type output to log file
    };
};

///  @struct  e4_flag
///
///  @brief   Display flags.

union e4_flag
{
    int flag;                   ///< Entire E4 flag

    struct
    {
        uint invert  : 1;       ///< Put command region above text region
        uint line    : 1;       ///< Line between text and command regions
        uint status  : 1;       ///< Display status on line
    };
};

///  @union   ed_flag
///
///  @brief   Definition of flags that affect edit operations.

union ed_flag
{
    int flag;                   ///< Entire ED flag

    struct
    {
        uint caret     : 1;     ///< Allow caret (^) in search strings
        uint yank      : 1;     ///< Allow all Y and _ commands
        uint memory    : 1;     ///< Expand memory as much as possible
        uint           : 1;     ///< (unused)
        uint keepdot   : 1;     ///< Preserve dot if search fails
        uint escape    : 1;     ///< Enable immediate escape-sequence commands
        uint movedot   : 1;     ///< Move dot by one on multiple occurrence searches
        uint noauto    : 1;     ///< Automatic refresh inhibit
    };
};

///  @union   eh_flag
///
///  @brief   Definition of flags that affect output of help messages.

union eh_flag
{
    int flag;                   ///< Entire EH flag

    struct
    {
        uint verbose   : 2;     ///< Error message verbosity
        uint command   : 1;     ///< Print failing command if error
    };
};

///  @union   et_flag
///
///  @brief   Definition of flags that affect terminal input and output.

union et_flag
{
    int flag;                   ///< Entire ET flag

    struct
    {
        uint image    : 1;      ///< Type out in image mode
        uint rubout   : 1;      ///< Process DEL and ^U in scope mode
        uint lower    : 1;      ///< Read lower case
        uint noecho   : 1;      ///< Disable echo for ^T commands
        uint ctrl_o   : 1;      ///< Cancel ^O on type out
        uint nowait   : 1;      ///< Read with no wait
        uint detach   : 1;      ///< Detach flag
        uint abort    : 1;      ///< Abort-on-error bit
        uint truncate : 1;      ///< Truncate output lines to terminal width
        uint scope    : 1;      ///< Terminal is a scope
        uint refresh  : 1;      ///< Terminal is a refresh scope
        uint          : 1;      ///< (unused)
        uint eightbit : 1;      ///< Can handle 8-bit characters
        uint accent   : 1;      ///< Accent grave is ESCAPE surrogate
        uint vt200    : 1;      ///< Special VT200 mode
        uint ctrl_c   : 1;      ///< CTRL/C detection control
    };
};

///  @struct  flags
///
///  @brief   Master flag structure.

struct flags
{
    bool       ctrl_e;          ///< Form feed flag
    int        ctrl_x;          ///< Search mode flag
    struct e0_flag e0;          ///< Internal flags (not settable by user)
    union  e1_flag e1;          ///< Extended features
    union  e2_flag e2;          ///< Command restrictions
    union  e3_flag e3;          ///< File I/O features
    union  e4_flag e4;          ///< Display mode flags
    union  ed_flag ed;          ///< Edit level flags
    int            ee;          ///< ESCape surrogate
    union  eh_flag eh;          ///< Help message flags
    int            ej;          ///< Operating system type
    int            eo;          ///< TECO version number
    int            es;          ///< Search verification flag
    union  et_flag et;          ///< Terminal flags

#if     defined(CONFIG_EU)

    int            eu;          ///< Upper/lower case flag

#endif

    int            ev;          ///< Edit verify flag
};

///  @var    f
///
///  @brief  Master flag used to access all other flag variables. Note that
///          the fact that this variable is only a single character is not
///          a problem, because it will never be referenced by itself, but
///          always in conjunction with its members (e.g., f.eh.verbose,
///          f.et.lower, f.eu).

extern struct flags f;

#endif  // !defined(_EFLAGS_H)
