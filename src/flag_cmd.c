///
///  @file    flag_cmd.c
///  @brief   Execute flag commands.
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
#include "exec.h"


// Local functions

static bool check_n_flag(int *flag);

static bool check_mn_flag(struct cmd *cmd, uint *flag);


///
///  @brief    Check flag variable that takes one or two arguments.
///
///  @returns  true if we have flag has a value, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////

static bool check_mn_flag(struct cmd *cmd, uint *flag)
{
    assert(cmd != NULL);
    assert(flag != NULL);

    if (!pop_expr(&cmd->n_arg))          // n argument?
    {
        push_expr((int)*flag, EXPR_VALUE);  // Assume we're an operand

        cmd_expr = false;

        return false;
    }

    // Here if there is a value preceding the flag, which
    // means that the flag is not part of an expression.

    if (!cmd->m_set)                    // m argument too?
    {
        *flag = (uint)cmd->n_arg;       // No, so just set flag
    }
    else                                // Both m and n were specified
    {
        if (cmd->m_arg != 0)
        {
            *flag &= ~(uint)cmd->m_arg; // Turn off m bits
        }
        if (cmd->n_arg != 0)
        {
            *flag |= (uint)cmd->n_arg;  // Turn on n bits
        }
    }

    cmd_expr = true;

    return true;
}


///
///  @brief    Check flag variable that takes one argument.
///
///  @returns  true if we have flag has a value, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////

static bool check_n_flag(int *flag)
{
    assert(flag != NULL);

    int n;

    if (!pop_expr(&n))
    {
        push_expr(*flag, EXPR_VALUE);

        cmd_expr = false;

        return false;
    }

    // Here if there is a value preceding the flag, which
    // means that the flag is not part of an expression.

    *flag = n;

    cmd_expr = true;

    return true;
}


///
///  @brief    Scan ^X (CTRL/X) command: read value of search mode flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_ctrl_x(struct cmd *unused1)
{
    (void)check_n_flag(&v.ctrl_x);
}


///
///  @brief    Scan ED command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_ED(struct cmd *cmd)
{
    assert(cmd != NULL);

    (void)check_mn_flag(cmd, &f.ed.flag);
}


///
///  @brief    Scan EH command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_EH(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (check_mn_flag(cmd, &f.eh.flag))
    {
        uint verbose = f.eh.verbose;
        uint command = f.eh.command;

        f.eh.flag = 0;

        f.eh.command = command;
        f.eh.verbose = verbose;
    }
}


///
///  @brief    Scan ES command: read search verification flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_ES(struct cmd *unused1)
{
    (void)check_n_flag(&f.es);
}


///
///  @brief    Scan ET command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_ET(struct cmd *cmd)
{
    assert(cmd != NULL);

    union et_flag old = { .flag = f.et.flag };

    if (check_mn_flag(cmd, &f.et.flag))
    {
        if (f.et.eightbit ^ old.eightbit)
        {
            // TODO: tell operating system if user changed bit
        }

        // The following are read-only bits and cannot be changed by the user.

        f.et.scope  = old.scope;
        f.et.rscope = old.rscope;
    }
}


///
///  @brief    Scan EU command: read upper/lower case flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_EU(struct cmd *unused1)
{
    (void)check_n_flag(&f.eu);
}


///
///  @brief    Scan EV command: read edit verify flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_EV(struct cmd *unused1)
{
    (void)check_n_flag(&f.ev);
}


///
///  @brief    Scan EZ command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_EZ(struct cmd *cmd)
{
    assert(cmd != NULL);

#if     0                               // TODO: use old or new EZ command?

    print_err(E_T10);                   // TECO-10 command not implemented

#else
    
    (void)check_mn_flag(cmd, &f.ez.flag);

#endif

}
