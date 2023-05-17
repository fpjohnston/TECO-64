///
///  @file    errors.h
///  @brief   TECO error codes
///
/* (INSERT: WARNING NOTICE) */
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

#if     !defined(_ERRORS_H)

#define _ERRORS_H

#if     defined(__DECC)

    #define noreturn

#else

    #include <stdnoreturn.h>

#endif

extern int last_error;

///  @enum   errcodes
///  @brief  Definitions of TECO error messages.

enum errcodes
{
    E_NUL,          ///< No error
/* (INSERT: ERROR CODES) */
    NERRORS,        ///< Total number of errors
    E_BALK          ///< Unexpected end of command or macro
};

extern void print_command(void);

extern void print_verbose(int err_teco);

#if     defined(DEBUG)

//lint -save -e652

extern noreturn void (throw)(const char *func, unsigned int line, int err_teco, ...);

/// @def    throw
/// @brief  Debug version of throw(), used in order to add function and line number.

#define throw(err_teco, ...) (throw)(__func__, __LINE__, err_teco, ## __VA_ARGS__)

//lint -restore

#else

extern noreturn void throw(int err_teco, ...);

#endif

#endif  // !defined(_ERRORS_H)
