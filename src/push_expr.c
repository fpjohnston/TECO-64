///
///  @file    push_expr.c
///  @brief   Push operand or operator on expression stack.
///
///  @author  Nowwith Treble Software
///
///  @bug     No known bugs.
///
///  @copyright  tbd
///
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files (the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"

struct estack estack =
{
    .level = 0,
    .item[0 ... EXPR_SIZE - 1] = 0L,
    .type[0 ... EXPR_SIZE - 1] = EXPR_NONE,
};


// Local functions

static void reduce(void);

static bool reduce2(void);

static void reduce3(void);


///
///  @brief    Handle numeric argument from expression stack. We check to see
///            if there's something on the stack, and that the top element is
///            an operand. If so, we pop it off and return it whenever someone
///            needs a numeric value.
///
///  @returns  Value of argument (if error, returns to main loop).
///
////////////////////////////////////////////////////////////////////////////////

int get_n_arg(void)
{
    assert(estack.level > 0);           // Caller should check before calling

    --estack.level;

    if (estack.level == 0 && estack.type[estack.level] == EXPR_OPERATOR
        && estack.item[estack.level] == '-')
    {
        return -1;
    }

    if (estack.type[estack.level] != EXPR_OPERAND)
    {
        print_err(E_IFE);               // Ill-formed numeric expression
    }

    return (int)estack.item[estack.level];
}


///
///  @brief    Initialize expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_expr(void)
{
    estack.level = 0;
}


///
///  @brief    Return whether the top of the expression stack is an operand.
///            Note: if the stack is empty, then there's obviously no operand.
///
///  @returns  true if an operand, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool operand_expr(void)
{
    if (estack.level == 0)                  // Anything on stack?
    {
        return false;                   // No
    }

    if (estack.type[estack.level - 1] == EXPR_OPERAND)
    {
        return true;                    // Done if we have an operand
    }

    // Say we have an "operand" if there is only one thing on the stack, and
    // it's a unary operator.

    if (estack.level == 1 && estack.type[0] == EXPR_OPERATOR
        && estack.item[0] == '-')
    {
        return true;
    }

    return false;
}


///
///  @brief    Push operator or operand on expression stack.
///
///            This function pushes an item onto the expression stack. The
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

void push_expr(int item, enum expr_type type)
{
    assert(type == EXPR_OPERATOR || type == EXPR_OPERAND);

    if (estack.level == EXPR_SIZE)
    {
        print_err(E_PDO);               // Push-down list overflow
    }

    estack.item[estack.level] = item;
    estack.type[estack.level] = type;

    ++estack.level;

    reduce();                           // Reduce what we can
}


///
///  @brief    Reduce expression stack if possible.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void reduce(void)
{
    while (estack.level > 1)
    {
        if (estack.level >= 3)            // At least three operands?
        {
            reduce3();
        }

        if (estack.level >= 2)            // At least two operands?
        {
            if (!reduce2())             // Could we reduce anything?
            {
                return;                 // No, so we're done
            }
        }
    }
}


///
///  @brief    Reduce top two items on expression stack if possible.
///
///  @returns  true if we did something, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool reduce2(void)
{
    long *e1_item = &estack.item[estack.level - 1];
    long *e2_item = &estack.item[estack.level - 2];
    enum expr_type *e1_type = &estack.type[estack.level - 1];
    enum expr_type *e2_type = &estack.type[estack.level - 2];

    if (*e1_type == EXPR_OPERAND && *e2_type == EXPR_OPERATOR)
    {
        if (*e2_item == '+')
        {
            *e2_item = *e1_item;
            *e2_type = EXPR_OPERAND;

            --estack.level;
        }
        else if (*e2_item == '-')
        {
            *e2_item = -*e1_item;
            *e2_type = EXPR_OPERAND;

            --estack.level;
        }
        else
        {
            return false;
        }
    }
    else if (*e1_type == EXPR_OPERATOR && *e1_item == US)
    {
        if (estack.level == 1 || *e2_type != EXPR_OPERAND)
        {
            print_err(E_NAB);           // No argument before ^_
        }

        *e2_item = (int)(~((uint)*e2_item));

        --estack.level;
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
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void reduce3(void)
{
    long *e1_item = &estack.item[estack.level - 1];
    long *e2_item = &estack.item[estack.level - 2];
    long *e3_item = &estack.item[estack.level - 3];
    enum expr_type *e1_type = &estack.type[estack.level - 1];
    enum expr_type *e2_type = &estack.type[estack.level - 2];
    enum expr_type *e3_type = &estack.type[estack.level - 3];

    if (*e1_type == EXPR_OPERAND  &&
        *e2_type == EXPR_OPERATOR &&
        *e3_type == EXPR_OPERAND  &&
        *e2_item != ')' && *e2_item != '(')
    {
        switch (*e2_item)
        {
            case '+':
                *e3_item += *e1_item;

                break;

            case '-':
                *e3_item -= *e1_item;

                break;

            case '*':
                *e3_item *= *e1_item;

                break;

            case '/':
                if (*e1_item == 0)      // Division by zero?
                {
                    // Don't allow divide by zero if we're scanning expression.

                    if (scan_state == SCAN_EXPR)
                    {
                        *e1_item = 1;   // Just use a dummy result here
                    }
                    else
                    {
                        print_err(E_DIV);
                    }
                }

                *e3_item /= *e1_item;

                break;

            case '&':
                *e3_item &= *e1_item;

                break;

            case '#':
                *e3_item |= *e1_item;

                break;

            default:
                print_err(E_ARG);       // Improper arguments
        }

        estack.level -= 2;
    }
    else if (*e1_type == EXPR_OPERATOR && *e1_item == ')' &&
             *e2_type == EXPR_OPERAND  &&
             *e3_type == EXPR_OPERATOR && *e3_item == '(')
    {
        *e3_item = *e2_item;
        *e3_type = EXPR_OPERAND;

        estack.level -= 2;
    }
}
