///
///  @file    estack.h
///  @brief   Header file for TECO expression stack functions.
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

#if     !defined(_ESTACK_H)

#define _ESTACK_H

#if     !defined(_STDBOOL_H)
#include <stdbool.h>
#endif

#if     !defined(_SYS_TYPES_H)
#include <sys/types.h>
#endif

#define EXPR_SIZE            64         ///< Size of expression stack

///  @enum   oper_type
///  @brief  Type of operator stored on expression stack

enum oper_type
{
    TYPE_GROUP = 1,                 // Group command (,),{,}
    TYPE_OPER  = 2                  // Operator command
};

///  @enum   expr_type
///  @brief  Type of item stored on expression stack.

enum expr_type
{
    EXPR_NONE   = 0,
    EXPR_VALUE  = '0',              // General value
    EXPR_LPAREN = '(',              // Left parenthesis
    EXPR_RPAREN = ')',              // Right parenthesis
    EXPR_LBRACE = '{',              // Left brace
    EXPR_RBRACE = '}',              // Right brace
    EXPR_COMP   = '\x1F',           // 1's complement
    EXPR_OR     = '#',              // Bitwise OR
    EXPR_AND    = '&',              // Bitwise AND
    EXPR_XOR    = '~',              // Bitwise XOR
    EXPR_MUL    = '*',              // Multiplication
    EXPR_PLUS   = '+',              // Addition and unary plus
    EXPR_MINUS  = '-',              // Subtraction and unary minus
    EXPR_DIV    = '/',              // Division w/ quotient
    EXPR_REM    = '%',              // Division w/ remainder (//)
    EXPR_NOT    = '!',              // Logical NOT
    EXPR_LT     = '<',              // Less than
    EXPR_GT     = '>',              // Greater than
    EXPR_EQ     = '=',              // Equals (==)
    EXPR_NE     = 'N',              // Not equals (<>)
    EXPR_LE     = 'L',              // Less than or equal (<=)
    EXPR_GE     = 'G',              // Greater than or equal (>=)
    EXPR_LSHIFT = 'S',              // Left shift (<<)
    EXPR_RSHIFT = 'U'               // Right shift (>>)
};

///  @struct e_obj
///  @brief  Definition of objects on expression stack.

struct e_obj
{
    long value;                     ///< Operand value (if type = EXPR_VALUE)
    enum expr_type type;            ///< Value type (operand/operator)
};

///  @struct estack
///  @brief  Definition of expression stack.

struct estack
{
    uint level;                     ///< Expression stack level
    struct e_obj obj[EXPR_SIZE];    ///< Expression stack objects
};

// Global variables

extern struct estack estack;        ///< Expression stack

// Expression stack functions

extern bool check_expr(void);

extern void init_expr(void);

extern bool pop_expr(int *operand);

extern void push_expr(int c, enum expr_type type);

#endif  // !defined(_ESTACK_H)
