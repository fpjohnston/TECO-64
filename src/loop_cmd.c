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


#define NO_POP      (bool)false     ///< Pop loop stack at end of loop
#define POP_OK      (bool)true      ///< Don't pop loop stack at end of loop

#define INFINITE    (0)             ///< Infinite loop count

#define MAX_LOOPS   (32)            ///< Maximum nesting level for loops

///  @struct  loop
///  @brief   Variables we need to keep track of for each loop level.

struct loop
{
    int_t count;                    ///< Iteration count for loop
    uint_t start;                   ///< Starting position of loop
    uint if_depth;                  ///< Depth of if statements
};

static struct loop loop[MAX_LOOPS]; ///< Nested loop array

static uint nloops = 0;             ///< Current loop level


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
    if (nloops != 0)
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

    uint level = 1;                     // Nesting level
    uint if_depth = getif_depth();      // Conditional depth

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

            if (nloops != 0 && loop[nloops - 1].if_depth > getif_depth())
            {
                throw(E_MAP);           // Missing apostrophe
            }
        }

        if (cmd->c1 == '<')             // Start of a new loop?
        {
            ++level;
        }
        else if (cmd->c1 == '>')        // End of a loop?
        {
            --level;
        }
    } while (level > 0);

    setif_depth(if_depth);

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

    if (nloops == 0)                    // Outside of loop?
    {
        cbuf->pos = cbuf->len;          // Yes, end the command string
    }
    else
    {
        endloop(cmd, POP_OK);           // Flow to end of loop

        init_x();                       // Reinitialize expression stack
    }
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

    if (nloops == 0)                    // Outside of loop?
    {
        cbuf->pos = 0;                  // Yes, reset to start of command
    }
    else                                // No, restart the loop
    {
        cbuf->pos = loop[nloops - 1].start;
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

    if (nloops == 0)
    {
        throw(E_BNI);                   // Right angle bracket not in iteration
    }

    if (f.e2.loop)
    {
        if (loop[nloops - 1].if_depth != getif_depth())
        {
            throw(E_MAP);               // Missing apostrophe
        }
    }

    if (loop[nloops - 1].count == INFINITE || --loop[nloops - 1].count > 0)
    {
        cbuf->pos = loop[nloops - 1].start;
                                        // Go back to start of loop
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

    if (nloops == 0)
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
    return nloops;
}


///
///  @brief    Get current loop start.
///
///  @returns  Starting position of loop (EOF if not in a loop).
///
////////////////////////////////////////////////////////////////////////////////

uint_t getloop_start(void)
{
    if (nloops != 0)
    {
        return loop[nloops - 1].start;
    }
    else
    {
        return (uint_t)EOF;
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
    if (pop_ok && nloops != 0)
    {
        --nloops;
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
    if (nloops == MAX_LOOPS)
    {
        throw(E_MAX);                   // Maximum nesting level reached
    }

    loop[nloops].count = count;
    loop[nloops].start = cbuf->pos;
    loop[nloops].if_depth = getif_depth();

    ++nloops;
}


///
///  @brief    Reset loop structures.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_loop(void)
{
    nloops = 0;
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
    assert(depth < MAX_LOOPS);

    nloops = depth;
}
