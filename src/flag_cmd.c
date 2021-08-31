///
///  @file    flag_cmd.c
///  @brief   Execute flag commands.
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "file.h"

#if     defined(DISPLAY_MODE)

#include "display.h"

#endif


// Local functions

static void check_n_flag(struct cmd *cmd, int_t *flag);

static void check_mn_flag(struct cmd *cmd, int_t *flag);


///
///  @brief    Check flag variable that takes one or two arguments.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void check_mn_flag(struct cmd *cmd, int_t *flag)
{
    assert(cmd != NULL);
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

static void check_n_flag(struct cmd *cmd, int_t *flag)
{
    assert(cmd != NULL);
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
    int_t n = f.ctrl_e ? -1 : 0;        // Reading flag returns 0 or -1

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

#if    defined(DISPLAY_MODE)

    union ed_flag saved = { .flag = f.ed.flag };

#endif

    check_mn_flag(cmd, &f.ed.flag);

#if    defined(DISPLAY_MODE)

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
    int_t n = f.ee;

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

    f.eh.flag &= 7;                     // Clear all but low 3 bits

    if (f.eh.verbose == 0)              // 0 is equivalent to 2 for EH
    {
        f.eh.verbose = 2;
    }
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

#if     defined(EU_COMMAND)

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
        n = (int)cmd->n_arg;            // Get whatever operand we can
    }

    n = teco_env(n, cmd->colon);        // Do the system-dependent part

    push_x((int_t)n, X_OPERAND);        // Now return the result

    cmd->colon = false;

    return true;
}


///
///  @brief    Scan flag commands that accept 1 numeric argument.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_flag1(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd->m_set);
    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    if (cmd->n_set)                     // n argument?
    {
        return false;                   // Yes, not an operand
    }

    switch (cmd->c1)
    {
        case CTRL_E:
            push_x(f.ctrl_e ? SUCCESS : FAILURE, X_OPERAND);

            return true;

        case CTRL_N:
        {
            reject_n(cmd->n_set);

            struct ifile *ifile = &ifiles[istream];

            push_x((int_t)feof(ifile->fp), X_OPERAND);

            return true;
        }

        case CTRL_X:
            push_x((int_t)f.ctrl_x, X_OPERAND);

            return true;

        case 'E':
        case 'e':
            switch (cmd->c2)
            {
                case 'E':
                case 'e':
                    push_x((int_t)f.ee, X_OPERAND);

                    return true;

                case 'O':
                case 'o':
                    push_x((int_t)f.eo, X_OPERAND);

                    return true;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    throw(E_ILL, cmd->c1);              // Should never get here!
}


///
///  @brief    Scan flag commands that accept 2 numeric arguments.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_flag2(struct cmd *cmd)
{
    assert(cmd != NULL);

    require_n(cmd->m_set, cmd->n_set);
    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    if (cmd->n_set)                     // n argument?
    {
        return false;                   // Yes, not an operand
    }

    assert(cmd->c1 == 'E' || cmd->c1 == 'e');

    switch (cmd->c2)
    {
        case '1':                       // E1
            push_x((int_t)f.e1.flag, X_OPERAND);

            return true;

        case '2':                       // E2
            push_x((int_t)f.e2.flag, X_OPERAND);

            return true;

        case '3':                       // E3
            push_x((int_t)f.e3.flag, X_OPERAND);

            return true;

        case '4':                       // E4
            push_x((int_t)f.e4.flag, X_OPERAND);

            return true;

        case 'D':                       // ED
        case 'd':
            push_x((int_t)f.ed.flag, X_OPERAND);

            return true;

        case 'H':                       // EH
        case 'h':
            push_x((int_t)f.eh.flag, X_OPERAND);

            return true;

        case 'S':                       // ES
        case 's':
            push_x((int_t)f.es, X_OPERAND);

            return true;

        case 'T':                       // ET
        case 't':
            push_x((int_t)f.et.flag, X_OPERAND);

            return true;

        case 'U':                       // EU
        case 'u':
            push_x((int_t)f.eu, X_OPERAND);

            return true;

        case 'V':                       // EV
        case 'v':
            push_x((int_t)f.ev, X_OPERAND);

            return true;

        default:
            break;
    }

    throw(E_ILL, cmd->c1);              // Should never get here!
}
