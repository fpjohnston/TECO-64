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
    [X_NULL]    = { .operands = 0, .precedence =  0, .order = NONE  },
    [X_LPAREN]  = { .operands = 0, .precedence =  1, .order = LEFT  }, // (
    [X_RPAREN]  = { .operands = 0, .precedence =  1, .order = LEFT  }, // )
    [X_NOT]     = { .operands = 1, .precedence =  2, .order = RIGHT }, // (!x)
    [X_1S_COMP] = { .operands = 1, .precedence =  2, .order = LEFT  }, // x^_
    [X_MINUS]   = { .operands = 1, .precedence =  2, .order = RIGHT }, // -x
    [X_PLUS]    = { .operands = 1, .precedence =  2, .order = RIGHT }, // +x
    [X_MUL]     = { .operands = 2, .precedence =  3, .order = LEFT  }, // x * y
    [X_DIV]     = { .operands = 2, .precedence =  3, .order = LEFT  }, // x / y
    [X_REM]     = { .operands = 2, .precedence =  3, .order = LEFT  }, // (x // y)
    [X_ADD]     = { .operands = 2, .precedence =  4, .order = LEFT  }, // x + y
    [X_SUB]     = { .operands = 2, .precedence =  4, .order = LEFT  }, // x - y
    [X_LSHIFT]  = { .operands = 2, .precedence =  5, .order = LEFT  }, // (x << y)
    [X_RSHIFT]  = { .operands = 2, .precedence =  5, .order = LEFT  }, // (x >> y)
    [X_GE]      = { .operands = 2, .precedence =  6, .order = LEFT  }, // (x >= y
    [X_GT]      = { .operands = 2, .precedence =  6, .order = LEFT  }, // (x > y)
    [X_LE]      = { .operands = 2, .precedence =  6, .order = LEFT  }, // (x <= y)
    [X_LT]      = { .operands = 2, .precedence =  6, .order = LEFT  }, // (x < y)
    [X_EQ]      = { .operands = 2, .precedence =  7, .order = LEFT  }, // (x == y)
    [X_NE]      = { .operands = 2, .precedence =  7, .order = LEFT  }, // (x <> y)
    [X_AND]     = { .operands = 2, .precedence =  8, .order = LEFT  }, // x & y
    [X_XOR]     = { .operands = 2, .precedence =  9, .order = LEFT  }, // (x ~ y)
    [X_OR]      = { .operands = 2, .precedence = 10, .order = LEFT  }, // x # y
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

static uint precedence[X_MAX];          ///< Current precedence table


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
    uint nparens;                   ///< No. of left parentheses seen
    bool operand;                   ///< Last item seen was an operand
};


static struct xstack *x = NULL;     ///< List of expression stacks

// Local functions

static void check_oper(enum x_oper o1);

static void exec_oper(enum x_oper oper);

static int_t fetch_val(void);

static struct xstack *make_x(void);

static void reset_x(struct xstack *stack);

static void store_oper(enum x_oper type);


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

static void check_oper(enum x_oper o1)
{
    enum x_oper o2;

    while (x->nopers > 0 && ((o2 = x->oper[x->nopers - 1]) != X_LPAREN))
    {
        uint p1 = precedence[o1];
        uint p2 = precedence[o2];

        if ((p2 < p1) || (p1 == p2 && oper[o1].order == LEFT))
        {
            --x->nopers;

            exec_oper(o2);
        }
        else
        {
            break;
        }
    }

    store_oper(o1);
}


///
///  @brief    Check to see if our parenthesis count is zero.
///
///  @returns  Nothing if count is zero, else error is issued.
///
////////////////////////////////////////////////////////////////////////////////

void check_parens(void)
{
    if (x->nparens != 0)
    {
        throw(E_MRP);                   // Missing right parenthesis
    }
}


///
///  @brief    Check to see if auto detection of number radix is possible.
///
///  @returns  true if we can do auto detect of radix, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool check_radix(void)
{
    if (f.e1.radix && x->nparens != 0)  // Auto-detect radix?
    {
        return true;
    }
    else
    {
        return false;
    }
}


///
///  @brief    Check to see if there's a value (operand) available on stack.
///
///  @returns  true if operand next in output queue, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool check_x(int_t *n)
{
    assert(n != NULL);

    // Last item saved was not an operand

    if (!x->operand)
    {
        return false;
    }

    // Here if an operand was the last item processed, so we have one or more
    // operands on the output queue. If we have anything on the operator stack,
    // try to use them up.

    enum x_oper type;

    while (x->nopers > 0 && (type = x->oper[x->nopers - 1]) != X_LPAREN) 
    {
        --x->nopers;

        exec_oper(type);
    }

    // Here when we are either out of operators, or the first operator on the
    // stack is a left parenthesis. We have at least one operand available,
    // since x->operand is set, and because although exec_oper() may have used
    // up one or two operands, it will always add one to the output queue.

    *n = fetch_val();

    // Because of such things as values being passed through macros, we can end
    // up with multiple values on the output queue if there are no operators on
    // the stack. For example, with a command such as "42MB 27UA QA=". If this
    // happens, we should reset the output queue.

    if (x->nopers == 0)
    {
        x->nvalues = 0;
    }

    // Reset operand flag if we used up all operands

    if (x->nvalues == 0)
    {
        x->operand = false;
    }

    return true;
}


///
///  @brief    We have seen an operator that requires existing operands. Check
///            to see if we have them, and if so, calculate the value of the
///            expression, and then push the new value on the output queue.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_oper(enum x_oper type)
{
    const uint operands = oper[type].operands;
    int_t a = 0, b = 0, n;

    assert(operands <= 2);

    if (operands > x->nvalues)          // Need more operands than we have?
    {
        if (f.e0.exec)                  // Are we executing?
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
    if (x != NULL)
    {
        do
        {
            pop_x();                    // Free up temporary stacks
        } while (x->next != NULL);

        free_mem(&x);                   // Now delete root stack
    }
}


///
///  @brief    Fetch value from top of output stack.
///
///  @returns  Returned value.
///
////////////////////////////////////////////////////////////////////////////////

static int_t fetch_val(void)
{
    if (x->nvalues == 0)
    {
        if (f.e0.exec)                  // Are we executing?
        {
            throw(E_IFE);               // Ill-formed expression
        }
        else                            // Not executing, so just return
        {
            return 0;
        }
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

#if     defined(DEBUG)

    for (uint i = 0; i < countof(stack->value); ++i)
    {
        stack->value[i] = 0;
    }

    for (uint i = 0; i < countof(stack->oper); ++i)
    {
        stack->oper[i] = X_NULL;
    }

#endif

    stack->nvalues = 0;
    stack->nopers = 0;
    stack->nparens = 0;
    stack->operand = false;
}


///
///  @brief    Scan / command: division operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_div(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    // Check for double slash remainder operator.

    int c;

    if (f.e1.xoper && x->nparens != 0 && (c = peek_cbuf()) == '/')
    {
        next_cbuf();

        cmd->c2 = (char)c;

        check_oper(X_REM);
    }
    else
    {
        check_oper(X_DIV);
    }

    return true;
}


///
///  @brief    Scan for possible == operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_eq(struct cmd *cmd)
{
    assert(cmd != NULL);

    // If extended operators are not enabled, or we're not inside any
    // parentheses, then the equals sign is not part of an operator, but
    // is instead a command.

    if (!f.e1.xoper || x->nparens == 0)
    {
        return scan_equals(cmd);
    }

    reject_m(cmd->m_set);
    reject_m(cmd->m_set);
    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    if (require_cbuf() != '=')          // If we have one '=', we must have two
    {
        throw(E_ARG);
    }

    if (cmd->n_set)
    {
        cmd->n_set = false;

        store_val(cmd->n_arg);
    }

    check_oper(X_EQ);

    return true;
}


///
///  @brief    Scan > command: relational operator.
///
///  @returns  true if extended operator found, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_gt(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_simple(cmd);                   // > command

    // ">" is a relational operator only if it's in parentheses; otherwise,
    // it's the end of a loop.

    if (!f.e1.xoper || x->nparens == 0)
    {
        return false;
    }

    // The following is necessary to handle the situation where a '>' command
    // is a relational operator (or part of one) and not the end of a loop.

    if (cmd->n_set)
    {
        cmd->n_set = false;

        store_val(cmd->n_arg);
    }

    if (peek_cbuf() == '=')             // >= operator
    {
        next_cbuf();
        check_oper(X_GE);
    }
    else if (peek_cbuf() == '>')        // >> operator
    {
        next_cbuf();
        check_oper(X_RSHIFT);
    }
    else                                // > operator
    {
        check_oper(X_GT);
    }

    return true;
}


///
///  @brief    Scan ( command: expression grouping.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_lparen(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    // The following means that a command string such as mmm (nnn) just has a
    // value of nnn; the mmm is discarded. This allows for enclosing Q and other
    // commands in parentheses to guard against being affected (and the command
    // possibly being completely changed) by a preceding value. In the case of
    // a Q command, for example, instead of returning the numeric value in the
    // Q-register, a preceding value would return the ASCII value of the nth
    // character in the Q-register text string.

    int_t n;

    (void)check_x(&n);                  // Discard any operand on stack

    ++x->nparens;

    store_oper(X_LPAREN);

    return true;
}


///
///  @brief    Scan < command: relational operator or start of loop.
///
///  @returns  true if extended operator found, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_lt(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd->m_set);
    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    // "<" is a relational operator only if it's in parentheses; otherwise,
    // it's the start of a loop.

    if (!f.e1.xoper || x->nparens == 0)
    {
        return false;
    }

    // The following is necessary to handle the situation where a '<' command
    // is a relational operator (or part therefore) and not the start of a loop.

    if (cmd->n_set)
    {
        cmd->n_set = false;

        store_val(cmd->n_arg);
    }

    int c = peek_cbuf();

    if (c == '=')                       // <= operator
    {
        next_cbuf();
        check_oper(X_LE);
    }
    else if (c == '>')                  // <> operator
    {
        next_cbuf();
        check_oper(X_NE);
    }
    else if (c == '<')                  // << operator
    {
        next_cbuf();
        check_oper(X_LSHIFT);
    }
    else                                // < operator
    {
        check_oper(X_LT);
    }

    return true;
}


///
///  @brief    Scan for '!' operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_not(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!f.e1.xoper || x->nparens == 0) // Is it really a '!' command?
    {
        return scan_bang(cmd);
    }

    reject_colon(cmd->colon);
    reject_m(cmd->m_set);
    reject_n(cmd->n_set);
    reject_atsign(cmd->atsign);

    check_oper(X_NOT);

    return true;
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

    // Try to process everything since the last left parenthesis

    enum x_oper type;

    while (x->nopers > 0)
    {
        if ((type = x->oper[--x->nopers]) == X_LPAREN)
        {
            assert(x->nparens != 0);

            --x->nparens;

            // We found a matching left parenthesis. Try to process any
            // operators preceding it, up to any previous left parenthesis.

            while (x->nopers > 0)
            {
                if ((type = x->oper[--x->nopers]) == X_LPAREN)
                {
                    ++x->nopers;        // Recover left parenthesis

                    return true;        // Say that we found an operator
                }
                else
                {
                    exec_oper(type);    // Process the operator
                }
            }

            // Here if we're out of operators, so ensure flag is set if there
            // are still any values on the output queue.

            if (x->nvalues > 0)
            {
                x->operand = true;
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
///  @brief    Scan ~ command: exclusive OR operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_xor(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    if (f.e0.exec && (!f.e1.xoper || x->nparens == 0))
    {
        throw(E_ILL, cmd->c1);          // Illegal command
    }

    check_oper(X_XOR);

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
    check_oper(X_1S_COMP);

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
    check_oper(x->operand ? X_ADD : X_PLUS);
}


///
///  @brief    Store logical AND operator.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_and(void)
{
    check_oper(X_AND);
}


///
///  @brief    Store multiplication operator.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_mul(void)
{
    check_oper(X_MUL);
}


///
///  @brief    Store operator on stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void store_oper(enum x_oper type)
{
    if (x->nopers == MAX_OPERS)
    {
        throw(E_PDO);                   // Push-down list overflow
    }

    x->oper[x->nopers++] = type;
    x->operand = false;
}


///
///  @brief    Store logical OR operator.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_or(void)
{
    check_oper(X_OR);
}


///
///  @brief    Store subtraction or unary minus operator on operator stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_sub(void)
{
    check_oper(x->operand ? X_SUB : X_MINUS);
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
