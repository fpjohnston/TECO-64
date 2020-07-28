///
///  @file    estack.c
///  @brief   Functions that handle the expression stack.
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

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eflags.h"
#include "errors.h"
#include "estack.h"


///  @struct  estack
///  @brief   Expression stack used for parsing command strings.

struct estack estack;

// Local functions

static void reduce(void);

static bool reduce2(void);

static bool reduce3(void);


///
///  @brief    Check to see if we have an operand on the top of the stack.
///
///  @returns  true if operand on top of stack, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool check_expr(void)
{
    if (estack.level == estack.base ||
        estack.obj[estack.level - 1].type != EXPR_VALUE)
    {
        return false;
    }

    return true;
}


///
///  @brief    Initialize expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_expr(void)
{
    estack.base  = 0;
    estack.level = 0;

#if     0                           // TODO: temporary initialization code
    for (uint i = estack.level; i < EXPR_SIZE; ++i)
    {
        estack.obj[i].type = estack.obj[i].value = 0;
    }
#endif
}


///
///  @brief    Return whether the top of the expression stack is an operand.
///            Note: if the stack is empty, then there's obviously no operand.
///
///  @returns  true if an operand, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool pop_expr(int *operand)
{
    assert(operand != NULL);

    if (estack.level == estack.base)    // Anything on stack?
    {
        return false;                   // No
    }

    if (estack.obj[estack.level - 1].type == EXPR_VALUE)
    {
        *operand = (int)estack.obj[--estack.level].value;

        return true;
    }

    // A leading minus sign without a previous operand is treated as -1;
    // any other leading operator is an error.

    if (estack.level == estack.base + 1)
    {
        if (estack.obj[estack.level - 1].type == EXPR_MINUS)
        {
            --estack.level;

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

void push_expr(int value, enum expr_type type)
{
    if (estack.level == EXPR_SIZE)
    {
        throw(E_PDO);                   // Push-down list overflow
    }

    estack.obj[estack.level].value = value;
    estack.obj[estack.level].type = type;

    ++estack.level;

    reduce();                           // Reduce what we can
}


///
///  @brief    Reduce expression stack if possible.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reduce(void)
{
    while (estack.level > estack.base + 1)
    {
        if (!reduce3() && !reduce2())
        {
            break;
        }
    }

    if (estack.level >= estack.base + 1 &&
        estack.obj[estack.level - 1].type == '\x1F')
    {
        if (estack.level == estack.base + 1 ||
            estack.obj[estack.level - 2].type != EXPR_VALUE)
        {
            throw(E_NAB);               // No argument before ^_
        }
    }
}


///
///  @brief    Reduce top two items on expression stack if possible.
///
///  @returns  true if we reduced expression stack, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool reduce2(void)
{
    if (estack.level < estack.base + 2) // At least two items?
    {
        return false;                   // No
    }

    struct e_obj *e1 = &estack.obj[estack.level - 1];
    struct e_obj *e2 = &estack.obj[estack.level - 2];

    // The following prevents double operators in expressions such as 1++2.

    if (f.e2.oper)
    {
        if (e1->type == EXPR_NOT)
        {
            if (e2->type == EXPR_VALUE)
            {
                throw(E_IFE);           // Ill-formed numeric expression
            }
        }
        else if (e1->type != EXPR_VALUE && e1->value == TYPE_OPER &&
                 e2->type != EXPR_VALUE && e2->value == TYPE_OPER &&
                 e1->type != EXPR_MINUS)
        {
            throw(E_IFE);               // Ill-formed numeric expression
        }
    }

    if (e1->type == EXPR_VALUE && e2->type != EXPR_VALUE)
    {
        if (e2->type == EXPR_PLUS)
        {
            e2->value = e1->value;
            e2->type = EXPR_VALUE;

            --estack.level;
        }
        else if (e2->type == EXPR_MINUS)
        {
            e2->value = -e1->value;
            e2->type = EXPR_VALUE;

            --estack.level;
        }
        else if (e2->type == EXPR_NOT)
        {
            // Logical NOT yields either -1 for true or 0 for false.

            e2->value = !e1->value ? -1 : 0;
            e2->type = EXPR_VALUE;

            --estack.level;
        }
        else
        {
            return false;
        }
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

static bool reduce3(void)
{
    if (estack.level < estack.base + 3) // At least three items?
    {
        return false;                   // No
    }

    struct e_obj *e1 = &estack.obj[estack.level - 1];
    struct e_obj *e2 = &estack.obj[estack.level - 2];
    struct e_obj *e3 = &estack.obj[estack.level - 3];

    // Reduce (x) and {x} to x

    if (((e3->type == '(' && e1->type == ')') ||
         (e3->type == '{' && e1->type == '}')) &&
        e2->type == EXPR_VALUE)
    {
        e3->value = e2->value;
        e3->type = EXPR_VALUE;

        estack.level -= 2;

        return true;
    }

    // Anything else has to be of the form x <operator> y.

    if (e3->type != EXPR_VALUE ||
        e2->type == EXPR_VALUE ||
        e1->type != EXPR_VALUE)
    {
        return false;
    }

    // Here to process arithmetic and logical operators

    switch ((int)e2->type)
    {
        case EXPR_PLUS:
            e3->value += e1->value;

            break;

        case EXPR_MINUS:
            e3->value -= e1->value;

            break;

        case EXPR_MUL:
            e3->value *= e1->value;

            break;

        case EXPR_DIV:
            if (e1->value == 0)
            {
                if (f.e2.zero)
                {
                    throw(E_DIV);       // Division by zero
                }

                e3->value = 0;
            }
            else
            {
                e3->value /= e1->value;
            }

            break;

        case EXPR_AND:
            e3->value &= e1->value;

            break;

        case EXPR_OR:
            e3->value |= e1->value;

            break;

        case EXPR_XOR:
            e3->value ^= e1->value;
            break;

        case EXPR_REM:
            e3->value %= e1->value;
            break;

        case EXPR_EQ:
            e3->value = (e3->value == e1->value) ? -1 : 0;

            break;

        case EXPR_NE:
            e3->value = (e3->value != e1->value) ? -1 : 0;

            break;

        case EXPR_LT:
            e3->value = (e3->value < e1->value) ? -1 : 0;

            break;

        case EXPR_LE:
            e3->value = (e3->value <= e1->value) ? -1 : 0;

            break;

        case EXPR_GT:
            e3->value = (e3->value > e1->value) ? -1 : 0;

            break;

        case EXPR_GE:
            e3->value = (e3->value >= e1->value) ? -1 : 0;

            break;

        case EXPR_LSHIFT:
            e3->value = (int)((uint)e3->value << e1->value);

            break;

        case EXPR_RSHIFT:
            e3->value = (int)((uint)e3->value >> e1->value);

            break;

        default:
            throw(E_ARG);               // Improper arguments
    }

    e3->type = EXPR_VALUE;
    estack.level -= 2;

    return true;
}


///
///  @brief    Reset base of expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_expr(uint base)
{
    estack.base = base;

    reduce();
}


///
///  @brief    Set base of expression stack.
///
///  @returns  Old base.
///
////////////////////////////////////////////////////////////////////////////////

uint set_expr(void)
{
    uint base = estack.base;

    estack.base = estack.level;

    return base;
}
