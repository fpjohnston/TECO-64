///
///  @file    flag_cmd.c
///  @brief   Execute flag commands.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "file.h"


// Local functions

static bool check_n_flag(struct cmd *cmd, int *flag);

static bool check_mn_flag(struct cmd *cmd, int *flag);


///
///  @brief    Check flag variable that takes one or two arguments.
///
///  @returns  true if flag has a value, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////

static bool check_mn_flag(struct cmd *cmd, int *flag)
{
    assert(cmd != NULL);                // Error if no command block
    assert(flag != NULL);               // Error if no flag bits

    if (!cmd->n_set)                    // n argument?
    {
        push_expr(*flag, EXPR_VALUE);   // Assume we're an operand

        return false;
    }

    if (!cmd->m_set)                    // m argument too?
    {
        *flag = cmd->n_arg;             // No, so just set flag
    }
    else                                // Both m and n were specified
    {
        if (cmd->m_arg != 0)            // Turn off m bits
        {
            *flag &= (int)~(uint)cmd->m_arg;
        }
        if (cmd->n_arg != 0)            // Turn on n bits
        {
            *flag |= cmd->n_arg;
        }
    }

    return true;
}


///
///  @brief    Check flag variable that takes one argument.
///
///  @returns  true if flag has a value, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////

static bool check_n_flag(struct cmd *cmd, int *flag)
{
    assert(cmd != NULL);                // Error if no command block
    assert(flag != NULL);               // Error if no flag bits

    if (!cmd->n_set)
    {
        push_expr(*flag, EXPR_VALUE);

        return false;
    }

    // Here if there is a value preceding the flag, which
    // means that the flag is not part of an expression.

    *flag = cmd->n_arg;

    return true;
}


///
///  @brief    Scan ^E (CTRL/E) command: read or set form feed flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_E(struct cmd *cmd)
{
    int n = f.ctrl_e ? -1 : 0;          // Reading flag returns 0 or -1

    if (check_n_flag(cmd, &n))
    {
        f.ctrl_e = n ? true : false;
    }
}


///
///  @brief    Scan ^N (CTRL/N) command: read end of file flag for current
///            input stream.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_N(struct cmd *unused1)
{
    struct ifile *ifile = &ifiles[istream];

    push_expr(feof(ifile->fp), EXPR_VALUE);
}


///
///  @brief    Scan ^X (CTRL/X) command: read value of search mode flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_X(struct cmd *cmd)
{
    if (check_n_flag(cmd, &f.ctrl_x))
    {
        // Make sure that the flag only has the values -1, 0, or 1.

        if (f.ctrl_x < 0)
        {
            f.ctrl_x = -1;
        }
        else if (f.ctrl_x > 0)
        {
            f.ctrl_x = 1;
        }
    }
}


///
///  @brief    Scan E1 command: set extended features.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E1(struct cmd *cmd)
{
    union e1_flag saved = { .flag = f.e1.flag };

    (void)check_mn_flag(cmd, &f.e1.flag);

    if (f.e0.init)                      // Initializing?
    {
        return;                         // Yes, don't worry about E1 changes
    }

    if (f.e1.new_ei ^ saved.new_ei)     // Did EI bit change?
    {
        switch (check_EI())
        {
            case 1:                     // Processing new-style EI
                if (f.e1.new_ei)        // Did we just set flag?
                {
                    return;             // Yes, that's okay
                }

                break;                  // Not okay

            case -1:                    // Processing old-style EI
                if (!f.e1.new_ei)       // Did we just clear flag?
                {
                    return;             // Yes, that's okay
                }

                break;                  // Not okay

            default:                    // No change
                return;
        }

        throw(E_IE1);                   // Illegal E1 command
    }
}


///
///  @brief    Scan E2 command: set extended features.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E2(struct cmd *cmd)
{
    (void)check_mn_flag(cmd, &f.e2.flag);
}


///
///  @brief    Scan E3 command: set I/O flags.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E3(struct cmd *cmd)
{
    (void)check_mn_flag(cmd, &f.e3.flag);
}


///
///  @brief    Scan E4 command: set window flags.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E4(struct cmd *cmd)
{
    (void)check_mn_flag(cmd, &f.e4.flag);
}


///
///  @brief    Scan ED command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ED(struct cmd *cmd)
{
    (void)check_mn_flag(cmd, &f.ed.flag);
}


///
///  @brief    Scan EE command: read alternate delimiter.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EE(struct cmd *cmd)
{
    int n = f.ee;

    if (check_n_flag(cmd, &n))
    {
        if (!isascii(n))                // Must be an ASCII character
        {
            throw(E_CHR);               // Invalid character for command
        }

        f.ee = n;
    }
}


///
///  @brief    Scan EH command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EH(struct cmd *cmd)
{
    if (check_mn_flag(cmd, &f.eh.flag))
    {
        union eh_flag eh;

        eh.flag = 0;

        eh.verbose = f.eh.verbose;
        eh.command = f.eh.command;

        f.eh.flag = 0;

        f.eh.command = eh.command;
        f.eh.verbose = eh.verbose;
    }
}


///
///  @brief    Scan EJ command; get operating environment information.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EJ(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->m_set)                     // Did user specify m,nEJ?
    {
        throw(E_NYI);                   // Not yet implemented
    }

    int n = 0;                          // 0EJ is default command

    if (cmd->n_set)
    {
        n = cmd->n_arg;                 // Get whatever operand we can
    }

    n = teco_env(n, cmd->colon);        // Do the system-dependent part

    push_expr(n, EXPR_VALUE);           // Now return the result
}


///
///  @brief    Scan ES command: read search verification flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ES(struct cmd *cmd)
{
    (void)check_n_flag(cmd, &f.es);
}


///
///  @brief    Scan ET command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ET(struct cmd *cmd)
{
    union et_flag saved = { .flag = f.et.flag };

    if (check_mn_flag(cmd, &f.et.flag))
    {

#if     defined(__vms)

        if (f.et.eightbit ^ saved.eightbit)
        {
            // TODO: tell operating system if user changed bit
        }

#endif

        // The following are read-only bits and cannot be changed by the user.

        f.et.scope  = saved.scope;
        f.et.rscope = saved.rscope;
    }
}


///
///  @brief    Scan EU command: read upper/lower case flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EU(struct cmd *cmd)
{
    (void)check_n_flag(cmd, &f.eu);
}


///
///  @brief    Scan EV command: read edit verify flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EV(struct cmd *cmd)
{
    (void)check_n_flag(cmd, &f.ev);
}
