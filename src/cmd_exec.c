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


static uint nescapes = 0;

struct tstring expr;

char *last_chr;

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

static bool exec_escape(void);


///
///  @brief    Execute command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_cmd(void)
{
    struct cmd cmd = null_cmd;

    nescapes = 0;

    expr.buf = NULL;
    expr.len = 0;

    // Loop for all characters in command string.

    for (;;)
    {
        if (expr.buf == NULL)
        {
            expr.buf = next_buf();
        }

        if (expr.buf == NULL)           // If end of command string,
        {
            break;                      //  then back to main loop
        }

        f.ei.exec = true;

        exec_func *exec = next_cmd(&cmd);

        f.ei.exec = false;

        // Assume any tags that start with a space are really comments,
        // and skip them.

        if (cmd.c1 == '!' && cmd.text1.len != 0 && cmd.text1.buf[0] == ' ')
        {
            cmd = null_cmd;

            continue;
        }

        if (cmd.c1 != ESC)
        {
            nescapes = 0;
        }
        else if (exec_escape())
        {
            return;
        }

        if (exec != NULL)
        {
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

            f.ei.exec = true;

            (*exec)(&cmd);               // Now finally execute command

            f.ei.exec = false;

            expr.buf = NULL;
            expr.len = 0;

            cmd = null_cmd;
        }

        if (f.ei.ctrl_c)                // If CTRL/C typed, return to main loop
        {
            f.ei.ctrl_c = false;

            print_err(E_XAB);           // Execution aborted
        }
    }
}


///
///  @brief    Process ESCape which terminates a command string.
///
///  @returns  true if double escape seen, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool exec_escape(void)
{
    //  If we're in strict mode, and a command ended while we were in
    //  the middle of a conditional statement, then issue an error.

    bool cmd_done = ++nescapes >= 2;

    if (cmd_done)
    {
        nescapes = 0;

        if (f.ei.strict && test_if())
        {
            print_err(E_UTQ);           // Unterminated quote command
        }
    }

    int n;

    (void)pop_expr(&n);
    (void)pop_expr(&n);

    return cmd_done;
}


///
///  @brief    Get next command.
///
///  @returns  Command block with options.
///
////////////////////////////////////////////////////////////////////////////////

exec_func *next_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan.q_register = false;

    last_chr = next_buf();

    cmd->c1 = (char)fetch_buf();        // Get next command string character

    if (cmd->c1 < 0 || cmd->c1 >= (int)cmd_count)
    {
        printc_err(E_ILL, cmd->c1);     // Illegal character
    }

    return scan_pass1(cmd);
}
