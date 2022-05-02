///
///  @file    flag_cmd.c
///  @brief   Execute flag commands.
///
///  @copyright 2019-2022 Franklin P. Johnston / Nowwith Treble Software
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
#include "display.h"
#include "editbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "file.h"


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
///  @brief    Execute CTRL/E command: read or set form feed flag.
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
///  @brief    Execute CTRL/X command: read or set search mode flag.
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
///  @brief    Execute E1 command: read or set extended features.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E1(struct cmd *cmd)
{
    union e1_flag e1 = { .flag = f.e1.flag };

    check_mn_flag(cmd, &e1.flag);

    // Only allow defined bits to be set or cleared

    f.e1.xoper   = e1.xoper;
    f.e1.text    = e1.text;
    f.e1.ctrl_a  = e1.ctrl_a;
    f.e1.equals  = e1.equals;
    f.e1.eimacro = e1.eimacro;
    f.e1.bang    = e1.bang;
    f.e1.prompt  = e1.prompt;
    f.e1.radix   = e1.radix;
    f.e1.dollar  = e1.dollar;
    f.e1.insert  = e1.insert;
    f.e1.percent = e1.percent;

#if     defined(DEBUG)

    f.e1.repeat  = e1.repeat;
    f.e1.newline = e1.newline;

#endif

}


///
///  @brief    Execute E2 command: read or set extended features.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E2(struct cmd *cmd)
{
    union e2_flag e2 = { .flag = f.e2.flag };

    check_mn_flag(cmd, &e2.flag);

    // Only allow defined bits to be set or cleared

    f.e2.zero   = e2.zero;
    f.e2.oper   = e2.oper;
    f.e2.atsign = e2.atsign;
    f.e2.colon  = e2.colon;
    f.e2.comma  = e2.comma;
    f.e2.m_arg  = e2.m_arg;
    f.e2.n_arg  = e2.n_arg;
    f.e2.loop   = e2.loop;;
    f.e2.quote  = e2.quote;
    f.e2.page   = e2.page;
    f.e2.args   = e2.args;
}


///
///  @brief    Execute E3 command: read or set I/O flags.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E3(struct cmd *cmd)
{
    union e3_flag e3 = { .flag = f.e3.flag };

    check_mn_flag(cmd, &e3.flag);

    // Only allow defined bits to be set or cleared

    f.e3.nopage  = e3.nopage;
    f.e3.smart   = e3.smart;
    f.e3.CR_in   = e3.CR_in;
    f.e3.CR_out  = e3.CR_out;
    f.e3.noin    = e3.noin;
    f.e3.noout   = e3.noout;
    f.e3.keepNUL = e3.keepNUL;
    f.e3.CR_type = e3.CR_type;
}


///
///  @brief    Execute E4 command: read or set display flags.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E4(struct cmd *cmd)
{
    union e4_flag e4 = { .flag = f.e4.flag };

    check_mn_flag(cmd, &e4.flag);

    int changes = (f.e4.flag ^ e4.flag);

    // Only allow defined bits to be set or cleared

    f.e4.invert = e4.invert;
    f.e4.fence  = e4.fence;
    f.e4.status = e4.status;

    if (changes)                        // Any changes?
    {
        reset_dpy((bool)true);          // Yes, reset display
    }
}


///
///  @brief    Execute ED command: read or set mode flags.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ED(struct cmd *cmd)
{
    union ed_flag ed = { .flag = f.ed.flag };

    check_mn_flag(cmd, &ed.flag);

    // Only allow defined bits to be set or cleared

    f.ed.caret    = ed.caret;
    f.ed.yank     = ed.yank;
    f.ed.keepdot  = ed.keepdot;
    f.ed.movedot  = ed.movedot;
    f.ed.nobuffer = ed.nobuffer;

    if (f.ed.escape ^ ed.escape)        // Do we need to update display?
    {
        set_escape((bool)(f.ed.escape = ed.escape));
    }
}


///
///  @brief    Execute EE command: read or set alternate delimiter.
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
///  @brief    Execute EH command: read or set help flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EH(struct cmd *cmd)
{
    assert(cmd != NULL);

    union eh_flag eh = { .flag = f.eh.flag };

    check_mn_flag(cmd, &eh.flag);

    // Only allow defined bits to be set or cleared

    f.eh.why   = eh.why;                // Print why we failed
    f.eh.what  = eh.what;               // Print command (what failed)

#if     defined(DEBUG)

    f.eh.where = eh.where;              // Print line no. (where error occurred)
    f.eh.who   = eh.who;                // Print function (who issued error)

#endif

}


///
///  @brief    Execute ES command: read or set search verification flag.
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
///  @brief    Execute ET command: read or set terminal mode flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ET(struct cmd *cmd)
{
    union et_flag et = { .flag = f.et.flag };

    check_mn_flag(cmd, &et.flag);

    bool eightbit = f.et.eightbit ^ et.eightbit;
    bool truncate = f.et.truncate ^ et.truncate;

    // Only allow defined bits to be set or cleared

    f.et.image    = et.image;
    f.et.rubout   = et.rubout;
    f.et.lower    = et.lower;
    f.et.noecho   = et.noecho;
    f.et.nowait   = et.nowait;
    f.et.abort    = et.abort;
    f.et.truncate = et.truncate;
    //f.et.scope  = et.scope;           // Scope bit is read-only
    f.et.eightbit = et.eightbit;
    f.et.accent   = et.accent;
    f.et.ctrl_c   = et.ctrl_c;

    if (!f.et.detach && et.detach)      // Does user want us to detach?
    {
        detach_term();                  // Detach from terminal

        f.et.detach = true;
    }

    if (eightbit)                       // Did 8-bit setting just change?
    {
        set_bits((bool)f.et.eightbit);
        init_keys();
    }

    if (truncate)                       // Did truncation bit just change?
    {
        f.e0.window = true;             // Window refresh needed
    }
}


///
///  @brief    Execute EU command: read or set upper/lower case flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EU(struct cmd *cmd)
{
    check_n_flag(cmd, &f.eu);

    if (f.eu < -1)
    {
        f.eu = -1;
    }
    else if (f.eu > 1)
    {
        f.eu = 1;
    }
}


///
///  @brief    Execute EV command: read or set edit verify flag.
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
///  @brief    Scan EJ command: read or set operating environment
///            information.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_EJ(struct cmd *cmd)
{
    assert(cmd != NULL);

    require_n(cmd->m_set, cmd->n_set);
    reject_dcolon(cmd->dcolon);
    reject_atsign(cmd->atsign);

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
            int_t eof = feof(ifile->fp) ? -1 : 0;

            push_x(eof, X_OPERAND);

            return true;
        }

        case CTRL_X:
            push_x((int_t)f.ctrl_x, X_OPERAND);

            return true;

        case 'E':
        case 'e':
            switch (cmd->c2)
            {
                case 'E':               // EE
                case 'e':
                    push_x((int_t)f.ee, X_OPERAND);

                    return true;

                case 'O':               // EO
                case 'o':
                    push_x((int_t)f.eo, X_OPERAND);

                    return true;

                case 'U':               // EU
                case 'u':
                    push_x((int_t)f.eu, X_OPERAND);

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

        case 'V':                       // EV
        case 'v':
            push_x((int_t)f.ev, X_OPERAND);

            return true;

        default:
            break;
    }

    throw(E_ILL, cmd->c1);              // Should never get here!
}
