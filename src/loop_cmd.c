///
///  @file    loop_cmd.c
///  @brief   Execute loop commands.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
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
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


// TODO: add code to handle termination of conditionals before end of loop
//       (e.g., a command string like "< 1 "N > '".


#define INFINITE        (-1)            ///< Infinite loop count

// TODO: add environment variable to limit loop depth?

///  @struct loop
///  @brief  Linked list structure for loops

struct loop
{
    struct loop *next;                  ///< Next item in list
    int count;                          ///< Iteration count
    uint start;                         ///< Starting position
};

static struct loop *loop_head;          ///< Head of loop list

// Local functions

static void endloop(struct cmd *cmd);


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

static void endloop(struct cmd *cmd)
{
    assert(cmd != NULL);

    uint depth = 1;                     // Nesting depth

    do
    {
        bool dryrun = f.e0.dryrun;

        f.e0.dryrun = true;
        (void)next_cmd(cmd);
        f.e0.dryrun = dryrun;

        if (cmd->c1 == '<')             // Start of a new loop?
        {
            ++depth;
        }
        else if (cmd->c1 == '>')        // End of a loop?
        {
            --depth;
        }
    } while (depth > 0);

    struct loop *loop = loop_head;    

    assert(loop != NULL);

    loop_head = loop->next;

    free_mem(&loop);
}


///
///  @brief    Execute F> command: flow to end of loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_gt(struct cmd *cmd)
{
    assert(cmd != NULL);

    endloop(cmd);                       // Flow to end of loop
}


///
///  @brief    Execute F< command: flow to start of loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F_lt(struct cmd *unused1)
{
    cmdbuf->pos = loop_head->start;    // Just restart the loop

    return;
}


///
///  @brief    Execute > command: end of loop.
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
        print_err(E_BNI);               // Close bracket not in iteration
    }

    if (loop->count == INFINITE || --loop->count > 0)
    {
        cmdbuf->pos = loop->start;     // Go back to start of loop
    }
    else
    {
        loop_head = loop->next;

        free_mem(&loop);
    }
}


///
///  @brief    Execute < command: start of loop.
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
        endloop(cmd);                   // End loop if count is <= 0
    }
    else
    {
        struct loop *loop = alloc_mem((uint)sizeof(*loop));

        loop->count = count;
        loop->start = cmdbuf->pos;
        loop->next  = loop_head;

        loop_head   = loop;
    }
}


///
///  @brief    Execute ; (semi-colon) command: exit loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_semi(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (loop_head == NULL)
    {
        print_err(E_SNI);               // Semi-colon not in loop
    }
    else if (!cmd->n_set)
    {
        print_err(E_NAS);               // No argument before semi-colon
    }

    if (cmd->colon_set)                 // n:; command
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

    endloop(cmd);
}


///
///  @brief    Initialize loop structures.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_loop(void)
{
    loop_head = NULL;

    if (atexit(reset_loop) != 0)
    {
        exit(EXIT_FAILURE);
    }
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
}
