///
///  @file    cmd_exec.c
///  @brief   Execute command string.
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
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"


///  @var    null_cmd
///
///  @brief  Initial command block values.

static const struct cmd null_cmd =
{
    .m_set  = false,
    .n_set  = false,
    .h      = false,
    .ctrl_y = false,
    .w      = false,
    .colon  = false,
    .dcolon = false,
    .atsign = false,
    .c1     = NUL,
    .c2     = NUL,
    .c3     = NUL,
    .m_arg  = 0,
    .n_arg  = 0,
    .delim  = ESC,
    .qname  = NUL,
    .qlocal = false,
    .expr   = { .buf = NULL, .len = 0 },
    .text1  = { .buf = NULL, .len = 0 },
    .text2  = { .buf = NULL, .len = 0 },
};


///
///  @brief    Execute command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_cmd(void)
{
    // Loop for all characters in command string.

    reset_scan();
    init_expr();                        // Reset expression stack
    struct cmd cmd = null_cmd;

    for (;;)
    {
        f.e0.exec = true;               // Executing a command

        if (!next_cmd(&cmd))
        {
            return;
        }

        f.e0.exec = false;              // Done executing command

        if (f.e0.ctrl_c)                // If CTRL/C typed, return to main loop
        {
            f.e0.ctrl_c = false;

            throw(E_XAB);               // Execution aborted
        }
    }
}



///
///  @brief    Get next command.
///
///  @returns  true if there is another command, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool next_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c;

    // If we've reached the end of a command string, then make sure that
    // all of the parentheses and braces were properly paired.

    if ((c = fetch_cbuf(CMD_START)) == EOF)
    {
        if (scan.nparens || scan.nbraces)
        {
            throw(E_MRP);           // Missing right parenthesis/brace
        }

        return false;
    }

    scan_cmd(cmd, c);

    return true;
}


///
///  @brief    We've scanned an illegal character, so return to main loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_bad(struct cmd *cmd)
{
    assert(cmd != NULL);

    throw(E_ILL, cmd->c1);              // Illegal character
}


///
///  @brief    Execute whitespace command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_space(struct cmd *unused1)
{

}


///
///  @brief    Execute ESCape command. Note that we're called here only for
///            escape characters between commands, or at the end of command
///            strings, not for escapes used to delimit commands (e.g., a
///            command such as "^Ahello, world!<ESC>").
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_escape(struct cmd *unused1)
{

}


///
///  @brief    Execute ctrl/^ (caret or uparrow) command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctl_up(struct cmd *cmd)
{
    assert(cmd != NULL);

    check_args(cmd);

    int c = fetch_cbuf(NOCMD_START);

    push_expr(c, EXPR_VALUE);
}


///
///  @brief    Execute ctrl/underscore command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctl_ubar(struct cmd *cmd)
{
    assert(cmd != NULL);

    push_expr(TYPE_OPER, cmd->c1);
}


///
///  @brief    Execute : or :: command modifiers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_colon(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (f.e2.colon && cmd->colon)       // Do we allow extra colons?
    {
        throw(E_MOD);                   // Invalid modifier
    }

    cmd->colon = true;

    int c = fetch_cbuf(NOCMD_START);

    if (c == ':')
    {
        cmd->dcolon = true;
    }
    else
    {
        unfetch_cbuf(c);
    }
}


///
///  @brief    Execute @ command modifier.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_atsign(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (f.e2.atsign && cmd->atsign)
    {
        throw(E_MOD);               // No more than one at sign
    }

    cmd->atsign = true;
}


///
///  @brief    Execute left parenthesis.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_lparen(struct cmd *cmd)
{
    assert(cmd != NULL);

    ++scan.nparens;

    push_expr(TYPE_GROUP, cmd->c1);
}


///
///  @brief    Execute right parenthesis.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_rparen(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (scan.nparens == 0)              // Can't have ) without (
    {
        throw(E_MLP);                   // Missing left parenthesis
    }
    else if (!check_expr())             // Is there an operand available?
    {
        throw(E_NAP);                   // No argument before )
    }
    else
    {
        --scan.nparens;
    }

    push_expr(TYPE_GROUP, cmd->c1);
}


///
///  @brief    Execute general operator. This is called for the following:
///
///            +  addition
///            -  subtraction
///            *  multiplication
///            /  division
///            #  logical OR
///            &  logical AND
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_oper(struct cmd *cmd)
{
    assert(cmd != NULL);

    check_args(cmd);

    push_expr(TYPE_OPER, cmd->c1);
}
