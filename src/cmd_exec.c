///
///  @file    cmd_exec.c
///  @brief   Execute command string.
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
#include <ctype.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


uint line;                              ///< Current line number

uint nescapes = 0;               ///< No. of consecutive escapes seen

struct tstring expr;                    ///< Current expression

char *last_chr;                         ///< Last character in expression

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


///
///  @brief    Execute command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_cmd(void)
{
    struct cmd cmd;

    nescapes = 0;

    expr.buf = NULL;
    expr.len = 0;

    line = 1;

    // Loop for all characters in command string.

    for (;;)
    {
        if (expr.buf == NULL)
        {
            expr.buf = next_buf();
        }

        f.e0.exec = true;
        exec_func *exec = next_cmd(&cmd);
        f.e0.exec = false;

        if (exec == NULL)
        {
            break;                      // Back to main loop if command done
        }
        else if (cmd.c1 == LF)
        {
            ++line;
        }
        else if (cmd.c1 == '!' && cmd.text1.len != 0 && cmd.text1.buf[0] == ' ')
        {
            continue;
        }

        // If the only thing on the expression stack is a minus sign,
        // then say we have an n argument equal to -1. Otherwise check
        // for m and n arguments.

        if (cmd.expr.len == 1 && estack.obj[0].type == EXPR_MINUS)
        {
            estack.level = 0;

            cmd.n_set = true;
            cmd.n_arg = -1;
        }
        else if (pop_expr(&cmd.n_arg))
        {
            cmd.n_set = true;

            if (pop_expr(&cmd.m_arg))
            {
                cmd.m_set = true;
            }
        }

        f.e0.exec = true;
        (*exec)(&cmd);                  // Execute command
        f.e0.exec = false;

        expr.buf = NULL;
        expr.len = 0;

        if (f.e0.ctrl_c)                // If CTRL/C typed, return to main loop
        {
            f.e0.ctrl_c = false;

            print_err(E_XAB);           // Execution aborted
        }
    }
}


///
///  @brief    Execute ESCape command. Note that we're called here only for
///            escape characters between commands, or at the end of command
///            strings, not for escapes used to delimit commands (such as is
///            the case for a command such as ^Ahello, world!<ESC>).
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
///  @brief    Get next command. This 
///
///  @returns  Command block with options, or NULL if at end of command string.
///
////////////////////////////////////////////////////////////////////////////////

exec_func *next_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);

    *cmd = null_cmd;
    reset_scan();
    init_expr();                        // Reset expression stack
    last_chr = next_buf();

    // Keep reading characters until we have a full command.

    bool start = CMD_START;
    exec_func *exec;
    int c;

    for (;;)
    {
        if ((c = fetch_buf(start)) == EOF)
        {
            return NULL;                // NULL means command is done
        }
        else if (c < 0 || c >= (int)cmd_count)
        {
            printc_err(E_ILL, c);       // Illegal character
        }

        cmd->c1 = (char)c;

        if ((exec = scan_pass1(cmd)) != NULL)
        {
            return exec;
        }

        start = NOCMD_START;
    }
}
