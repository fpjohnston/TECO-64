///
///  @file    estack.h
///  @brief   Header file for TECO expression stack functions.
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

#if     !defined(_XSTACK_H)

#define _XSTACK_H

#include <stdbool.h>            //lint !e451 !e537
#include <sys/types.h>          //lint !e451 !e537


#define XSTACK_SIZE          64     ///< Size of expression stack

///  @enum   x_type
///  @brief  Type of item stored on expression stack.

enum x_type
{
    X_LPAREN  = -2,                 // Left parenthesis
    X_RPAREN  = -1,                 // Right parenthesis
    X_OPERAND = 0,                  // Operand
    X_1S_COMP = '\x1F',             // 1's complement
    X_OR      = '#',                // Bitwise OR
    X_AND     = '&',                // Bitwise AND
    X_XOR     = '~',                // Bitwise XOR
    X_MUL     = '*',                // Multiplication
    X_PLUS    = '+',                // Addition and unary plus
    X_MINUS   = '-',                // Subtraction and unary minus
    X_DIV     = '/',                // Division w/ quotient
    X_REM     = '%',                // Division w/ remainder (//)
    X_NOT     = '!',                // Logical NOT
    X_LT      = '<',                // Less than
    X_GT      = '>',                // Greater than
    X_EQ      = '=',                // Equals (==)
    X_NE      = 'N',                // Not equals (<>)
    X_LE      = 'L',                // Less than or equal (<=)
    X_GE      = 'G',                // Greater than or equal (>=)
    X_LSHIFT  = 'S',                // Left shift (<<)
    X_RSHIFT  = 'U'                 // Right shift (>>)
};

///  @struct x_obj
///  @brief  Definition of objects on expression stack.

struct x_obj
{
    int_t operand;                  ///< Operand value (if type = X_OPERAND)
    enum x_type type;               ///< Operator type (or operand flag)
};

///  @struct xstack
///  @brief  Definition of expression stack.

struct xstack
{
    uint level;                     ///< Expression stack level
    uint base;                      ///< Expression stack base
    bool operand;                   ///< Top of stack is an operand
    struct x_obj obj[XSTACK_SIZE];  ///< Expression stack objects
};

#define check_x() x.operand         ///< true if expression stack top is operand

// Global variables

extern struct xstack x;             ///< Expression stack

// Expression stack functions

extern void init_x(void);

extern bool pop_x(int_t *operand);

extern void push_x(int_t value, enum x_type type);

extern void reset_x(uint base);

extern uint set_x(void);

extern bool unary_x(void);

#endif  // !defined(_XSTACK_H)
