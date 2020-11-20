///
///  @file    cmd_estack.c
///  @brief   Functions that handle the expression stack.
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

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"


///  @struct  x
///  @brief   Expression stack used for parsing command strings.

struct xstack x;

// Local functions

static inline void reduce(void);

static inline bool reduce2(void);

static inline bool reduce3(void);


///
///  @brief    Initialize expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_x(void)
{
    x.base    = 0;
    x.level   = 0;
    x.operand = false;

    for (uint i = x.level; i < XSTACK_SIZE; ++i)
    {
        x.obj[i].type = x.obj[i].operand = 0;
    }
}


///
///  @brief    Return whether the top of the expression stack is an operand.
///            Note: if the stack is empty, then there's obviously no operand.
///
///  @returns  true if an operand, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool pop_x(int_t *operand)
{
    assert(operand != NULL);            // Error if NULL operand

    x.operand = false;

    if (x.level == x.base)              // Anything on stack?
    {
        return false;                   // No
    }

    if (x.obj[x.level - 1].type == X_OPERAND)
    {
        *operand = (int)x.obj[--x.level].operand;

        return true;
    }

    // A leading minus sign without a previous operand is treated as -1;
    // any other leading operator is an error.

    if (x.level == x.base + 1)
    {
        if (x.obj[x.level - 1].type == X_MINUS)
        {
            --x.level;

            *operand = -1;

            return true;
        }
    }

    // Here if we have a partial expression with no operand on top

    return false;
}


///
///  @brief    Push operator or operand on expression stack.
///
///            This function pushes an value onto the expression stack. The
///            expression stack implement's TECO's expression handling capa-
///            bility. For instance, if a command like 10+qa=$ is executed,
///            then three values are pushed onto the expression stack: 10,
///            the plus sign and the value of qa. Each time a value is pushed
///            onto the expression stack, the reduce() function is called
///            to see if the stack can be reduced. In the above example, re-
///            duce() would cause the stack to be reduced when the value of
///            qa is pushed, because the expression can be evaluated then.
///
///  @returns  Nothing (return to main loop if error).
///
////////////////////////////////////////////////////////////////////////////////

void push_x(int_t value, enum x_type type)
{
    if (x.level == XSTACK_SIZE)
    {
        throw(E_PDO);                   // Push-down list overflow
    }

    x.obj[x.level].operand = value;
    x.obj[x.level++].type  = type;

    if (x.level == 1)
    {
        x.operand = (type == X_OPERAND);

        return;
    }

    reduce();
}


///
///  @brief    Reduce expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static inline void reduce(void)
{
    // Try to reduce the expression stack if 3 or more items

    while (x.level >= x.base + 3 && reduce3())
    {
        ;
    }

    // Try to reduce the expression stack if 2 or more items
    
    while (x.level >= x.base + 2 && reduce2())
    {
        ;
    }

    // If the top of the expression stack is a 1's complement operator,
    // and there's no operand preceding it, that's an error.

    if (x.level >= x.base + 1 && x.obj[x.level - 1].type == X_1S_COMP)
    {
        if (x.level == x.base + 1 || x.obj[x.level - 2].type != X_OPERAND)
        {
            throw(E_NAB);               // No argument before ^_
        }
    }

    // Set flag based on whether the top stack item is an operand

    x.operand = (x.level != x.base && x.obj[x.level - 1].type == X_OPERAND);
}


///
///  @brief    Reduce top two items on expression stack if possible.
///
///  @returns  true if we reduced expression stack, else false.
///
////////////////////////////////////////////////////////////////////////////////

static inline bool reduce2(void)
{
    struct x_obj *p1 = &x.obj[x.level - 1];
    struct x_obj *p2 = &x.obj[x.level - 2];

    // The following prevents expressions such as these:
    //
    //     12!34   (use of logical NOT following an operand)
    //     BZ+34   (two operands with no operator)
    //     12++34  (two operators with no operand)
    //
    // If it is desired to use a unary plus after an addition operator, or a
    // unary minus after a subtraction operator, then use parentheses:
    //
    //     12+(+34)
    //     12-(-34)

    if (f.e2.oper)
    {
        if ((p1->type == X_NOT     && p2->type == X_OPERAND) ||
            (p1->type == X_OPERAND && p2->type == X_OPERAND) ||
            (p1->type > X_OPERAND  && p2->type > X_OPERAND))
        {
            throw(E_IFE);               // Ill-formed numeric expression
        }
    }

    if (p1->type == X_OPERAND && p2->type == X_PLUS)
    {
        p2->operand = p1->operand;
        p2->type = X_OPERAND;

        --x.level;
    }
    else if (p1->type == X_OPERAND && p2->type == X_MINUS)
    {
        p2->operand = -p1->operand;
        p2->type = X_OPERAND;

        --x.level;
    }
    else if (p1->type == X_OPERAND && p2->type == X_NOT)
    {
        // Logical NOT yields -1 for true and 0 for false.

        p2->operand = !p1->operand ? -1 : 0;
        p2->type = X_OPERAND;

        --x.level;
    }
    else
    {
        return false;
    }

    return true;
}


///
///  @brief    Reduce top three items on expression stack if possible.
///
///  @returns  true if we were able to reduce expression stack, else false.
///
////////////////////////////////////////////////////////////////////////////////

static inline bool reduce3(void)
{
    struct x_obj *p1 = &x.obj[x.level - 1];
    struct x_obj *p2 = &x.obj[x.level - 2];
    struct x_obj *p3 = &x.obj[x.level - 3];

    // Reduce (x) to x

    if (p3->type == X_LPAREN && p1->type == X_RPAREN && p2->type == X_OPERAND)
    {
        p3->operand = p2->operand;
        p3->type = X_OPERAND;

        x.level -= 2;

        return true;
    }

    // Anything else has to be of the form x <operator> y.

    if (p3->type != X_OPERAND || p2->type == X_OPERAND || p1->type != X_OPERAND)
    {
        return false;
    }

    // Here to process arithmetic and logical operators

    switch ((int)p2->type)
    {
        case X_PLUS:
            p3->operand += p1->operand;

            break;

        case X_MINUS:
            p3->operand -= p1->operand;

            break;

        case X_MUL:
            p3->operand *= p1->operand;

            break;

        case X_DIV:
            if (p1->operand == 0)
            {
                if (f.e2.zero)
                {
                    throw(E_DIV);       // Division by zero
                }

                p3->operand = 0;
            }
            else
            {
                p3->operand /= p1->operand;
            }

            break;

        case X_AND:
            p3->operand &= p1->operand;

            break;

        case X_OR:
            p3->operand |= p1->operand;

            break;

        case X_XOR:
            p3->operand ^= p1->operand;
            break;

        case X_REM:
            p3->operand %= p1->operand;
            break;

        case X_EQ:
            p3->operand = (p3->operand == p1->operand) ? -1 : 0;

            break;

        case X_NE:
            p3->operand = (p3->operand != p1->operand) ? -1 : 0;

            break;

        case X_LT:
            p3->operand = (p3->operand < p1->operand) ? -1 : 0;

            break;

        case X_LE:
            p3->operand = (p3->operand <= p1->operand) ? -1 : 0;

            break;

        case X_GT:
            p3->operand = (p3->operand > p1->operand) ? -1 : 0;

            break;

        case X_GE:
            p3->operand = (p3->operand >= p1->operand) ? -1 : 0;

            break;

        case X_LSHIFT:
            p3->operand = (int)((uint)p3->operand << p1->operand);

            break;

        case X_RSHIFT:
            p3->operand = (int)((uint)p3->operand >> p1->operand);

            break;

        default:
            throw(E_ARG);               // Improper arguments
    }

    p3->type = X_OPERAND;
    x.level -= 2;

    return true;
}


///
///  @brief    Reset base of expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_x(uint base)
{
    x.base = base;

    reduce();
}


///
///  @brief    Set base of expression stack.
///
///  @returns  Old base.
///
////////////////////////////////////////////////////////////////////////////////

uint set_x(void)
{
    uint base = x.base;

    x.base = x.level;
    x.operand = false;

    return base;
}


///
///  @brief    See if top of stack is a minus sign, which may mean a command
///            argument of -1.
///
///  @returns  true if minus sign found, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool unary_x(void)
{
    if (x.level != x.base + 1 || x.obj[0].type != X_MINUS)
    {
        return false;
    }

    --x.level;

    return true;
}
