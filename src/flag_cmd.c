///
///  @file    flag_cmd.c
///  @brief   Execute flag commands.
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "file.h"

#if     defined(TECO_DISPLAY)

#include "display.h"

#endif


// Local functions

static void check_n_flag(struct cmd *cmd, int *flag);

static void check_mn_flag(struct cmd *cmd, int *flag);


///
///  @brief    Check flag variable that takes one or two arguments.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void check_mn_flag(struct cmd *cmd, int *flag)
{
    assert(cmd != NULL);                // Error if no command block
    assert(flag != NULL);               // Error if no flag bits

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
}


///
///  @brief    Check flag variable that takes one argument.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void check_n_flag(struct cmd *cmd, int *flag)
{
    assert(cmd != NULL);                // Error if no command block
    assert(flag != NULL);               // Error if no flag bits

    // Here if there is a value preceding the flag, which
    // means that the flag is not part of an expression.

    *flag = cmd->n_arg;
}


///
///  @brief    Execute "^E" (CTRL/E) command: read or set form feed flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_E(struct cmd *cmd)
{
    int n = f.ctrl_e ? -1 : 0;          // Reading flag returns 0 or -1

    check_n_flag(cmd, &n);

    f.ctrl_e = n ? true : false;
}


///
///  @brief    Execute "^X" (CTRL/X) command: read or set search mode flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_X(struct cmd *cmd)
{
    check_n_flag(cmd, &f.ctrl_x);

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


///
///  @brief    Execute "E1" command: read or set extended features.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E1(struct cmd *cmd)
{
    union e1_flag saved = { .flag = f.e1.flag };

    check_mn_flag(cmd, &f.e1.flag);

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

        throw(E_IE1);                   // Invalid E1 command
    }
}


///
///  @brief    Execute "E2" command: read or set extended features.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E2(struct cmd *cmd)
{
    check_mn_flag(cmd, &f.e2.flag);
}


///
///  @brief    Execute "E3" command: read or set I/O flags.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E3(struct cmd *cmd)
{
    check_mn_flag(cmd, &f.e3.flag);
}


///
///  @brief    Execute "E4" command: read or set display flags.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E4(struct cmd *cmd)
{
    check_mn_flag(cmd, &f.e4.flag);
}


///
///  @brief    Execute "ED" command: read or set mode flags.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ED(struct cmd *cmd)
{

#if    defined(TECO_DISPLAY)

    union ed_flag saved = { .flag = f.ed.flag };

#endif

    check_mn_flag(cmd, &f.ed.flag);

#if    defined(TECO_DISPLAY)

    check_escape((uint)saved.escape);

#else

    f.ed.escape = false;

#endif
}


///
///  @brief    Execute "EE" command: read or set alternate delimiter.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EE(struct cmd *cmd)
{
    int n = f.ee;

    check_n_flag(cmd, &n);

    if (!isascii(n))                    // Must be an ASCII character
    {
        throw(E_CHR);                   // Invalid character for command
    }

    f.ee = n;
}


///
//  @brief    Execute "EH" command: read or set help flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EH(struct cmd *cmd)
{
    assert(cmd != NULL);

    check_mn_flag(cmd, &f.eh.flag);

    union eh_flag eh;

    eh.flag = 0;

    eh.verbose = f.eh.verbose;
    eh.command = f.eh.command;

    f.eh.flag = 0;

    f.eh.command = eh.command;
    f.eh.verbose = eh.verbose;
}


///
///  @brief    Execute "ES" command: read or set search verification flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ES(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->m_set)
    {
        cmd->n_arg &= 0xFF;
        cmd->n_arg += (cmd->m_arg + 1) * 256;
    }

    check_n_flag(cmd, &f.es);
}


///
///  @brief    Execute "ET" command: read or set terminal mode flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ET(struct cmd *cmd)
{
    union et_flag saved = { .flag = f.et.flag };

    check_mn_flag(cmd, &f.et.flag);

#if     defined(__vms)

    if (f.et.eightbit ^ saved.eightbit)
    {
        // TODO: tell operating system if user changed bit
    }

#endif

    // The following are read-only bits and cannot be changed by the user.

    f.et.scope   = saved.scope;
    f.et.refresh = saved.refresh;
}


///
///  @brief    Execute "EU" command: read or set upper/lower case flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(CONFIG_EU)

void exec_EU(struct cmd *cmd)
{
    check_n_flag(cmd, &f.eu);
}

#else

void exec_EU(struct cmd *unused)
{
    throw(E_CFG);                       // Command not configured
}

#endif


///
///  @brief    Execute "EV" command: read or set edit verify flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EV(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->m_set)
    {
        cmd->n_arg &= 0xFF;
        cmd->n_arg += (cmd->m_arg + 1) * 256;
    }

    check_n_flag(cmd, &f.ev);
}


///
///  @brief    Scan "EJ" command: read or set operating environment
///            information.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_EJ(struct cmd *cmd)
{
    assert(cmd != NULL);

    int n = 0;                          // 0EJ is default command

    if (cmd->n_set)
    {
        n = cmd->n_arg;                 // Get whatever operand we can
    }

    n = teco_env(n, cmd->colon);        // Do the system-dependent part

    push_x(n, X_OPERAND);               // Now return the result

    cmd->colon = false;

    return true;
}
