///
///  @file    cmd_estack.c
///  @brief   Functions that handle the expression stack.
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

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "cbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"


#define MAX_OPERS   8           ///< Max. no. of consecutive arithmetic operators

#define MAX_VALUES  64          ///< Max. no. of expression values

///  @enum   order
///  @brief  Definitions for operator associativity

enum order
{
    NONE,
    LEFT,
    RIGHT
};

///  @var      operator
///  @brief    Definition of operator characteristics, including the operator
//             precedence, (the same as for C), the number of required operands,
//             and the associativity order (right or left).
//
//             This table is ordered by precedence for easier reading, and not
//             by the order of the operator definitions.

static const struct
{
    uint precedence;            ///< Operator precedence
    uint operands;              ///< No. of required operands
    enum order order;           ///< Operator associativity
} operator[X_MAX] =
{
    [X_NULL]    = { .precedence =  0, .operands = 0, .order = NONE  },
    [X_LPAREN]  = { .precedence =  1, .operands = 0, .order = LEFT  }, // (
    [X_RPAREN]  = { .precedence =  1, .operands = 0, .order = LEFT  }, // )
    [X_NOT]     = { .precedence =  2, .operands = 1, .order = RIGHT }, // !x
    [X_1S_COMP] = { .precedence =  2, .operands = 1, .order = LEFT  }, // x^_
    [X_MINUS]   = { .precedence =  2, .operands = 1, .order = RIGHT }, // -x
    [X_PLUS]    = { .precedence =  2, .operands = 1, .order = RIGHT }, // +x
    [X_MUL]     = { .precedence =  3, .operands = 2, .order = LEFT  }, // x * y
    [X_DIV]     = { .precedence =  3, .operands = 2, .order = LEFT  }, // x / y
    [X_REM]     = { .precedence =  3, .operands = 2, .order = LEFT  }, // (x // y)
    [X_ADD]     = { .precedence =  4, .operands = 2, .order = LEFT  }, // x + y
    [X_SUB]     = { .precedence =  4, .operands = 2, .order = LEFT  }, // x - y
    [X_LSHIFT]  = { .precedence =  5, .operands = 2, .order = LEFT  }, // (x << y)
    [X_RSHIFT]  = { .precedence =  5, .operands = 2, .order = LEFT  }, // (x >> y)
    [X_GE]      = { .precedence =  6, .operands = 2, .order = LEFT  }, // (x >= y
    [X_GT]      = { .precedence =  6, .operands = 2, .order = LEFT  }, // (x > y)
    [X_LE]      = { .precedence =  6, .operands = 2, .order = LEFT  }, // (x <= y)
    [X_LT]      = { .precedence =  6, .operands = 2, .order = LEFT  }, // (x < y)
    [X_EQ]      = { .precedence =  7, .operands = 2, .order = LEFT  }, // (x == y)
    [X_NE]      = { .precedence =  7, .operands = 2, .order = LEFT  }, // (x <> y)
    [X_AND]     = { .precedence =  8, .operands = 2, .order = LEFT  }, // x & y
    [X_XOR]     = { .precedence =  9, .operands = 2, .order = LEFT  }, // (x ~ y)
    [X_OR]      = { .precedence = 10, .operands = 2, .order = LEFT  }, // x # y
};


///  @struct   xstack
///  @brief    Definition of expression stack, used to process arithmetic
///            expressions with operands and operators, including parentheses.
///            The data below facilitates the create of the shunting yard
///            algorithm first described by Edsger Dijkstra as a way to convert
///            standard infix notation to RPN notation. The approach taken in
///            the code below avoids the storage of operators in the output
///            (or value) stack by instead applying relevant operators to the
///            operands already stored as the operators are popped off the
///            operator stack.
///
///            We also apply for a linked list of stacks, since some TECO
///            commands, such as those that process macros, require their
///            own context with their own expression stack.
///

struct xstack
{
    struct xstack *next;            ///< Next expression stack block
    int_t value[MAX_VALUES];        ///< Stored numeric values
    uint nvalues;                   ///< No.of stored values
    enum x_oper oper[MAX_OPERS];    ///< Stored operators
    uint nopers;                    ///< No. of stored operators
    bool operand;                   ///< Last item seen was an operand
};


static struct xstack *x = NULL;     ///< List of expression stacks

// Local functions

static void exec_oper(void);

static int_t fetch_val(void);

static struct xstack *make_x(void);

static void reset_x(struct xstack *stack);


///
///  @brief    Check to see if there's a value (operand) available on stack.
///
///  @returns  true if operand next in output queue, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool check_x(int_t *n)
{
    if (!x->operand)
    {
        return false;
    }

    while (x->nopers > 0 && x->oper[x->nopers - 1] != X_LPAREN)
    {
        exec_oper();
    }

    if (x->nvalues == 0)
    {
        return false;
    }

    if (n != NULL)
    {
        *n = fetch_val();
    }

    if (x->nopers == 0)
    {
        x->nvalues = 0;
    }

    if (x->nvalues == 0)
    {
        x->operand = false;
    }

    return true;
}


///
///  @brief    Push value onto output queue.
///
///  @returns  true if we could pop operator from stack, else false.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_oper(void)
{
    assert(x->nopers > 0);

    enum x_oper type = x->oper[x->nopers - 1];
    uint operands = operator[type].operands;
    int_t a = 0, b = 0, n;

    assert(operands <= 2);

    --x->nopers;

    if (operands > x->nvalues)          // Need more operands than we have?
    {
        throw(E_IFE);                   // Ill-formed expression
    }
    else if (operands == 1)             // Unary operator
    {
        a = fetch_val();
    }
    else if (operands == 2)             // Binary operator
    {
        b = fetch_val();
        a = fetch_val();
    }

    switch (type)
    {
        case X_1S_COMP: n = (int_t)~(uint)a;       break;
        case X_ADD:     n = a + b;                 break;
        case X_AND:     n = a & b;                 break;
        case X_EQ:      n = (a == b ? -1 : 0);     break;
        case X_GE:      n = (a >= b ? -1 : 0);     break;
        case X_GT:      n = (a > b ? -1 : 0);      break;
        case X_LE:      n = (a <= b ? -1 : 0);     break;
        case X_LSHIFT:  n = (int_t)((uint)a << b); break;
        case X_LT:      n = (a < b ? -1 : 0);      break;
        case X_SUB:     n = a - b;                 break;
        case X_MUL:     n = a * b;                 break;
        case X_NE:      n = (a != b ? -1 : 0);     break;
        case X_OR:      n = a | b;                 break;
        case X_RSHIFT:  n = (int_t)((uint)a >> b); break;
        case X_MINUS:   n = -a;                    break;
        case X_PLUS:    n = a;                     break;
        case X_XOR:     n = a ^ b;                 break;

        case X_DIV:
            if (b == 0)
            {
                if (f.e2.zero)
                {
                    throw(E_DIV);       // Division by zero
                }

                n = 0;
            }
            else
            {
                n = a / b;
            }

            break;

        case X_REM:
            if (b == 0)
            {
                if (f.e2.zero)
                {
                    throw(E_DIV);       // Division by zero
                }

                n = 0;
            }
            else
            {
                n = a % b;
            }

            break;

        case X_NOT:
            while (x->nopers > 0)
            {
                a = (a != 0) ? 0 : -1;

                if (x->nopers > 0 && x->oper[x->nopers - 1] == X_NOT)
                {
                    --x->nopers;
                }
                else
                {
                    break;
                }
            }

            n = a;

            break;

        case X_NULL:
        case X_LPAREN:
        case X_RPAREN:
        case X_MAX:
        default:
            throw(E_ARG);
    }

    store_val(n);                       // Save the calculated value
}


///
///  @brief    Deallocate memory for expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exit_x(void)
{
    do
    {
        pop_x();                        // Free up temporary stacks
    } while (x->next != NULL);

    free_mem(&x);                       // Now delete root stack
}


///
///  @brief    Fetch value from top of output stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static int_t fetch_val(void)
{
    if (x->nvalues == 0)
    {
        throw(E_IFE);                   // Ill-formed expression
    }

    return x->value[--x->nvalues];
}


///
///  @brief    Initialize or reinitialize expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_x(void)
{
    if (x == NULL)
    {
        x = make_x();
    }
    else
    {
        struct xstack *next;

        while ((next = x->next) != NULL)
        {
            free_mem(&x);

            x = next;
        }

        reset_x(x);
    }
}


///
///  @brief    Create a new expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static struct xstack *make_x(void)
{
    struct xstack *stack = alloc_mem((uint)sizeof(*stack));

    stack->next = NULL;

    reset_x(stack);

    return stack;
}


///
///  @brief    Restore previous expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void pop_x(void)
{
    if (x->next != NULL)
    {
        struct xstack *stack = x;

        x = x->next;

        free_mem(&stack);
    }
}


///
///  @brief    Set new expression stack.
///
///  @returns  Nothing
///
////////////////////////////////////////////////////////////////////////////////

void push_x(void)
{
    struct xstack *stack = make_x();

    stack->next = x;

    x = stack;
}


///
///  @brief    Reset an expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void reset_x(struct xstack *stack)
{
    assert(stack != NULL);

    for (uint i = 0; i < countof(stack->value); ++i)
    {
        stack->value[i] = 0;
    }

    for (uint i = 0; i < countof(stack->oper); ++i)
    {
        stack->oper[i] = X_NULL;
    }

    stack->nvalues = 0;
    stack->nopers = 0;
    stack->operand = false;
}


///
///  @brief    Scan ) command: expression grouping.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_rparen(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    if (nparens != 0)
    {
        --nparens;
    }

    // Try to process everything since the last left parenthesis

    while (x->nopers > 0 && x->oper[x->nopers - 1] != X_LPAREN)
    {
        exec_oper();
    }

    // Here to see if we have a matching parenthesis

    if (x->nopers-- == 0)
    {
        throw(E_MLP);                   // Missing left parenthesis
    }

    // Try again to process everything since the last left parenthesis

    while (x->nopers > 0 && x->oper[x->nopers - 1] != X_LPAREN)
    {
        exec_oper();
    }

    if (x->nvalues > 0 && x->nopers == 0)
    {
        x->operand = true;
    }
    else
    {
        x->operand = false;
    }

    return true;
}


///
///  @brief    Store ^_ operator on operator stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_1s_comp(void)
{
    store_oper(X_1S_COMP);

    if (x->nvalues > 0)
    {
        x->operand = true;
    }
}


///
///  @brief    Store addition or unary plus operator on operator stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_add(void)
{
    store_oper(x->operand ? X_ADD : X_PLUS);
}


///
///  @brief    Save an operator on the expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_oper(enum x_oper o1)
{
    if (o1 != X_LPAREN)
    {
        uint p1 = operator[o1].precedence;

        while (x->nopers > 0)
        {
            enum x_oper o2 = x->oper[x->nopers - 1];

            if (o2 == X_LPAREN)
            {
                break;
            }

            uint p2 = operator[o2].precedence;

            if ((p2 < p1) || (p1 == p2 && operator[o1].order == LEFT))
            {
                exec_oper();
            }
            else
            {
                break;
            }
        }
    }

    if (x->nopers == MAX_OPERS)
    {
        throw(E_PDO);                   // Push-down list overflow
    }

    x->oper[x->nopers++] = o1;

    x->operand = false;
}


///
///  @brief    Store subtraction or unary minus operator on operator stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_sub(void)
{
    store_oper(x->operand ? X_SUB : X_MINUS);
}


///
///  @brief    Store operand in expression queue.
///
///  @returns  Nothing (return to main loop if error).
///
////////////////////////////////////////////////////////////////////////////////

void store_val(int_t value)
{
    if (x->nvalues == MAX_VALUES)
    {
        throw(E_PDO);                   // Push-down list overflow
    }

    x->value[x->nvalues++] = value;
    x->operand = true;
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
    if (x->nopers > 0 && x->oper[x->nopers - 1] == X_MINUS)
    {
        --x->nopers;

        return true;
    }
    else
    {
        return false;
    }
}
