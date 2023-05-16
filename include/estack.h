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

#if     !defined(_ESTACK_H)

#define _ESTACK_H

#include <stdbool.h>


#define OPER    ((int_t)0)          ///< 'value' for expression operators

///  @enum   x_oper
///  @brief  Type of item stored on expression stack.

enum x_oper
{
    X_NULL = 0,         // Null type
    X_ADD,              // Addition
    X_AND,              // Bitwise AND
    X_COMPL,            // 1's complement
    X_DIV,              // Division w/ quotient
    X_EQ,               // Equals (==)
    X_GE,               // Greater than or equal (>=)
    X_GT,               // Greater than
    X_LE,               // Less than or equal (<=)
    X_LPAREN,           // Left parenthesis
    X_LSHIFT,           // Left shift (<<)
    X_LT,               // Less than
    X_MINUS,            // Unary minus
    X_MUL,              // Multiplication
    X_NE,               // Not equals (<>)
    X_NOT,              // Logical NOT
    X_OR,               // Bitwise OR
    X_PLUS,             // Unary plus
    X_REM,              // Division w/ remainder (//)
    X_RPAREN,           // Right parenthesis
    X_RSHIFT,           // Right shift (>>)
    X_SUB,              // Subtraction
    X_XOR,              // Bitwise XOR
    X_MAX               // Total no. of values
};

// Expression stack functions

extern bool auto_radix(void);

extern void confirm_parens(void);

extern void delete_x(void);

extern void exit_x(void);

extern void init_x(void);

extern void new_x(void);

extern bool query_x(int_t *n);

extern void scan_x(struct cmd *cmd);

extern void store_add(void);

extern void store_and(void);

extern void store_complement(void);

extern void store_mul(void);

extern void store_or(void);

extern void store_sub(void);

extern void store_val(int_t value);

#endif  // !defined(_ESTACK_H)
