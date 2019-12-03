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


enum scan_state scan_state;             ///< Current expression scanning state

// Local functions

static void exec_expr(struct cmd *cmd);

static void finish_cmd(struct cmd *cmd, const struct cmd_table *table);


///
///  @var    cmd_table
///
///  @brief  Dispatch table, defining functions to scan execute input
///          characters, as well as the options for each command.
///
///          Note that to avoid duplication, lower-case letters can be omitted,
///          because we translate characters to upper-case before indexing into
///          this table. Also, we handle E and F commands specially, as they
///          involve a 2nd character.
///

struct cmd_table cmd_table[] =
{
    [NUL]         = { NULL,           NULL,             ""             },
    [CTRL_A]      = { NULL,           exec_ctrl_a,      "@ 1"          },
    [CTRL_B]      = { scan_ctrl_b,    NULL,             ""             },
    [CTRL_C]      = { NULL,           exec_ctrl_c,      ""             },
    [CTRL_D]      = { NULL,           exec_ctrl_d,      ""             },
    [CTRL_E]      = { scan_ctrl_e,    exec_ctrl_e,      ""             },
    [CTRL_F]      = { scan_ctrl_f,    NULL,             "n"            },
    [CTRL_G]      = { scan_bad,       NULL,             ""             },
    [CTRL_H]      = { scan_ctrl_h,    NULL,             ""             },
    [CTRL_I]      = { NULL,           exec_ctrl_i,      "1"            },
    [LF]          = { NULL,           NULL,             ""             },
    [VT]          = { scan_bad,       NULL,             ""             },
    [FF]          = { NULL,           NULL,             ""             },
    [CR]          = { NULL,           NULL,             ""             },
    [CTRL_N]      = { scan_ctrl_n,    NULL,             ""             },
    [CTRL_O]      = { NULL,           exec_ctrl_o,      ""             },
    [CTRL_P]      = { scan_bad,       NULL,             ""             },
    [CTRL_Q]      = { NULL,           exec_ctrl_q,      ""             },
    [CTRL_R]      = { scan_ctrl_r,    exec_ctrl_r,      "n"            },
    [CTRL_S]      = { scan_ctrl_s,    NULL,             ""             },
    [CTRL_T]      = { scan_ctrl_t,    exec_ctrl_t,      "n :"          },
    [CTRL_U]      = { NULL,           exec_ctrl_u,      "n : @ q 1"    },
    [CTRL_V]      = { NULL,           exec_ctrl_v,      ""             },
    [CTRL_W]      = { NULL,           exec_ctrl_w,      ""             },
    [CTRL_X]      = { scan_ctrl_x,    exec_ctrl_x,      "n"            },
    [CTRL_Y]      = { scan_ctrl_y,    NULL,             ""             },
    [CTRL_Z]      = { scan_ctrl_z,    NULL,             ""             },
    [ESC]         = { NULL,           exec_escape,      "m n"          },
    ['\x1C']      = { scan_bad,       NULL,             ""             },
    ['\x1D']      = { scan_bad,       NULL,             ""             },
    ['\x1E']      = { NULL,           NULL,             ""             },
    ['\x1F']      = { scan_operator,  NULL,             ""             },
    [SPACE]       = { NULL,           NULL,             ""             },
    ['!']         = { NULL,           exec_bang,        "@ 1"          },
    ['"']         = { scan_quote,     exec_quote,       "n"            },
    ['#']         = { scan_operator,  NULL,             ""             },
    ['$']         = { NULL,           exec_escape,      "m n"          },
    ['%']         = { scan_pct,       NULL,             "n q"          },
    ['&']         = { scan_operator,  NULL,             ""             },
    ['\'']        = { NULL,           exec_apos,        ""             },
    ['(']         = { scan_operator,  NULL,             ""             },
    [')']         = { scan_operator,  NULL,             ""             },
    ['*']         = { scan_operator,  NULL,             ""             },
    ['+']         = { scan_operator,  NULL,             ""             },
    [',']         = { scan_comma,     NULL,             ""             },
    ['-']         = { scan_operator,  NULL,             ""             },
    ['.']         = { scan_dot,       NULL,             ""             },
    ['/']         = { scan_operator,  NULL,             ""             },
    ['0']         = { scan_digits,    NULL,             ""             },
    ['1']         = { scan_digits,    NULL,             ""             },
    ['2']         = { scan_digits,    NULL,             ""             },
    ['3']         = { scan_digits,    NULL,             ""             },
    ['4']         = { scan_digits,    NULL,             ""             },
    ['5']         = { scan_digits,    NULL,             ""             },
    ['6']         = { scan_digits,    NULL,             ""             },
    ['7']         = { scan_digits,    NULL,             ""             },
    ['8']         = { scan_digits,    NULL,             ""             },
    ['9']         = { scan_digits,    NULL,             ""             },
    [':']         = { scan_mod,       NULL,             ""             },
    [';']         = { NULL,           exec_semi,        "n :"          },
    ['<']         = { NULL,           exec_lt,          "n"            },
    ['=']         = { NULL,           exec_equals,      "n :"          },
    ['>']         = { NULL,           exec_gt,          ""             },
    ['?']         = { NULL,           exec_question,    ""             },
    ['@']         = { scan_mod,       NULL,             ""             },
    ['A']         = { scan_A,         exec_A,           "n :"          },
    ['B']         = { scan_B,         NULL,             ""             },
    ['C']         = { NULL,           exec_C,           "n :"          },
    ['D']         = { NULL,           exec_D,           "m n :"        },
    ['E']         = { NULL,           NULL,             ""             },
    ['F']         = { NULL,           NULL,             ""             },
    ['G']         = { NULL,           exec_G,           ":q"           },
    ['H']         = { scan_H,         NULL,             ""             },
    ['I']         = { NULL,           exec_I,           "n @ 1"        },
    ['J']         = { NULL,           exec_J,           "n :"          },
    ['K']         = { NULL,           exec_K,           "m n"          },
    ['L']         = { NULL,           exec_L,           "n"            },
    ['M']         = { NULL,           exec_M,           "m n : q"      },
    ['N']         = { NULL,           exec_N,           "n : @ 1"      },
    ['O']         = { NULL,           exec_O,           "n @ 1"        },
    ['P']         = { NULL,           exec_P,           "m n H : W"    },
    ['Q']         = { scan_Q,         NULL,             "n : q"        },
    ['R']         = { NULL,           exec_R,           "n :"          },
    ['S']         = { NULL,           exec_S,           "m n : :: @ 1" },
    ['T']         = { NULL,           exec_T,           "m n :"        },
    ['U']         = { NULL,           exec_U,           "m n q"        },
    ['V']         = { NULL,           exec_V,           "m n"          },
    ['W']         = { scan_W,         exec_W,           "m n :"        },
    ['X']         = { NULL,           exec_X,           "m n : q"      },
    ['Y']         = { NULL,           exec_Y,           "n :"          },
    ['Z']         = { scan_Z,         NULL,             ""             },
    ['[']         = { NULL,           exec_lbracket,    "q"            },
    ['\\']        = { scan_back,      exec_back,        "n"            },
    [']']         = { NULL,           exec_rbracket,    ": q"          },
    ['^']         = { NULL,           NULL,             ""             },
    ['_']         = { NULL,           exec_ubar,        "n : @ 1"      },
    ['`']         = { scan_bad,       NULL,             ""             },
    ['a']         = { NULL,           NULL,             ""             },
    ['b']         = { NULL,           NULL,             ""             },
    ['c']         = { NULL,           NULL,             ""             },
    ['d']         = { NULL,           NULL,             ""             },
    ['e']         = { NULL,           NULL,             ""             },
    ['f']         = { NULL,           NULL,             ""             },
    ['g']         = { NULL,           NULL,             ""             },
    ['h']         = { NULL,           NULL,             ""             },
    ['i']         = { NULL,           NULL,             ""             },
    ['j']         = { NULL,           NULL,             ""             },
    ['k']         = { NULL,           NULL,             ""             },
    ['l']         = { NULL,           NULL,             ""             },
    ['m']         = { NULL,           NULL,             ""             },
    ['n']         = { NULL,           NULL,             ""             },
    ['o']         = { NULL,           NULL,             ""             },
    ['p']         = { NULL,           NULL,             ""             },
    ['q']         = { NULL,           NULL,             ""             },
    ['r']         = { NULL,           NULL,             ""             },
    ['s']         = { NULL,           NULL,             ""             },
    ['t']         = { NULL,           NULL,             ""             },
    ['u']         = { NULL,           NULL,             ""             },
    ['v']         = { NULL,           NULL,             ""             },
    ['w']         = { NULL,           NULL,             ""             },
    ['x']         = { NULL,           NULL,             ""             },
    ['y']         = { NULL,           NULL,             ""             },
    ['z']         = { NULL,           NULL,             ""             },
    ['{']         = { scan_bad,       NULL,             ""             },
    ['|']         = { NULL,           exec_vbar,        ""             },
    ['}']         = { scan_bad,       NULL,             ""             },
    ['~']         = { scan_bad,       NULL,             ""             },
    [DEL]         = { scan_bad,       NULL,             ""             },
};

///  @var    null_cmd
///  @brief  Initial command block values.

struct cmd null_cmd =
{
    .level      = 0,
    .flag       = 0,
    .c1         = NUL,
    .c2         = NUL,
    .c3         = NUL,
    .m_arg      = 0,
    .n_arg      = 0,
    .paren      = 0,
    .delim      = ESC,
    .qreg       = NUL,
    .qlocal     = false,
    .expr       = { .buf = NULL, .len = 0},
    .text1      = { .buf = NULL, .len = 0},
    .text2      = { .buf = NULL, .len = 0},
    .next       = NULL,
    .prev       = NULL,
};


///
///  @brief    Execute command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_cmd(void)
{
    struct cmd cmd = null_cmd;

    scan_state = SCAN_NULL;

    // Loop for all characters in command string.

    for (;;)
    {
        // The following is used to note the start of any expression that may
        // precede the actual command. Until we start one, we will keep setting
        // a pointer to our current location in the command string. Once we
        // start an expression, we will increment the count for each character
        // parsed. Once we're done with the expression, we will have a pointer
        // to the start of it, along with the length of the no. of characters.
        //
        // Note that one of the reasons we do it this way is so that we will
        // count will be correct even if the expression contains characters
        // such as spaces which don't affect its evaluation, but which need to
        // be counted anyway.

        if (cmd.expr.len == 0)
        {
            cmd.expr.buf = next_buf();
        }

        if (next_buf() == NULL)         // If end of command string,
        {
            break;                      //  then back to main loop
        }

        int c = fetch_buf();            // Get next command string character

        // Flag that command execution has started. This is placed after we
        // call fetch_buf(), so that any CTRL/C causes return to main loop
        // without an error message.

        // TODO: add check for trace mode

        f.ei.exec = true;

        if ((uint)c >= countof(cmd_table))
        {
            printc_err(E_ILL, c);       // Illegal character
        }

        const struct cmd_table *table = scan_cmd(&cmd, c);

        if (table->scan != NULL)
        {
            (*table->scan)(&cmd);
        }
        else if (table->exec != NULL)
        {
            scan_state = SCAN_DONE;
        }

        // Some commands have a postfix Q-register, which is an alphanumeric name
        // optionally preceded by a . (to flag that it's a local and not a global
        // Q-register.

        if (cmd.q_req)                  // Do we need a Q-register?
        {
            c = fetch_buf();            // Yes

            if (c == '.')               // Is it a local Q-register?
            {
                cmd.qlocal = true;      // Yes, mark it

                c = fetch_buf();        // Get Q-register name
            }        

            if (!isalnum(c))
            {
                // The following allows use of G* and G_

                if (toupper(cmd.c1) != 'G' || (c != '*' && c != '_'))
                {
                    printc_err(E_IQN, c); // Illegal Q-register name
                }
            }

            cmd.qreg = (char)c;         // Save the name
        }

        if (scan_state == SCAN_EXPR)    // Still scanning expression?
        {
            cmd.expr.len = (uint)(next_buf() - cmd.expr.buf);
        }
        else if (scan_state == SCAN_DONE) // Done scanning expression?
        {
            finish_cmd(&cmd, table);

            scan_state = SCAN_NULL;
        }

        f.ei.exec = false;              // Suspend this to check CTRL/C

        if (f.ei.ctrl_c)                // If CTRL/C typed, return to main loop
        {
            f.ei.ctrl_c = false;

            print_err(E_XAB);
        }
    }
}


///
///  @brief    Process ESCape which terminates a command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_escape(struct cmd *cmd)
{
    assert(cmd != NULL);

    init_expr();
}


///
///  @brief    Do a full scan of the expression. All we've done up to now is to
///            check the expression for form. Now we check it for substance.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_expr(struct cmd *cmd)
{
    assert(cmd != NULL);

    cmd->m_set = false;
    cmd->n_set = false;
    cmd->colon_set = false;

    if (cmd->c1 == ESC)
    {
        return;
    }

    const char *p = cmd->expr.buf;

    while (p < cmd->expr.buf + cmd->expr.len)
    {
        struct cmd_table *table = NULL;
        int c = *p++;

        if ((uint)c >= countof(cmd_table))
        {
            printc_err(E_ILL, c);
        }

        cmd->c1 = (char)c;

        if (toupper(c) == 'E')          // E{x} command
        {
            assert(p < cmd->expr.buf + cmd->expr.len);

            cmd->c2 = (char)*p++;

            table = scan_E(cmd);
        }
        else if (toupper(c) == 'F')     // F{x} command
        {
            assert(p < cmd->expr.buf + cmd->expr.len);

            cmd->c2 = (char)*p++;

            table = scan_F(cmd);
        }
        else if (c == '^')              // ^{x} or ^^{x} command
        {
            assert(p < cmd->expr.buf + cmd->expr.len);

            if ((c = *p++) == '^')      // ^^{x} - value of character
            {
                assert(p < cmd->expr.buf + cmd->expr.len);

                push_expr(*p++, EXPR_VALUE);

                continue;
            }
            else                        // ^{x}
            {
                cmd->c1 = (char)c;
                cmd->c1 = (char)scan_caret(cmd);

                table = &cmd_table[(int)cmd->c1];
            }
        }
        else if (c == '\x1E')           // CTRL/^{x} command
        {
            assert(p < cmd->expr.buf + cmd->expr.len);

            push_expr(*p++, EXPR_VALUE);

            continue;
        }
        else if (isdigit(c))
        {
            long sum = 0;

            // Here when we have a digit. Check to see that it's valid for the current
            // radix, and then loop until we run out of valid digits, at which point we
            // have to return the last character to the command buffer.

            --p;

            while (valid_radix(*p))
            {
                const char *digits = "0123456789ABCDEF";
                const char *digit = strchr(digits, toupper(*p));

                assert(digit != NULL);

                long n = digit - digits;

                sum *= v.radix;
                sum += n;

                if (++p >= cmd->expr.buf + cmd->expr.len)
                {
                    break;
                }
            }

            push_expr((int)sum, EXPR_VALUE);

            continue;
        }
        else                            // Everything else
        {
            cmd->c2 = NUL;

            table = &cmd_table[toupper(c)];
        }

        assert(table != NULL);

        if (table->scan != NULL)
        {
            (*table->scan)(cmd);
        }
    }

    if (operand_expr())
    {
        cmd->n_arg = get_n_arg();
        cmd->n_set = true;
    }
}


///
///  @brief    Finish scanning command, after we have done a preliminary scan
///            for an expression, and for any modifiers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void finish_cmd(struct cmd *cmd, const struct cmd_table *table)
{
    assert(cmd != NULL);
    assert(table != NULL);

    (void)fflush(stdout);

    scan_tail(cmd);                     // Finish scanning command

    // Tags that start with !+ are really comments, so discard them.

    if (cmd->c1 == '!')
    {
        if (cmd->text1.len > 0)
        {
            if (cmd->text1.buf[0] == '+')
            {
                return;
            }
        }
    }

    if (cmd->c1 == '\'' || cmd->c1 == '|' || cmd->c1 == '>')
    {
        --cmd->level;
    }

    if (teco_debug && cmd->c1 != ESC)
    {
        print_cmd(cmd);                 // Print command if debugging
    }

    if (cmd->c1 == '"' || cmd->c1 == '|' || cmd->c1 == '<')
    {
        ++cmd->level;
    }

    if (cmd->expr.len != 0)             // Did we parse an expression?
    {
        init_expr();                    // Yes, so we need to re-initialize
    }

    // Save command characters

    char c1 = cmd->c1;
    char c2 = cmd->c2;
    char c3 = cmd->c3;

    exec_expr(cmd);                     // Execute expression for real

    // Restore command characters

    cmd->c3 = c3;
    cmd->c2 = c2;
    cmd->c1 = c1;

    // If the only thing on the expression stack is a minus sign, then say we
    // have an n argument equal to -1.
    //
    // If we have anything else on the stack after parsing an expression, then
    // that's an error.

    if (cmd->expr.len == 1 && estack.obj[0].type == EXPR_MINUS)
    {
        estack.obj[0].type = EXPR_VALUE;
        estack.obj[0].value = -1;
    }
    else if (estack.level > 0 && estack.obj[estack.level - 1].type != EXPR_VALUE)
    {
        print_err(E_ARG);
    }

    init_expr();

    if (table->exec != NULL)
    {
        if (operand_expr())
        {
            cmd->n_set = true;
            cmd->n_arg = get_n_arg();
        }

        if (!teco_debug || (cmd->c1 == 'e' && cmd->c2 == 'i'))
            (*table->exec)(cmd);
    }

    // Re-initialize command block, but keep m and n arguments for next command.
    
    uint m_set = cmd->m_set;
    int m_arg  = cmd->m_arg;
    uint n_set = cmd->n_set;
    int n_arg  = cmd->n_arg;
    uint h_set = cmd->h_set;
    uint level = cmd->level;
    
    *cmd = null_cmd;

    cmd->m_set = m_set;
    cmd->m_arg = m_arg;
    cmd->n_set = n_set;
    cmd->n_arg = n_arg;
    cmd->h_set = h_set;
    cmd->level = level;
}

