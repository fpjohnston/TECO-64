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
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"


#if     defined(INLINE)

#undef INLINE
#define INLINE  inline              ///< Enable inline functions

#else

#define INLINE                      ///< Disable inline functions

#endif

///  @enum   order
///  @brief  Definitions for operator associativity

enum order
{
    NONE,
    LEFT,
    RIGHT
};

///  @struct   oper
///  @brief    Definition of characteristics for each operator, including the
//             precedence, number of required operands, and associativity order
//             (right or left).

struct oper
{
    uint operands;              ///< No. of required operands
    uint precedence;            ///< Operator precedence
    enum order order;           ///< Operator associativity
};

///  @var      oper
///  @brief    Table of operator characteristics when using standard C
///            precedence and associativity rules. Table is ordered from
///            highest to lowest precedence, but the parentheses are
///            handled specially, so their precedence isn't checked.

static const struct oper oper[X_MAX] =
{
    [X_NULL]   = { .operands = 0, .precedence =  0, .order = NONE  },
    [X_LPAREN] = { .operands = 0, .precedence =  1, .order = LEFT  }, // (
    [X_RPAREN] = { .operands = 0, .precedence =  1, .order = LEFT  }, // )
    [X_NOT]    = { .operands = 1, .precedence =  2, .order = RIGHT }, // (!x)
    [X_COMPL]  = { .operands = 1, .precedence =  2, .order = LEFT  }, // x^_
    [X_MINUS]  = { .operands = 1, .precedence =  2, .order = RIGHT }, // -x
    [X_PLUS]   = { .operands = 1, .precedence =  2, .order = RIGHT }, // +x
    [X_MUL]    = { .operands = 2, .precedence =  3, .order = LEFT  }, // x * y
    [X_DIV]    = { .operands = 2, .precedence =  3, .order = LEFT  }, // x / y
    [X_REM]    = { .operands = 2, .precedence =  3, .order = LEFT  }, // (x // y)
    [X_ADD]    = { .operands = 2, .precedence =  4, .order = LEFT  }, // x + y
    [X_SUB]    = { .operands = 2, .precedence =  4, .order = LEFT  }, // x - y
    [X_LSHIFT] = { .operands = 2, .precedence =  5, .order = LEFT  }, // (x << y)
    [X_RSHIFT] = { .operands = 2, .precedence =  5, .order = LEFT  }, // (x >> y)
    [X_GE]     = { .operands = 2, .precedence =  6, .order = LEFT  }, // (x >= y
    [X_GT]     = { .operands = 2, .precedence =  6, .order = LEFT  }, // (x > y)
    [X_LE]     = { .operands = 2, .precedence =  6, .order = LEFT  }, // (x <= y)
    [X_LT]     = { .operands = 2, .precedence =  6, .order = LEFT  }, // (x < y)
    [X_EQ]     = { .operands = 2, .precedence =  7, .order = LEFT  }, // (x == y)
    [X_NE]     = { .operands = 2, .precedence =  7, .order = LEFT  }, // (x <> y)
    [X_AND]    = { .operands = 2, .precedence =  8, .order = LEFT  }, // x & y
    [X_XOR]    = { .operands = 2, .precedence =  9, .order = LEFT  }, // (x ~ y)
    [X_OR]     = { .operands = 2, .precedence = 10, .order = LEFT  }, // x # y
};

//  The following table allows us to switch back between the operator precedence
//  in the structure above, and the behavior of classic TECO, in which all
//  operators have equal precedence. This is controlled by the f.e1.c_oper bit,
//  and the init_x() function, which is called before execution of each command
//  string. Note that we always use the operator associativity specified in C,
//  even though classic TECO states that all associativity should be left to
//  right. This is because it was determined that classic TECO simply ignores
//  any operators preceding a unary minus or plus, so using C's associativity
//  rules simply made more sense (this is also done for the logical NOT
//  operator, which did not exist in classic TECO).

static uint precedence[X_MAX];      ///< Current precedence table


#define MAX_VALUES  (64 + 1)        ///< Maximum expression values

#define MAX_OPERS   (MAX_VALUES * 2) ///< Maximum consecutive arithmetic operators


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
    struct xstack *next;            ///< Next block in linked list
    struct
    {
        int_t stack[MAX_VALUES];    ///< Operand list
        int_t *top;                 ///< Top of operand stack
        uint count;                 ///< No.of stored elements
    } number;                       ///< Operand stack
    struct
    {
        enum x_oper stack[MAX_OPERS]; ///< Operator list
        enum x_oper *top;           ///< Top of operator stack
        uint count;                 ///< No.of stored elements
        bool last;                  ///< Last element pushed was an operator
        uint nesting;               ///< Nesting level for parentheses
    } oper;                         ///< Operator stack
};


static struct xstack *x = NULL;     ///< List of expression stacks


// Local functions

static INLINE int_t exec_div(int_t a, int_t b);

static INLINE int_t exec_not(int_t a);

static INLINE int_t exec_rem(int_t a, int_t b);

static int_t fetch_val(void);


///
///  @brief    Check to see if we're in the middle of parentheses.
///
///  @returns  true if we're between parentheses, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool check_parens(void)
{
    return (x->oper.nesting != 0);
}


///
///  @brief    Restore previous expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void delete_x(void)
{
    if (x->next != NULL)
    {
        struct xstack *ptr = x;

        x = ptr->next;

        free_mem(&ptr);
    }
}


///
///  @brief    Do binary division, yielding quotient.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static INLINE int_t exec_div(int_t a, int_t b)
{
    if (b == 0)
    {
        if (f.e2.zero)
        {
            throw(E_DIV);               // Division by zero
        }

        return 0;
    }
    else
    {
        return a / b;
    }
}


///
///  @brief    We have seen an operator that requires existing operands. Check
///            to see if we have them, and if so, calculate the value of the
///            expression, and then push the new value on the output queue.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_oper(enum x_oper type)
{
    const uint operands = oper[type].operands;
    int_t a = 0, b = 0, n;

    assert(operands <= 2);

    if (operands > x->number.count)     // Need more operands than we have?
    {
        if (!f.e0.skip)                 // Are we skipping commands??
        {
            throw(E_IFE);               // Ill-formed expression
        }
        else                            // Not executing, so just return
        {
            return;
        }
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
        case X_ADD:    n = a + b;                 break;
        case X_AND:    n = a & b;                 break;
        case X_COMPL:  n = (int_t)~(uint_t)a;       break;
        case X_DIV:    n = exec_div(a, b);        break;
        case X_EQ:     n = (a == b ? -1 : 0);     break;
        case X_GE:     n = (a >= b ? -1 : 0);     break;
        case X_GT:     n = (a > b ? -1 : 0);      break;
        case X_LE:     n = (a <= b ? -1 : 0);     break;
        case X_LSHIFT: n = (int_t)((uint_t)a << b); break;
        case X_LT:     n = (a < b ? -1 : 0);      break;
        case X_MINUS:  n = -a;                    break;
        case X_MUL:    n = a * b;                 break;
        case X_NE:     n = (a != b ? -1 : 0);     break;
        case X_NOT:    n = exec_not(a);           break;
        case X_OR:     n = a | b;                 break;
        case X_PLUS:   n = a;                     break;
        case X_REM:    n = exec_rem(a, b);        break;
        case X_RSHIFT: n = (int_t)((uint_t)a >> b); break;
        case X_SUB:    n = a - b;                 break;
        case X_XOR:    n = a ^ b;                 break;

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
///  @brief    Do unary negation.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static INLINE int_t exec_not(int_t a)
{
    while (x->oper.count != 0)
    {
        a = (a != 0) ? 0 : -1;

        if (x->oper.count != 0 && x->oper.top[-1] == X_NOT)
        {
            --x->oper.count;
            --x->oper.top;
        }
        else
        {
            break;
        }
    }

    return a;
}


///
///  @brief    Do binary divison, yielding remainder.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static INLINE int_t exec_rem(int_t a, int_t b)
{
    if (b == 0)
    {
        if (f.e2.zero)
        {
            throw(E_DIV);               // Division by zero
        }

        return 0;
    }
    else
    {
        return a % b;
    }
}


///
///  @brief    Deallocate memory for expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exit_x(void)
{
    if (x != NULL)
    {
        do
        {
            delete_x();                    // Free up temporary stacks
        } while (x->next != NULL);

        free_mem(&x);                   // Now delete root stack
    }
}


///
///  @brief    Fetch value from top of output stack.
///
///  @returns  Returned value (error thrown if none available).
///
////////////////////////////////////////////////////////////////////////////////

static int_t fetch_val(void)
{
    if (x->number.count-- != 0)         // Anything to fetch?
    {
        return *--x->number.top;
    }
    else if (f.e0.skip)                 // Are we skipping commands?
    {
        x->number.count = 0;            // Yes, so reset stack

        return 0;                       // And return a dummy value
    }
    else                                // No, so issue an error
    {
        throw(E_IFE);                   // Ill-formed expression
    }
}


///
///  @brief    Initialize or reinitialize expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_x(void)
{
    // If user changed operator precedence, then update our table.

    if (f.e1.c_oper)
    {
        if (precedence[X_ADD] == 0)
        {
            for (uint i = 0; i < countof(precedence); ++i)
            {
                precedence[i] = oper[i].precedence;
            }
        }
    }
    else
    {
        if (precedence[X_ADD] != 0)
        {
            for (uint i = 0; i < countof(precedence); ++i)
            {
                precedence[i] = 0;
            }
        }
    }

    if (x == NULL)
    {
        x = alloc_mem((uint_t)sizeof(*x));

        x->next = NULL;

        reset_x();
    }
    else
    {
        struct xstack *next;

        while ((next = x->next) != NULL)
        {
            free_mem(&x);

            x = next;
        }

        reset_x();
    }
}


///
///  @brief    Set new expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void new_x(void)
{
    struct xstack *p = alloc_mem((uint_t)sizeof(*p));

    p->next = x;
    x = p;

    reset_x();
}


///
///  @brief    Push operator on stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static INLINE void push_oper(enum x_oper o1)
{
    if (x->oper.count++ == MAX_OPERS)
    {
        throw(E_PDO);                   // Push-down list overflow
    }

    *x->oper.top++ = o1;
    x->oper.last = true;
}


///
///  @brief    Check to see if there's a value (operand) available on stack.
///
///  @returns  true if we returned a value, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool query_x(int_t *n)
{
    assert(n != NULL);

    if (x->number.count == 0 || x->oper.last)
    {
        return false;
    }

    enum x_oper type;

    while (x->oper.count != 0 && (type = x->oper.top[-1]) != X_LPAREN)
    {
        --x->oper.count;
        --x->oper.top;

        exec_oper(type);
    }

    // Here when we are either out of operators, or the first operator on the
    // stack is a left parenthesis.

    *n = fetch_val();

    // Because of such things as values being passed through macros, we can end
    // up with multiple values on the output queue if there are no operators on
    // the stack. For example, with a command such as "42MB 27UA QA=". If this
    // happens, we should reset the output queue.

    if (x->oper.count == 0)
    {
        x->oper.last = false;
        x->number.count = 0;
        x->number.top = x->number.stack;
    }

    return true;
}


///
///  @brief    Reset an expression stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_x(void)
{
    assert(x != NULL);

#if     defined(DEBUG)          // Reset expression stack data

    for (uint i = 0; i < countof(x->number.stack); ++i)
    {
        x->number.stack[i] = 0;
    }

    for (uint i = 0; i < countof(x->oper.stack); ++i)
    {
        x->oper.stack[i] = X_NULL;
    }

#endif

    x->number.top = x->number.stack;
    x->number.count = 0;
    x->oper.top = x->oper.stack;
    x->oper.count = 0;
    x->oper.last = false;
    x->oper.nesting = 0;

#if     !defined(NSTRICT)

    f.e0.digit = false;

#endif

}


///
///  @brief    Scan ( command: expression grouping.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_left(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

#if     !defined(NSTRICT)

    f.e0.digit = false;                 // Last command wasn't a digit

#endif

    // The following means that a command string such as mmm (nnn) just has a
    // value of nnn; the mmm is discarded. This allows for enclosing Q and other
    // commands in parentheses to guard against being affected (and the command
    // possibly being completely changed) by a preceding value. In the case of
    // a Q command, for example, instead of returning the numeric value in the
    // Q-register, a preceding value would return the ASCII value of the nth
    // character in the Q-register text string.

    int_t n;

    (void)query_x(&n);                  // Discard any operand on stack

    ++x->oper.nesting;

    push_oper(X_LPAREN);

    return true;
}


///
///  @brief    Scan ) command: expression grouping.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_right(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

#if     !defined(NSTRICT)

    f.e0.digit = false;                 // Last command wasn't a digit

#endif

    // Try to process everything since the last left parenthesis

    enum x_oper type;

    while (x->oper.count-- != 0)
    {
        if ((type = *--x->oper.top) == X_LPAREN)
        {
            assert(x->oper.nesting != 0);

            --x->oper.nesting;

            // We found a matching left parenthesis. Try to process any
            // operators preceding it, up to any previous left parenthesis.

            while (x->oper.count != 0)
            {
                if ((type = *--x->oper.top) == X_LPAREN)
                {
                    ++x->oper.top;

                    return true;        // Say that we found an operator
                }
                else
                {
                    --x->oper.count;

                    exec_oper(type);    // Process the operator
                }
            }

            return true;                // Say that we found an operator
        }
        else
        {
            exec_oper(type);            // Process the operator
        }
    }

    // Here if we ran out of operators before finding a left parenthesis

    throw(E_MLP);                       // Missing left parenthesis
}


///
///  @brief    Like query_x(), but checks for unary minus as final operator.
///            Use of this function allows for commands such as -P to be
///            treated as equivalent to -1P.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_x(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (query_x(&cmd->n_arg))
    {
        cmd->n_set = true;
    }
    else if (!cmd->n_set)
    {
        if (x->oper.count != 0 && x->oper.top[-1] == X_MINUS)
        {
            --x->oper.count;
            --x->oper.top;

            cmd->n_set = true;
            cmd->n_arg = -1;
        }
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
    //  If the last item processed was an operator, or if we have not processed
    //  a number, then we have a unary plus operator; otherwise, we have a
    //  binary addition operator.

    if (x->oper.last || x->number.count == 0)
    {
        store_oper(X_PLUS);
    }
    else
    {
        store_oper(X_ADD);
    }
}


///
///  @brief    Check to see if operator already on stack before storing another
///            operator.
///
///            Note that this function should never be called to process left
///            or right parentheses, as those are processed elsewhere.
///
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_oper(enum x_oper o1)
{
    enum x_oper o2;

#if     !defined(NSTRICT)

    f.e0.digit = false;                 // Last command wasn't a digit

#endif

    while (x->oper.count != 0 && ((o2 = x->oper.top[-1]) != X_LPAREN))
    {
        uint p1 = precedence[o1];
        uint p2 = precedence[o2];

        if ((p2 < p1) || (p1 == p2 && oper[o1].order == LEFT))
        {
            --x->oper.count;
            --x->oper.top;

            exec_oper(o2);
        }
        else
        {
            break;
        }
    }

    push_oper(o1);
}


///
///  @brief    Store subtraction or unary minus operator on operator stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_sub(void)
{
    //  If the last item processed was an operator, or if we have not processed
    //  a number, then we have a unary minus operator; otherwise, we have a
    //  binary subtraction operator.

    if (x->oper.last || x->number.count == 0)
    {
        store_oper(X_MINUS);
    }
    else
    {
        store_oper(X_SUB);
    }
}


///
///  @brief    Store operand in expression queue.
///
///  @returns  Nothing (return to main loop if error).
///
////////////////////////////////////////////////////////////////////////////////

void store_val(int_t value)
{

#if     !defined(NSTRICT)

    f.e0.digit = false;                 // Last command wasn't a digit

#endif

    if (x->number.count++ == MAX_VALUES)
    {
        throw(E_PDO);                   // Push-down list overflow
    }

    *x->number.top++ = value;
    x->oper.last = false;
}
