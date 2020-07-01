///
///  @file    flag_cmd.c
///  @brief   Execute flag commands.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
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
    assert(cmd != NULL);
    assert(flag != NULL);

    if (!cmd->n_set)                    // n argument?
    {
        push_expr(*flag, EXPR_VALUE);  // Assume we're an operand

        return false;
    }

    // Here if there is a value preceding the flag, which
    // means that the flag is not part of an expression.

    if (!f.e0.dryrun)
    {
        if (!cmd->m_set)                // m argument too?
        {
            *flag = cmd->n_arg;         // No, so just set flag
        }
        else                            // Both m and n were specified
        {
            if (cmd->m_arg != 0)        // Turn off m bits
            {
                *flag &= (int)~(uint)cmd->m_arg;
            }
            if (cmd->n_arg != 0)        // Turn on n bits
            {
                *flag |= cmd->n_arg;
            }
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
    assert(cmd != NULL);
    assert(flag != NULL);

    if (!cmd->n_set)
    {
        push_expr(*flag, EXPR_VALUE);

        return false;
    }

    // Here if there is a value preceding the flag, which
    // means that the flag is not part of an expression.

    if (!f.e0.dryrun)
    {
        *flag = cmd->n_arg;
    }

    return true;
}


///
///  @brief    Scan ^E (CTRL/E) command: read form feed flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_e(struct cmd *cmd)
{
    assert(cmd != NULL);

    int n = f.ctrl_e ? -1 : 0;          // Reading flag returns 0 or -1

    if (check_n_flag(cmd, &n))
    {
        f.ctrl_e = n ? true : false;
    }
}


///
///  @brief    Scan ^F (CTRL/F) command: return value of console switch
///            register, or terminal number of specified job.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_f(struct cmd *cmd)
{
    assert(cmd != NULL);

    int n = 0;                          // Default value for CTRL/F

    (void)check_n_flag(cmd, &n);
}


///
///  @brief    Scan ^N (CTRL/N) command: read end of file flag for current
///            input stream.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_n(struct cmd *cmd)
{
    assert(cmd != NULL);

    struct ifile *ifile = &ifiles[istream];

    push_expr(ifile->eof, EXPR_VALUE);
}


///
///  @brief    Scan ^X (CTRL/X) command: read value of search mode flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_x(struct cmd *cmd)
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
    assert(cmd != NULL);

    (void)check_mn_flag(cmd, &f.e1.flag);
}


///
///  @brief    Scan E2 command: set extended features.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E2(struct cmd *cmd)
{
    assert(cmd != NULL);

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
    assert(cmd != NULL);

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
    assert(cmd != NULL);

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
    assert(cmd != NULL);

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
    assert(cmd != NULL);

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
///  @brief    Scan EJ command; get operating environment characteristics:
///
///            -1EJ - This has the form (m << 8) + n, where m is a code for the
///                   computer hardware in use, and n is a code for the operating
///                   system, as follows:
///
///                    m   n    -1EJ  Hardware  Operating system
///                   --- ---   ----  --------  ----------------
///                     0   0      0   PDP-11   RSX-11D
///                     0   1      1   PDP-11   RSX-11M
///                     0   2      2   PDP-11   RSX-11S
///                     0   3      3   PDP-11   IAS
///                     0   4      4   PDP-11   RSTS/E
///                     0   5      5   PDP-11   VAX/VMS (compatibility mode)
///                     0   6      6   PDP-11   RSM-11M+
///                     0   7      7   PDP-11   RT-11
///                     1   0    256   PDP-8    OS/8
///                     2   0    512   DEC-10   TOPS-10
///                     3   0    768   DEC-20   TOPS-20
///                     4   0   1024   VAX-11   VAX/VMS (native mode)
///                     4   1   1025   VAX-11   Ultrix
///                   100   0  25600   Sun      SunOS
///                   101   0  25856   x86      MS-DOS
///                   101   1  25857   x86      OS/2
///                   101   2  25858   x86      Linux
///
///             0EJ - Process number, 0 if single-process system.
///              EJ - Same as 0EJ.
///             1EJ - Terminal keyboard number, 0 if single-user system.
///             2EJ - User identification number.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EJ(struct cmd *cmd)
{
    assert(cmd != NULL);

    int n = 0;                          // 0EJ is default command

    if (!f.e0.dryrun)
    {
        if (cmd->n_set)
        {
            n = cmd->n_arg;             // Get whatever operand we can
        }

        n = teco_env(n, cmd->colon_set); // Do the system-dependent part
    }

    push_expr(n, EXPR_VALUE);           // Now return the result
}


///
///  @brief    Scan EO command: read or set TECO version number.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EO(struct cmd *cmd)
{
    assert(cmd != NULL);

    int n = teco_version;

    if (check_n_flag(cmd, &n))
    {
        throw(E_NYI);                   // Not yet implemented
    }
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
    assert(cmd != NULL);

    union et_flag saved = { .flag = f.et.flag };

    if (check_mn_flag(cmd, &f.et.flag))
    {
        if (f.et.eightbit ^ saved.eightbit)
        {
            // TODO: tell operating system if user changed bit
        }

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
