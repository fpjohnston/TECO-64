///
///  @file    loop_cmd.c
///  @brief   Execute loop commands.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"


#define NO_POP      (bool)false         ///< Pop loop stack at end of loop
#define POP_OK      (bool)true          ///< Don't pop loop stack at end of loop

#define INFINITE        (-1)            ///< Infinite loop count

uint loop_depth = 0;                    ///< Nested loop depth

///  @struct loop
///  @brief  Linked list structure for loops

struct loop
{
    struct loop *next;                  ///< Next item in list
    int count;                          ///< Iteration count
    uint start;                         ///< Starting position
    uint depth;                         ///< Depth of if statements
};

static struct loop *loop_head;          ///< Head of loop list

// Local functions

static void endloop(struct cmd *cmd, bool pop_ok);

static void exit_loop(void);

static void pop_loop(bool pop_ok);

static void push_loop(int count);


///
///  @brief    Check to see if we're in a loop.
///
///  @returns  true if we're in a loop, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool check_loop(void)
{
    if (loop_head != NULL)
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
                    ++if_depth;
                }
                else if (cmd->c1 == '\'')
                {
                    --if_depth;
                }
            }

            if (loop_head != NULL && loop_head->depth > if_depth)
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

    cbuf->pos = loop_head->start;       // Just restart the loop

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

    struct loop *loop = loop_head;

    if (loop == NULL)
    {
        throw(E_BNI);                   // Right angle bracket not in iteration
    }

    if (f.e2.loop)
    {
        if (loop->depth != if_depth)
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

    int count = INFINITE;               // Assume infinite loop

    if (cmd->n_set && (count = cmd->n_arg) <= 0)
    {
        endloop(cmd, NO_POP);           // End loop if count is <= 0
    }
    else
    {
        push_loop(count);
    }
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

    if (loop_head == NULL)
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
}


///
///  @brief    Clean up memory before we exit from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exit_loop(void)
{
    reset_loop();
}


///
///  @brief    Initialize loop structures.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_loop(void)
{
    register_exit(exit_loop);

    loop_depth = 0;
    loop_head = NULL;
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

    if (pop_ok && (loop = loop_head) != NULL)
    {
        loop_head = loop->next;

        free_mem(&loop);
    }

    assert(loop_depth > 0);             // Error if not in loop

    --loop_depth;
}


///
///  @brief    Push loop block onto linked list stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void push_loop(int count)
{
    struct loop *loop = alloc_mem((uint)sizeof(*loop));

    loop->count = count;
    loop->start = cbuf->pos;
    loop->next  = loop_head;
    loop->depth = if_depth;

    loop_head   = loop;

    ++loop_depth;
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

    while ((loop = loop_head) != NULL)
    {
        loop_head = loop->next;

        free_mem(&loop);
    }

    loop_depth = 0;
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
        return false;
    }

    int c = peek_cbuf();

    if (c == '=')                       // >= operator
    {
        (void)fetch_cbuf();

        cmd->c2 = (char)c;

        push_x(0, X_GE);
    }
    else if (c == '>')                  // >> operator
    {
        (void)fetch_cbuf();

        cmd->c2 = (char)c;

        push_x(0, X_RSHIFT);
    }
    else                                // > operator
    {
        cmd->c2 = (char)c;

        push_x(0, X_GT);
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

    int c = peek_cbuf();

    if (c == '=')                       // <= operator
    {
        (void)fetch_cbuf();

        cmd->c2 = (char)c;

        push_x(0, X_LE);
    }
    else if (c == '>')                  // <> operator
    {
        (void)fetch_cbuf();

        cmd->c2 = (char)c;

        push_x(0, X_NE);
    }
    else if (c == '<')                  // << operator
    {
        (void)fetch_cbuf();

        push_x(0, X_LSHIFT);
    }
    else                                // < operator
    {
        cmd->c2 = (char)c;

        push_x(0, X_LT);
    }
    
    return true;
}
