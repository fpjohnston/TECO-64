///
///  @file    loop_cmd.c
///  @brief   Execute loop commands.
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

#include "teco.h"
#include "cbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "term.h"


#define NO_POP      (bool)false     ///< Pop loop stack at end o  f loop
#define POP_OK      (bool)true      ///< Don't pop loop stack at end of loop

#define INFINITE        (-1)        ///< Infinite loop count

///  @struct loop
///  @brief  Linked list structure for loops

struct loop
{
    struct loop *next;              ///< Next item in list
    int_t count;                    ///< Iteration count
    uint_t start;                   ///< Starting position
    uint depth;                     ///< Depth of if statements
};

#define STACK_MAX  20               ///< Maximum items in loop stack

//  Define the root structure for keeping track of loops. Nested loops are
//  handled by dynamically allocating loop structures, as defined above, and
//  putting them in a linked list. When the loops terminate, we push these
//  structures on an internal stack rather than deallocating them, since we
//  are likely to need them again, and this method avoids unnecessary use
//  of the heap. The high-water mark is intended to assist in analyzing
//  command strings to see if the stack maximum may need adjustment. Note
//  that if the stack fills up, we will still handle loops properly, as
//  terminated loop structures will just be returned to the heap instead of
//  being pushed on the stack.
//
//  When TECO exits, all loop structures, either in the linked list or on
//  the stack, will be deallocated.
 
///  @struct loop_root
///  @brief  Root structure for controlling loop commands

struct loop_root
{
    struct loop *head;              ///< Head of list
    struct loop *stack[STACK_MAX];  ///< Discarded list items
    uint nstack;                    ///< No. of saved items on stack
    uint hi_mark;                   ///< High-water mark for stack
    uint nloops;                    ///< Loop nesting depth
};

///  @var    loop_root
///  @brief  Root structure for controlling loop commands

static struct loop_root loop_root =
{
    .head    = NULL,
    .stack   = { NULL },
    .nstack  = 0,
    .hi_mark = 0,
    .nloops  = 0,
};

// Local functions

static void endloop(struct cmd *cmd, bool pop_ok);

static void pop_loop(bool pop_ok);

static void push_loop(int_t count);


///
///  @brief    Check to see if we're in a loop.
///
///  @returns  true if we're in a loop, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool check_loop(void)
{
    if (loop_root.head != NULL)
    {
        return true;
    }
    else
    {
        return false;
    }
}


///
///  @brief    Flow to end of loop
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void endloop(struct cmd *cmd, bool pop_ok)
{
    assert(cmd != NULL);

    uint depth = 1;                     // Nesting depth

    do
    {
        if (!skip_cmd(cmd, "\"'<>"))
        {
            throw(E_MRA);               // Missing right angle bracket
        }

        if (f.e2.loop)
        {
            if (f.e2.quote)
            {
                if (cmd->c1 == '"')
                {
                    setif_depth(getif_depth() + 1);
                }
                else if (cmd->c1 == '\'')
                {
                    setif_depth(getif_depth() - 1);
                }
            }

            if (loop_root.head != NULL && loop_root.head->depth > getif_depth())
            {
                throw(E_MAP);           // Missing apostrophe
            }
        }

        if (cmd->c1 == '<')             // Start of a new loop?
        {
            ++depth;
        }
        else if (cmd->c1 == '>')        // End of a loop?
        {
            --depth;
        }
    } while (depth > 0);

    pop_loop(pop_ok);
}


///
///  @brief    Execute "F>" command: flow to end of loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_gt(struct cmd *cmd)
{
    assert(cmd != NULL);

    endloop(cmd, POP_OK);               // Flow to end of loop

    init_x();                           // Reinitialize expression stack
}


///
///  @brief    Execute "F<" command: flow to start of loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_lt(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (loop_root.nloops == 0)          // Outside of loop?
    {
        cbuf->pos = 0;                  // Yes, reset to start of command
    }
    else
    {
        cbuf->pos = loop_root.head->start; // Just restart the loop
    }

    init_x();                           // Reinitialize expression stack

    return;
}


///
///  @brief    Execute ">" command: end of loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_gt(struct cmd *cmd)
{
    assert(cmd != NULL);

    struct loop *loop = loop_root.head;

    if (loop == NULL)
    {
        throw(E_BNI);                   // Right angle bracket not in iteration
    }

    if (f.e2.loop)
    {
        if (loop->depth != getif_depth())
        {
            throw(E_MAP);               // Missing apostrophe
        }
    }

    if (loop->count == INFINITE || --loop->count > 0)
    {
        cbuf->pos = loop->start;        // Go back to start of loop
    }
    else
    {
        pop_loop(POP_OK);
    }

    init_x();                           // Reinitialize expression stack
}


///
///  @brief    Execute "<" command: start of loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_lt(struct cmd *cmd)
{
    assert(cmd != NULL);

    int_t count = INFINITE;             // Assume infinite loop

    if (cmd->n_set && (count = cmd->n_arg) <= 0)
    {
        endloop(cmd, NO_POP);           // End loop if count is <= 0
    }
    else
    {
        push_loop(count);
    }

    init_x();                           // Reinitialize expression stack
}


///
///  @brief    Execute ";" command: exit loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_semi(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (loop_root.head == NULL)
    {
        throw(E_SNI);                   // Semi-colon not in loop
    }
    else if (!cmd->n_set)
    {
        throw(E_NAS);                   // No argument before semi-colon
    }

    if (cmd->colon)                     // n:; command
    {
        if (cmd->n_arg >= 0)            // End loop if n < 0
        {
            return;
        }
    }
    else                                // n; command
    {
        if (cmd->n_arg < 0)             // End loop if n >= 0
        {
            return;
        }
    }

    endloop(cmd, POP_OK);

    init_x();                           // Reinitialize expression stack
}


///
///  @brief    Get current loop depth.
///
///  @returns  Loop depth.
///
////////////////////////////////////////////////////////////////////////////////

uint getloop_depth(void)
{
    return loop_root.nloops;
}


///
///  @brief    Get current loop start.
///
///  @returns  Starting position of loop (EOF if not in a loop).
///
////////////////////////////////////////////////////////////////////////////////

uint getloop_start(void)
{
    if (loop_root.nloops != 0)
    {
        return loop_root.head->start;
    }
    else
    {
        return (uint)EOF;
    }
}


///
///  @brief    Pop loop block from linked list stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void pop_loop(bool pop_ok)
{
    struct loop *loop;

    if (pop_ok && (loop = loop_root.head) != NULL)
    {
        loop_root.head = loop->next;

        if (loop_root.nstack >= STACK_MAX)
        {
            free_mem(&loop);
        }
        else
        {
            loop_root.stack[loop_root.nstack++] = loop;

            if (loop_root.hi_mark < loop_root.nstack)
            {
                loop_root.hi_mark = loop_root.nstack;
            }
        }

        assert(loop_root.nloops > 0);   // Error if not in loop

        --loop_root.nloops;
    }
}


///
///  @brief    Push loop block onto linked list stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void push_loop(int_t count)
{
    struct loop *loop;

    if (loop_root.nstack > 0)
    {
        loop = loop_root.stack[--loop_root.nstack];
    }
    else
    {
        loop = alloc_mem((uint_t)sizeof(*loop));
    }

    loop->count = count;
    loop->start = cbuf->pos;
    loop->next  = loop_root.head;
    loop->depth = getif_depth();

    loop_root.head   = loop;

    ++loop_root.nloops;
}


///
///  @brief    Reset loop structures.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_loop(void)
{
    struct loop *loop;

    while ((loop = loop_root.head) != NULL)
    {
        loop_root.head = loop->next;

        free_mem(&loop);
    }

    while (loop_root.nstack > 0)
    {
        free_mem(&loop_root.stack[--loop_root.nstack]);
    }

    loop_root.nloops = 0;
}


///
///  @brief    Scan ">" command: relational operator.
///
///  @returns  true if extended operator found, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_gt(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!f.e1.xoper || nparens == 0)
    {
        (void)parse_escape(cmd);

        return false;
    }

    (void)parse_oper(cmd);

    // The following is necessary to handle the situation where a '>' command
    // is a relational operator (or part therefore) and not the end of a loop.

    if (cmd->n_set)
    {
        cmd->n_set = false;

        push_x(cmd->n_arg, X_OPERAND);
    }

    int c;

    if ((c = peek_cbuf()) == '=')       // >= operator
    {
        next_cbuf();
        trace_cbuf(c);

        cmd->c2 = (char)c;

        push_x(OPER, X_GE);
    }
    else if ((c = peek_cbuf()) == '>')  // >> operator
    {
        next_cbuf();
        trace_cbuf(c);

        cmd->c2 = (char)c;

        push_x(OPER, X_RSHIFT);
    }
    else                                // > operator
    {
        push_x(OPER, X_GT);
    }

    return true;
}


///
///  @brief    Scan "<" command: relational operator or start of loop.
///
///  @returns  true if extended operator found, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_lt(struct cmd *cmd)
{
    assert(cmd != NULL);

    // "<" is a relational operator only if it's in parentheses; otherwise,
    // it's the start of a loop.

    if (!f.e1.xoper || nparens == 0)
    {
        reject_m(cmd);
        reject_colon(cmd);
        reject_atsign(cmd);

        return false;
    }

    // The following is necessary to handle the situation where a '<' command
    // is a relational operator (or part therefore) and not the start of a loop.

    if (cmd->n_set)
    {
        cmd->n_set = false;

        push_x(cmd->n_arg, X_OPERAND);
    }

    int c = peek_cbuf();

    if (c == '=')                       // <= operator
    {
        next_cbuf();
        trace_cbuf(c);

        cmd->c2 = (char)c;

        push_x(OPER, X_LE);
    }
    else if (c == '>')                  // <> operator
    {
        next_cbuf();
        trace_cbuf(c);

        cmd->c2 = (char)c;

        push_x(OPER, X_NE);
    }
    else if (c == '<')                  // << operator
    {
        next_cbuf();
        trace_cbuf(c);

        cmd->c2 = (char)c;

        push_x(OPER, X_LSHIFT);
    }
    else                                // < operator
    {
        push_x(OPER, X_LT);
    }

    return true;
}


///
///  @brief    Set current loop depth.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void setloop_depth(uint depth)
{
    loop_root.nloops = depth;
}
