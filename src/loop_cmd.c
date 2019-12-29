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
#include <unistd.h>                     // TODO: temporary

#include "teco.h"
#include "errors.h"
#include "exec.h"

#define INFINITE        (-1)            ///< Infinite loop count

bool search_success = true;             ///< true if last search succeeded

// TODO: add loop stack

struct loop
{
    int count;                          ///< Iteration count
    uint start;                         ///< Starting position
};

struct loop loop;

bool loop_active = false;

// Local functions

static void endloop(struct cmd *cmd);


///
///  @brief    Flow to end of loop
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void endloop(struct cmd *cmd)
{
    assert(cmd != NULL);

    uint depth = 0;                     // Nesting depth

    do
    {
        *cmd = null_cmd;

        (void)next_cmd(cmd);

        if (cmd->c1 == '<')             // Start of a new loop?
        {
            ++depth;
        }
        else if (cmd->c1 == '>')        // End of a loop?
        {
            --depth;
        }
    } while (depth > 0);

    loop_active = false;
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
    (void)sleep(1);                     // TODO: temporary

    cmd_buf->get = loop.start;          // Just restart the loop

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

    if (!loop_active)
    {
        print_err(E_BNI);               // Close bracket not in iteration
    }

    printf("loop count = %d\r\n", loop.count); // TODO: temporary

    if (loop.count == INFINITE || --loop.count > 0)
    {
        (void)sleep(1);                 // TODO: temporary

        cmd_buf->get = loop.start;      // Go back to start of loop
    }
    else
    {
        loop_active = false;
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

    if (!cmd->n_set)
    {
        loop.count = INFINITE;          // Special value for infinite loop
    }
    else if ((loop.count = cmd->n_arg) <= 0)
    {
        endloop(cmd);                   // End loop if count is <= 0
    }
    else
    {
        loop.start = cmd_buf->get;

        loop_active = true;
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

    if (!loop_active)
    {
        print_err(E_SNI);               // Semi-colon not in loop
    }

    if (cmd->n_set)
    {
        if (cmd->colon_set)             // n:; command
        {
            if (cmd->n_arg >= 0)        // End loop if n < 0
            {
                return;
            }
        }
        else                            // n; command
        {
            if (cmd->n_arg < 0)         // End loop if n >= 0
            {
                return;
            }
        }
    }
    else if (cmd->colon_set)            // :; command
    {
        if (!search_success)            // End loop if last search succeeded
        {
            return;
        }
    }
    else                                // ; command
    {
        if (search_success)             // End loop if last search failed
        {
            return;
        }
    }

    endloop(cmd);
}
