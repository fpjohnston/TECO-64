///
///  @file    cmd_scan.c
///  @brief   Parse and scan command arguments.
///
///  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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

#include "teco.h"
#include "ascii.h"
#include "cmdbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "estack.h"
#include "exec.h"


///
///  @brief    Scan invalid command.
///
///  @returns  true (not a full command).
///
////////////////////////////////////////////////////////////////////////////////

void exec_bad(struct cmd *unused)
{
    throw(E_ILL);                       // Invalid command
}


///
///  @brief    Scan at sign command.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_atsign(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->atsign && f.e2.atsign)
    {
        throw(E_ATS);                   // Too many at signs
    }

    cmd->atsign = true;

    return true;
}


///
///  @brief    Scan colon command.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_colon(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!cmd->colon)
    {
        if (peek_cbuf() == ':')        // Another colon?
        {
            next_cbuf();               // Yes, count it

            cmd->dcolon = true;        // Flag double colon
        }
        else
        {
            cmd->colon = true;          // Flag the first colon
        }
    }
    else if (f.e2.colon)
    {
        throw(E_COL);
    }
    else
    {
        cmd->dcolon = true;             // Flag double colon
        cmd->colon = false;             // Flag the first colon
    }

    return true;
}


///
///  @brief    Scan / command: division operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_div(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    // Check for double slash remainder operator.

    int c;

    if ((c = peek_cbuf()) == '/' && f.e1.xoper && check_parens())
    {
        next_cbuf();

        cmd->c2 = (char)c;

        store_oper(X_REM);
    }
    else
    {
        store_oper(X_DIV);
    }

    return true;
}


///
///  @brief    Scan F1, F2, F3, and F4 commands.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_F1(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_M_ONLY, NO_COLON, NO_DCOLON);

    scan_texts(cmd, 2, ESC);

    return false;
}


///
///  @brief    Dummy scan routine for ^, E, and F commands.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_false(struct cmd *unused)
{
    return false;
}


///
///  @brief    Scan FM command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_FM(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_M, NO_N, NO_DCOLON);

    scan_texts(cmd, 2, ESC);

    return false;
}


///
///  @brief    Execute whitespace characters.
///
///  @returns  true: command is a (quasi) operand or operator.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_white(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->c1 == LF && cmd_line != 0)
    {
        ++cmd_line;
    }

    return true;
}


///
///  @brief    Scan ~ command: exclusive OR operator.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_xor(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    if (!f.e0.skip && (!f.e1.xoper || !check_parens()))
    {
        throw(E_ILL, cmd->c1);          // Illegal command
    }

    store_oper(X_XOR);

    return true;
}
