///
///  @file    estack.h
///  @brief   Header file for TECO expression stack functions.
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

#if     !defined(_XSTACK_H)

#define _XSTACK_H

#include <stdbool.h>            //lint !e451
#include <sys/types.h>          //lint !e451


#define OPER    ((int_t)0)          ///< 'value' for expression operators

///  @enum   x_oper
///  @brief  Type of item stored on expression stack.

enum x_oper
{
    X_NULL = 0,         // Null type
    X_1S_COMP,          // 1's complement
    X_AND,              // Bitwise AND
    X_DIV,              // Division w/ quotient
    X_EQ,               // Equals (==)
    X_GE,               // Greater than or equal (>=)
    X_GT,               // Greater than
    X_LE,               // Less than or equal (<=)
    X_LPAREN,           // Left parenthesis
    X_LSHIFT,           // Left shift (<<)
    X_LT,               // Less than
    X_MUL,              // Multiplication
    X_MINUS,            // Subtraction
    X_NE,               // Not equals (<>)
    X_NOT,              // Logical NOT
    X_OR,               // Bitwise OR
    X_PLUS,             // Addition
    X_REM,              // Division w/ remainder (//)
    X_RPAREN,           // Right parenthesis
    X_RSHIFT,           // Right shift (>>)
    X_UMINUS,           // Unary minus
    X_UPLUS,            // Unary plus
    X_XOR,              // Bitwise XOR
    X_MAX               // Total no. of values
};

// Expression stack functions

extern void exit_x(void);

extern void init_x(void);

extern bool check_x(int_t *n);

extern void pop_x(void);

extern void push_x(void);

extern void store_1s_comp(void);

extern void store_minus(void);

extern void store_oper(enum x_oper type);

extern void store_plus(void);

extern void store_val(int_t value);

extern bool unary_x(void);

#endif  // !defined(_XSTACK_H)
