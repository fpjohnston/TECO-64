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

struct expr_stack expr =
{
    .stack[0 ... EXPR_SIZE - 1] = { .item = 0L, .type = EXPR_NONE },
    .len = 0,
};

int m_arg;                              // "m" argument

// Local functions

static void reduce(void);

static bool reduce2(void);

static void reduce3(void);


///
///  @brief    Check to see if expression stack is empty.
///
///  @returns  true if stack is empty, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool empty_expr(void)
{
    return (expr.len == 0);
}


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
    assert(expr.len > 0);               // Caller should check before calling

    if (expr.stack[--expr.len].type != EXPR_OPERAND)
    {
        print_err(E_IFE);               // Ill-formed numeric expression
    }

    return expr.stack[expr.len].item;
}


///
///  @brief    Initialize expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_expr(void)
{
    expr.len = 0;
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
    if (expr.len == 0 || expr.stack[expr.len - 1].type != EXPR_OPERAND)
    {
        return false;
    }

    return true;
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

    if (expr.len == EXPR_SIZE)
    {
        print_err(E_PDO);               // Push-down list overflow
    }

    expr.stack[expr.len].item = item;
    expr.stack[expr.len].type = type;

    ++expr.len;

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
    while (expr.len > 1)
    {
        if (expr.len >= 3)            // At least three operands?
        {
            reduce3();
        }

        if (expr.len >= 2)            // At least two operands?
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
    struct expr *e1 = &expr.stack[expr.len - 1];
    struct expr *e2 = &expr.stack[expr.len - 2];

    if (e1->type == EXPR_OPERAND && e2->type == EXPR_OPERATOR)
    {
        if (e2->item == '+')
        {
            e2->item = e1->item;
            e2->type = EXPR_OPERAND;

            --expr.len;
        }
        else if (e2->item == '-')
        {
            e2->item = -e1->item;
            e2->type = EXPR_OPERAND;

            --expr.len;
        }
        else
        {
            return false;
        }
    }
    else if (e1->type == EXPR_OPERATOR && e1->item == US)
    {
        if (expr.len == 1 || e2->type != EXPR_OPERAND)
        {
            print_err(E_NAB);           // No argument before ^_
        }

        e2->item = ~(e2->item);

        --expr.len;
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
    struct expr *e1 = &expr.stack[expr.len - 1];
    struct expr *e2 = &expr.stack[expr.len - 2];
    struct expr *e3 = &expr.stack[expr.len - 3];

    if (e1->type == EXPR_OPERAND  &&
        e2->type == EXPR_OPERATOR &&
        e3->type == EXPR_OPERAND  &&
        e2->item != ')' && e2->item != '(')
    {
        switch (e2->item)
        {
            case '+':
                e3->item += e1->item;

                break;

            case '-':
                e3->item -= e1->item;

                break;

            case '*':
                e3->item *= e1->item;

                break;

            case '/':
                if (e1->item == 0)      // Division by zero?
                {
                    if (f.ei.scan)      // Are we just scanning?
                    {
                        e1->item = 1;   // Yes, just let it pass
                    }
                    else
                    {
                        print_err(E_DIV);
                    }
                }

                e3->item /= e1->item;

                break;

            case '&':
                e3->item &= e1->item;

                break;

            case '#':
                e3->item |= e1->item;

                break;

            default:
                print_err(E_ARG);       // Improper arguments
        }

        expr.len -= 2;
    }
    else if (e1->type == EXPR_OPERATOR && e1->item == ')' &&
             e2->type == EXPR_OPERAND  &&
             e3->type == EXPR_OPERATOR && e3->item == '(')
    {
        e3->item = e2->item;
        e3->type = EXPR_OPERAND;

        expr.len -= 2;
    }
}
