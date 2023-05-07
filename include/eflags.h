///
///  @file    eflags.h
///  @brief   Definition of various bit flag structures used in TECO.
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

#if     !defined(_EFLAGS_H)

#define _EFLAGS_H

#include <stdbool.h>            //lint !e451
#include <sys/types.h>          //lint !e451


///  @struct  e0_flag
///
///  @brief   Definition of flags are used internally, and which generally
///           cannot be read or set by the user.

union e0_flag
{
    int_t flag;                 ///< Entire E0 flag

    struct
    {
        uint exec    : 1;       ///< Executing command (reset by CTRL/C)
        uint skip    : 1;       ///< Skipping command
        uint error   : 1;       ///< Last command caused error
        uint sigint  : 1;       ///< SIGINT signal seen once
        uint lower   : 1;       ///< Force string arguments to lower case
        uint upper   : 1;       ///< Force string arguments to upper case
        uint display : 1;       ///< Display mode is active
        uint window  : 1;       ///< Window refresh needed
        uint cursor  : 1;       ///< Cursor update needed
        uint init    : 1;       ///< TECO is initializing
        uint i_redir : 1;       ///< stdin has been redirected
        uint o_redir : 1;       ///< stdout has been redirected
        uint ctrl_t  : 1;       ///< Reading input for CTRL/T command
    };
};

///  @struct  e1_flag
///
///  @brief   Definitions of extended features

union e1_flag
{
    int_t flag;                 ///< Entire E1 flag

    struct
    {
        uint xoper   : 1;       ///< Enable extended operators
        uint text    : 1;       ///< Enable extended text strings
        uint ctrl_a  : 1;       ///< Allow colon modifier for CTRL/A
        uint equals  : 1;       ///< Allow at-sign modifier for '='
        uint eimacro : 1;       ///< Execute EI commands as macros
        uint bang    : 1;       ///< !! starts end-of-line comment
        uint prompt  : 1;       ///< Add CR/LF before prompt if needed
        uint radix   : 1;       ///< Automatically determine number radix
        uint dollar  : 1;       ///< Echo delimiter as ESCape
        uint insert  : 1;       ///< Allow nI w/o ESCape or delimiter
        uint percent : 1;       ///< Allow :%q
        uint c_oper  : 1;       ///< Use C precedence for operators
        uint         : 1;       ///< (unused)
        uint         : 1;       ///< (unused)
        uint repeat  : 1;       ///< Double Ctrl-] repeats command
        uint newline : 1;       ///< LF acts like double ESCape
    };
};

///  @struct  e2_flag
///
///  @brief   Definition of command restrictions.

union e2_flag
{
    int_t flag;                 ///< Entire E2 flag

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
    int_t flag;                 ///< Entire E3 flag

    struct
    {
        uint nopage  : 1;       ///< Don't use FF as page delimiter
        uint smart   : 1;       ///< Smart mode for line terminators
        uint CR_in   : 1;       ///< Convert CR/LF to LF reading input
        uint CR_out  : 1;       ///< Convert LF to CR/LF writing output
        uint noin    : 1;       ///< Don't type input to log file
        uint noout   : 1;       ///< Don't type output to log file
        uint         : 1;       ///< (unused)
        uint keepNUL : 1;       ///< Keep NUL chrs. in input files
        uint CR_type : 1;       ///< Convert LF to CR/LF on type out
    };
};

///  @struct  e4_flag
///
///  @brief   Display flags.

union e4_flag
{
    int_t flag;                 ///< Entire E4 flag

    struct
    {
        uint invert  : 1;       ///< Put command window above edit window
        uint fence   : 1;       ///< Line between edit and command windows
        uint status  : 1;       ///< Display status on line
    };
};

///  @union   ed_flag
///
///  @brief   Definition of flags that affect edit operations.

union ed_flag
{
    int_t flag;                 ///< Entire ED flag

    struct
    {
        uint caret     : 1;     ///< Allow caret (^) in search strings
        uint yank      : 1;     ///< Allow all Y and _ commands
        uint           : 1;     ///< (Expand memory as much as possible)
        uint           : 1;     ///< (Reserved for TECO-8)
        uint keepdot   : 1;     ///< Preserve dot if search fails
        uint escape    : 1;     ///< Enable immediate escape-sequence commands
        uint movedot   : 1;     ///< Move dot by one on multiple occurrence searches
        uint           : 1;     ///< (Automatic refresh inhibit)
        uint nobuffer  : 1;     ///< Flush output immediately
    };
};

///  @union   eh_flag
///
///  @brief   Definition of flags that affect output of help messages when an
///           error occurs.

union eh_flag
{
    int_t flag;                 ///< Entire EH flag

    struct
    {
        uint why       : 2;     ///< Why the error occurred
        uint what      : 1;     ///< What command caused the error
        uint where     : 1;     ///< Where the error occurred (line no.)

#if     defined(DEBUG)

        uint who       : 1;     ///< Who issued the error (function name)

#endif

    };
};

///  @enum   help
///  @brief  Definitions of verbosity levels for error messages.

enum help
{
    HELP_TERSE   = 1,
    HELP_CONCISE = 2,
    HELP_VERBOSE = 3
};

///  @union   et_flag
///
///  @brief   Definition of flags that affect terminal input and output.

union et_flag
{
    int_t flag;                 ///< Entire ET flag

    struct
    {
        uint image    : 1;      ///< Type out in image mode
        uint rubout   : 1;      ///< Process DEL and ^U in scope mode
        uint lower    : 1;      ///< Read lower case
        uint noecho   : 1;      ///< Disable echo for ^T commands
        uint          : 1;      ///< (Cancel ^O on type out)
        uint nowait   : 1;      ///< Read w/o wait for ^T commands
        uint detach   : 1;      ///< Detach from terminal
        uint abort    : 1;      ///< Abort-on-error bit
        uint truncate : 1;      ///< Truncate output lines to terminal width
        uint scope    : 1;      ///< Scope terminal
        uint          : 1;      ///< (Refresh scope terminal)
        uint          : 1;      ///< (unused)
        uint eightbit : 1;      ///< Can handle 8-bit characters
        uint accent   : 1;      ///< Accent grave is ESCape surrogate
        uint          : 1;      ///< (unused)
        uint ctrl_c   : 1;      ///< CTRL/C detection control
    };
};

///  @struct  flags
///
///  @brief   Master flag structure.

struct flags
{
    bool       ctrl_e;          ///< Form feed flag
    int_t      ctrl_x;          ///< Search mode flag
    union  e0_flag e0;          ///< Internal flags (not settable by user)
    union  e1_flag e1;          ///< Extended features
    union  e2_flag e2;          ///< Command restrictions
    union  e3_flag e3;          ///< File I/O features
    union  e4_flag e4;          ///< Display mode flags
    union  ed_flag ed;          ///< Edit level flags
    int_t          ee;          ///< ESCape surrogate
    union  eh_flag eh;          ///< Help message flags
    int_t          ej;          ///< Operating system type
    int_t          eo;          ///< TECO version number
    int_t          es;          ///< Search verification flag
    union  et_flag et;          ///< Terminal flags
    int_t          eu;          ///< Upper/lower case flag
    int_t          ev;          ///< Edit verify flag
    int_t       radix;          ///< Current input radix
    bool        trace;          ///< Command trace flag
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
