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
#include "exec.h"


///  @var    null_cmd
///  @brief  Initial command block values.

static const struct cmd null_cmd =
{
    .m_set      = false,
    .n_set      = false,
    .h_set      = false,
    .w_set      = false,
    .colon_set  = false,
    .dcolon_set = false,
    .atsign_set = false,
    .c1         = NUL,
    .c2         = NUL,
    .c3         = NUL,
    .m_arg      = 0,
    .n_arg      = 0,
    .delim      = ESC,
    .qname      = NUL,
    .qlocal     = false,
    .expr       = { .buf = NULL, .len = 0 },
    .text1      = { .buf = NULL, .len = 0 },
    .text2      = { .buf = NULL, .len = 0 },
};


// Local functions

static void exec_dummy(struct cmd *unused1);


///
///  @brief    Execute command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_cmd(void)
{
    struct cmd cmd;

    // Loop for all characters in command string.

    for (;;)
    {
        f.e0.exec = true;
        exec_func *exec = next_cmd(&cmd);

        if (exec == NULL)
        {
            break;                      // Back to main loop if command done
        }
//        else if (cmd.c1 == '!' && cmd.text1.len != 0 && cmd.text1.buf[0] == ' ')
//        {
//            continue;
//        }

        if (f.e0.dryrun)
        {
            print_cmd(&cmd);            // Just print command
        }
        else
        {
            (*exec)(&cmd);              // Execute command
        }

        f.e0.exec = false;

        if (f.e0.ctrl_c)                // If CTRL/C typed, return to main loop
        {
            f.e0.ctrl_c = false;

            print_err(E_XAB);           // Execution aborted
        }
    }
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

    printc_err(E_ILL, cmd->c1);
}


///
///  @brief    Dummy function, used to ignore commands.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_dummy(struct cmd *unused1)
{

}


///
///  @brief    Execute ESCape command. Note that we're called here only for
///            escape characters between commands, or at the end of command
///            strings, not for escapes used to delimit commands (such as is
///            the case for a command such as "^Ahello, world!<ESC>").
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_escape(struct cmd *unused1)
{
    int n;

    (void)pop_expr(&n);
    (void)pop_expr(&n);
}


///
///  @brief    Execute : or @: command modifiers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_mod(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->c1 == '@')
    {
        if (f.e1.strict && cmd->atsign_set)
        {
            print_err(E_MOD);           // Two @'s are not allowed
        }

        cmd->atsign_set = true;
    }
    else if (cmd->c1 == ':')
    {
        if (f.e1.strict && cmd->dcolon_set)
        {
            print_err(E_MOD);           // More than two :'s are not allowed
        }

        if (cmd->colon_set)
        {
            cmd->colon_set = false;
            cmd->dcolon_set = true;
        }
        else if (!cmd->dcolon_set)
        {
            cmd->colon_set = true;
        }
    }
}


///
///  @brief    Execute operator in expression. This may be any of the binary
///            operators (+, -, *, /, &, #), the 1's complement operator (^_),
///            or a left or right parenthesis.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_operator(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c;
    int type = cmd->c1;

    switch (type)
    {
        case '(':
            ++scan.nparens;
            break;

        case ')':
            if (scan.nparens == 0)      // Can't have ) without (
            {
                print_err(E_MLP);       // Missing left parenthesis
            }
            else if (!pop_expr(NULL))   // Is there an operand available?
            {
                print_err(E_NAP);       // No argument before )
            }
            else
            {
                --scan.nparens;
            }
            break;

        case '{':
            ++scan.nbraces;
            break;

        case '}':
            if (scan.nbraces == 0)      // Can't have ) without (
            {
                print_err(E_MLP);       // Missing left brace
            }
            else if (!pop_expr(NULL))   // Is there an operand available?
            {
                print_err(E_NAP);       // No argument before )
            }
            else
            {
                --scan.nbraces;
            }
            break;

        case '/':
            if (f.e3.brace && scan.nbraces)
            {
                if ((c = fetch_buf(NOCMD_START)) == '/')
                {
                    type = EXPR_REM;
                }
                else
                {
                    unfetch_buf(c);
                }
            }
            break;

        case '<':
            if ((c = fetch_buf(NOCMD_START)) == '=')
            {
                type = EXPR_LE;
            }
            else if (c == '>')
            {
                type = EXPR_NE;
            }
            else if (c == '<')
            {
                type = EXPR_LEFT;
            }
            else
            {
                unfetch_buf(c);
            }
            break;

        case '>':
            if ((c = fetch_buf(NOCMD_START)) == '=')
            {
                type = EXPR_GE;
            }
            else if (c == '>')
            {
                type = EXPR_RIGHT;
            }
            else
            {
                unfetch_buf(c);
            }
            break;

        case '=':
            if (fetch_buf(NOCMD_START) != '=')
            {
                print_err(E_ARG);
            }
            break;

        case '+':
        case '-':
        case '*':
        case '&':
        case '#':
        case '~':
        case '!':
            break;

        default:
            print_err(E_ARG);
    }

    if (strchr("(){}", type) != NULL)
    {
        push_expr(TYPE_GROUP, type);
    }
    else
    {
        push_expr(TYPE_OPER, type);
    }
}


///
///  @brief    Get next command.
///
///  @returns  Command block with options, or NULL if at end of command string.
///
////////////////////////////////////////////////////////////////////////////////

exec_func *next_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);

    *cmd = null_cmd;

    // Pop m and n arguments (if any) from the expression stack

    if (pop_expr(&cmd->n_arg))
    {
        cmd->n_set = true;

        if (pop_expr(&cmd->m_arg))
        {
            cmd->m_set = true;
        }
    }

    reset_scan();
    init_expr();                        // Reset expression stack

    // Keep reading characters until we have a full command.

    bool start = CMD_START;
    exec_func *exec = NULL;

    cmd->expr.buf = next_buf();

    for (;;)
    {
        int c;

        if (!scan.mod)
        {
            cmd->expr.len = (uint)(next_buf() - cmd->expr.buf);
        }

        if ((c = fetch_buf(start)) == EOF)
        {
            break;
        }
        else if (c < 0 || c >= (int)cmd_count)
        {
            printc_err(E_ILL, c);       // Illegal character
        }

        cmd->c1 = (char)c;

        if ((exec = scan_cmd(cmd)) != NULL)
        {
            break;
        }

//        cmd->m_set = cmd->n_set = false;
//        start = NOCMD_START;
    }

    // If we have a tag and the accompanying text starts with the character
    // defined in E4, then we skip it. This allows us to differentiate between
    // tags (which may the target of O commands) and comments (which aren't).
    // So for example, E4 could be set to a SPACE character (ASCII 32), which
    // would mean that a comment could take the form ! comment !, and a tag
    // could take the form !tag!. Note that the E4 character is only checked
    // for the first character after the first !, and is not required to
    // precede the second !.

    if (cmd->c1 == '!' && (cmd->text1.len == 0 ||
                          (cmd->text1.len != 0 && cmd->text1.buf[0] == f.e5)))
    {
        return exec_dummy;              // Just ignore tag
    }

    // Pop m and n arguments (if any) from the expression stack

    if (pop_expr(&cmd->n_arg))
    {
        cmd->n_set = true;

        if (pop_expr(&cmd->m_arg))
        {
            cmd->m_set = true;
        }
    }

    // If the only thing on the expression stack is a minus sign,
    // then say we have an n argument equal to -1. Otherwise check
    // for m and n arguments.

    if (estack.level == 1 && estack.obj[0].type == EXPR_MINUS)
    {
        estack.level = 0;

        cmd->n_set = true;
        cmd->n_arg = -1;
    }

#if     0               // TODO: can we keep this?
    else if (f.e1.strict)
    {
        if (estack.level != 0)
        {
            printf("expression stack warning\r\n"); // TODO: TBD
        }

        if (cmd->n_set && !scan.n_opt)
        {
            print_err(E_UNA);           // Unused n argument
        }
        else if (cmd->m_set && !scan.m_opt)
        {
            print_err(E_UMA);           // Unused m argument
        }
    }
#endif

    return exec;
}
