///
///  @file    cmd_estack.c
///  @brief   Functions that handle the expression stack.
///
///  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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
    x.opflag  = false;
    x.operand = x.operands;
    x.type    = x.types;

    int_t *p1       = x.operand;
    enum x_type *p2 = x.type;
    uint count      = XSTACK_SIZE;

    while (count-- > 0)
    {
        *p1++ = 0;
        *p2++ = 0;
    }
}


///
///  @brief    Fetch operand from top of stack. Note that we presume that the
///            caller confirmed that an operand is available, using isoperand().
///
///  @returns  Returned operand.
///
////////////////////////////////////////////////////////////////////////////////

int_t pop_x(void)
{
    x.opflag = false;                   // No operand on top after we return

    // A leading minus sign without a previous operand is equivalent
    // to an operand of -1; any other leading operator is an error.

    if (x.level == x.base + 1 && x.type[-1] == X_MINUS)
    {
        --x.operand;
        --x.type;
        --x.level;

        return -1;
    }
    else                                // Must be operand
    {
        assert(x.level != x.base);
        assert(x.type[-1] == X_OPERAND);

        --x.type;
        --x.level;

        return *--x.operand;
    }
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

void push_x(int_t operand, enum x_type type)
{
    if (x.level == XSTACK_SIZE)
    {
        throw(E_PDO);                   // Push-down list overflow
    }

    *x.operand++ = operand;
    *x.type++    = type;

    if (++x.level == x.base + 1)
    {
        x.opflag = (type == X_OPERAND);
    }
    else
    {
        reduce();
    }
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

    if (x.level >= x.base + 1 && x.type[-1] == X_1S_COMP)
    {
        if (x.level == x.base + 1 || x.type[-1] != X_OPERAND)
        {
            throw(E_NAB);               // No argument before ^_
        }
    }

    // Set flag based on whether the top stack item is an operand
    // (or a minus sign if it's the only item on the stack).

    if (x.level != x.base &&
        (x.type[-1] == X_OPERAND ||
         (x.type[-1] == X_MINUS && x.level == x.base + 1)))
    {
        x.opflag = true;
    }
    else
    {
        x.opflag = false;
    }
}


///
///  @brief    Reduce top two items on expression stack if possible.
///
///  @returns  true if we reduced expression stack, else false.
///
////////////////////////////////////////////////////////////////////////////////

static inline bool reduce2(void)
{
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
        if ((x.type[-1] == X_NOT     && x.type[-2] == X_OPERAND) ||
            (x.type[-1] == X_OPERAND && x.type[-2] == X_OPERAND) ||
            (x.type[-1] >  X_OPERAND && x.type[-2] >  X_OPERAND))
        {
            throw(E_IFE);               // Ill-formed numeric expression
        }
    }

    if (x.type[-1] == X_OPERAND && x.type[-2] == X_PLUS)
    {
        x.operand[-2] = x.operand[-1];
        x.type[-2] = X_OPERAND;

        --x.operand;
        --x.type;
        --x.level;
    }
    else if (x.type[-1] == X_OPERAND && x.type[-2] == X_MINUS)
    {
        x.operand[-2] = -x.operand[-1];
        x.type[-2] = X_OPERAND;

        --x.operand;
        --x.type;
        --x.level;
    }
    else if (x.type[-1] == X_OPERAND && x.type[-2] == X_NOT)
    {
        // Logical NOT yields -1 for true and 0 for false.

        x.operand[-2] = !x.operand[-1] ? SUCCESS : FAILURE;
        x.type[-2] = X_OPERAND;

        --x.operand;
        --x.type;
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
    // Reduce (x) to x

    if (x.type[-3] == X_LPAREN && x.type[-2] == X_OPERAND &&
        x.type[-1] == X_RPAREN)
    {
        x.operand[-3] = x.operand[-2];
        x.type[-3]    = X_OPERAND;

        x.operand -= 2;
        x.type    -= 2;
        x.level   -= 2;

        return true;
    }

    // Anything else has to be of the form x <operator> y.

    if (x.type[-3] != X_OPERAND || x.type[-2] == X_OPERAND ||
        x.type[-1] != X_OPERAND)
    {
        return false;
    }

    // Here to process arithmetic and logical operators

    switch ((int)x.type[-2])
    {
        case X_PLUS:
            x.operand[-3] += x.operand[-1];

            break;

        case X_MINUS:
            x.operand[-3] -= x.operand[-1];

            break;

        case X_MUL:
            x.operand[-3] *= x.operand[-1];

            break;

        case X_DIV:
            if (x.operand[-1] == 0)
            {
                if (f.e2.zero)
                {
                    throw(E_DIV);       // Division by zero
                }

                x.operand[-3] = 0;
            }
            else
            {
                x.operand[-3] /= x.operand[-1];
            }

            break;

        case X_AND:
            x.operand[-3] &= x.operand[-1];

            break;

        case X_OR:
            x.operand[-3] |= x.operand[-1];

            break;

        case X_XOR:
            x.operand[-3] ^= x.operand[-1];
            break;

        case X_REM:
            x.operand[-3] %= x.operand[-1];
            break;

        case X_EQ:
            x.operand[-3] = (x.operand[-3] == x.operand[-1]) ? -1 : 0;

            break;

        case X_NE:
            x.operand[-3] = (x.operand[-3] != x.operand[-1]) ? -1 : 0;

            break;

        case X_LT:
            x.operand[-3] = (x.operand[-3] < x.operand[-1]) ? -1 : 0;

            break;

        case X_LE:
            x.operand[-3] = (x.operand[-3] <= x.operand[-1]) ? -1 : 0;

            break;

        case X_GT:
            x.operand[-3] = (x.operand[-3] > x.operand[-1]) ? -1 : 0;

            break;

        case X_GE:
            x.operand[-3] = (x.operand[-3] >= x.operand[-1]) ? -1 : 0;

            break;

        case X_LSHIFT:
            x.operand[-3] = (int)((uint)x.operand[-3] << x.operand[-1]);

            break;

        case X_RSHIFT:
            x.operand[-3] = (int)((uint)x.operand[-3] >> x.operand[-1]);

            break;

        default:
            throw(E_ARG);               // Improper arguments
    }

    x.type[-3] = X_OPERAND;
    x.operand -= 2;
    x.type    -= 2;
    x.level   -= 2;

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

    x.base   = x.level;
    x.opflag = false;

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
    if (x.level != x.base + 1 || x.type[-1] != X_MINUS)
    {
        return false;
    }

    --x.operand;
    --x.type;
    --x.level;

    return true;
}
