///
///  @file    cmd_exec.c
///  @brief   Execute command string.
///
///  @author  Nowwith Treble Software
///
///  @bug     No known bugs.
///
///  @copyright  tbd
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

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"

struct vars
{
    int b;                              // Beginning of buffer = 0
    int z;                              // End of buffer
    int dot;                            // Current pointer position
    int eof;                            // End of file flag
};

struct vars vars =
{
    .b   = 0,
    .z   = 1000,
    .dot = 0,
    .eof = 0,
};

// The following is used when a command is done and we want to skip to the
// next command.

jmp_buf jump_command;

// Local functions

static const struct cmd_table *scan_caret(struct cmd *cmd);


//
//  Dispatch table, defining functions to call for each character read, as well
//  as the options for each command.
//
//  Note that to avoid duplication, lower-case letters can be omitted, because
//  we translate characters to upper-case before indexing into this table.
//
//  Also, we handle E and F commands specially, as they involve a 2nd character.
//

static const struct cmd_table cmd_table[] =
{
    [NUL]         = { NULL,       NULL,             0                        },
    [CTRL_A]      = { scan_done,  exec_ctrl_a,      _A | _T1                 },
    [CTRL_B]      = { scan_expr,  exec_ctrl_b,      0                        },
    [CTRL_C]      = { NULL,       exec_ctrl_c,      0                        },
    [CTRL_D]      = { scan_done,  exec_ctrl_d,      0                        },
    [CTRL_E]      = { scan_expr,  exec_ctrl_e,      0                        },
    [CTRL_F]      = { scan_expr,  exec_ctrl_f,      _N                       },
    [CTRL_G]      = { scan_bad,   NULL,             0                        },
    [BS]          = { scan_expr,  exec_ctrl_h,      0                        },
    [TAB]         = { scan_done,  exec_ctrl_i,      _T1                      },
    [LF]          = { NULL,       exec_nul,         0                        },
    [VT]          = { scan_bad,   NULL,             0                        },
    [FF]          = { NULL,       exec_ctrl_l,      0                        },
    [CR]          = { NULL,       NULL,             0                        },
    [CTRL_N]      = { scan_expr,  exec_ctrl_n,      0                        },
    [CTRL_O]      = { scan_done,  exec_ctrl_o,      0                        },
    [CTRL_P]      = { scan_bad,   exec_ctrl_p,      0                        },
    [CTRL_Q]      = { NULL,       exec_ctrl_q,      0                        },
    [CTRL_R]      = { NULL,       exec_ctrl_r,      _N                       },
    [CTRL_S]      = { scan_expr,  exec_ctrl_s,      0                        },
    [CTRL_T]      = { NULL,       exec_ctrl_t,      _C                       },
    [CTRL_U]      = { scan_done,  exec_ctrl_u,      _A | _C | _N | _Q | _T1  },
    [CTRL_V]      = { NULL,       exec_ctrl_v,      0                        },
    [CTRL_W]      = { NULL,       exec_ctrl_w,      0                        },
    [CTRL_X]      = { scan_expr,  exec_ctrl_x,      _N                       },
    [CTRL_Y]      = { scan_expr,  exec_ctrl_y,      0                        },
    [CTRL_Z]      = { scan_expr,  exec_ctrl_z,      0                        },
    [ESC]         = { scan_done,  exec_escape,      _MN                      },
    [FS]          = { scan_bad,   NULL,             0                        },
    [GS]          = { scan_bad,   NULL,             0                        },
    [RS]          = { NULL,       exec_ctrl_caret,  0                        },
    [US]          = { scan_expr,  exec_ctrl_ubar,   0                        },
    [SPACE]       = { NULL,       NULL,             0                        },
    ['!']         = { scan_done,  exec_bang,        _A | _T1                 },
    ['"']         = { scan_done,  exec_quote,       0                        },
    ['#']         = { scan_expr,  exec_operator,    0                        },
    ['$']         = { scan_done,  exec_escape,      _MN                      },
    ['%']         = { scan_done,  exec_pct,         _N | _Q                  },
    ['&']         = { scan_expr,  exec_operator,    0                        },
    ['\'']        = { scan_done,  exec_apos,        0                        },
    ['(']         = { scan_expr,  exec_operator,    0                        },
    [')']         = { scan_expr,  exec_rparen,      0                        },
    ['*']         = { scan_expr,  exec_operator,    0                        },
    ['+']         = { scan_expr,  exec_operator,    0                        },
    [',']         = { scan_expr,  exec_comma,       0                        },
    ['-']         = { scan_expr,  exec_operator,    0                        },
    ['.']         = { scan_expr,  exec_dot,         0                        },
    ['/']         = { scan_expr,  exec_operator,    0                        },
    ['0' ... '9'] = { scan_expr,  NULL,             0                        },
    [':']         = { scan_mod,   NULL,             0                        },
    [';']         = { scan_done,  exec_semi,        _C | _N                  },
    ['<']         = { scan_done,  exec_langle,      _N                       },
    ['=']         = { scan_done,  exec_equals,      _C | _N                  },
    ['>']         = { scan_done,  exec_rangle,      0                        },
    ['?']         = { scan_done,  exec_question,    0                        },
    ['@']         = { scan_mod,   NULL,             0                        },
    ['A']         = { scan_done,  exec_A,           _C | _N                  },
    ['B']         = { scan_expr,  exec_B,           0                        },
    ['C']         = { scan_done,  exec_C,           _C | _N                  },
    ['D']         = { scan_done,  exec_D,           _C | _MN                 },
    ['E']         = { NULL,       NULL,             0                        },
    ['F']         = { NULL,       NULL,             0                        },
    ['G']         = { scan_done,  exec_G,           _C | _Q                  },
    ['H']         = { scan_expr,  exec_H,           0                        },
    ['I']         = { scan_done,  exec_I,           _A | _N | _T1            },
    ['J']         = { scan_done,  exec_J,           _C | _N                  },
    ['K']         = { scan_done,  exec_K,           _MN                      },
    ['L']         = { scan_done,  exec_L,           _N                       },
    ['M']         = { scan_done,  exec_M,           _C | _MN | _Q            },
    ['N']         = { scan_done,  exec_N,           _A | _C | _N | _T1       },
    ['O']         = { scan_done,  exec_O,           _A | _N | _T1            },
    ['P']         = { scan_done,  exec_P,           _C | _MN | _W            },
    ['Q']         = { scan_done,  exec_Q,           _C | _Q                  },
    ['R']         = { scan_done,  exec_R,           _C | _N                  },
    ['S']         = { scan_done,  exec_S,           _A | _C | _D | _MN | _T1 },
    ['T']         = { scan_done,  exec_T,           _C | _MN                 },
    ['U']         = { scan_done,  exec_U,           _MN | _Q                 },
    ['V']         = { scan_done,  exec_V,           _MN                      },
    ['W']         = { scan_done,  exec_W,           _C | _MN                 },
    ['X']         = { scan_done,  exec_X,           _C | _MN | _Q            },
    ['Y']         = { scan_done,  exec_Y,           _C | _N                  },
    ['Z']         = { scan_expr,  exec_Z,           0                        },
    ['[']         = { scan_done,  exec_lbracket,    _Q                       },
    ['\\']        = { scan_expr,  exec_backslash,   0                        },
    [']']         = { scan_done,  exec_rbracket,    _C | _Q                  },
    ['^']         = { NULL,       NULL,             0                        },
    ['_']         = { scan_expr,  exec_ubar,        _A | _C | _N | _T1       },
    ['`']         = { scan_bad,   NULL,             0                        },
    ['a' ... 'z'] = { NULL,       NULL,             0                        },
    ['{']         = { scan_bad,   NULL,             0                        },
    ['|']         = { scan_done,  exec_vbar,        0                        },
    ['}']         = { scan_bad,   NULL,             0                        },
    ['~']         = { scan_bad,   NULL,             0                        },
    [DEL]         = { scan_bad,   NULL,             0                        },
};

// Define initial values for command block

const struct cmd null_cmd =
{
    .state      = CMD_NULL,
    .level      = 0,
    .flag       = 0,
    .c1         = NUL,
    .c2         = NUL,
    .c3         = NUL,
    .m          = 0,
    .n          = 0,
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


// Local functions

static void exec_expr(struct cmd *cmd);

static void set_opts(struct cmd *cmd, int opts);


///
///  @brief    Execute command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_cmd(void)
{
    struct cmd cmd = null_cmd;

    // Here to start parsing a command string,  one character at a time. Note
    // that although some commands may contain only a single character, most of
    // them comprise multiple characters, so we have to keep looping until we
    // have found everything we need. As we continue, we store information in
    // the command block defined by 'cmd', for use when we're ready to actually
    // execute the command. This includes such things as m and n arguments,
    // modifiers such as : and @, and any text strings followed the command.

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
            cmd.expr.buf = next_cmd();
        }

        if (next_cmd() == NULL)
        {
            break;
        }

        int c = fetch_cmd();

        // Flag that command execution has started. This is placed after we
        // read the next character, so that any CTRL/C causes return to main
        // loop without an error message.

        // TODO: add check for trace mode

        f.ei.exec = true;

        if ((uint)c >= countof(cmd_table))
        {
            printc_err(E_ILL, c);
        }
        else if (setjmp(jump_command) == 0)
        {
            const struct cmd_table *table;

            cmd.c1 = c;

            if (toupper(c) == 'E')               // E{x} command
            {
                cmd.c2 = fetch_cmd();

                table = init_E(&cmd);
            }
            else if (toupper(c) == 'F')          // F{x} command
            {
                cmd.c2 = fetch_cmd();

                table = init_F(&cmd);
            }
            else if (toupper(c) == '^')          // ^{x} command
            {
                cmd.c1 = fetch_cmd();

                table = scan_caret(&cmd);
            }
            else                                 // Everything else
            {
                cmd.c2 = NUL;

                table = &cmd_table[toupper(c)];
            }

            void (*exec_scan)(struct cmd *cmd) = table->scan;
            void (*exec_cmd)(struct cmd *cmd) = table->exec;
            
            if (exec_scan == NULL)
            {
                continue;
            }
 
            set_opts(&cmd, table->opts);

            f.ei.dryrun = true;

            (*exec_scan)(&cmd);

            f.ei.dryrun = false;

            if (cmd.state == CMD_EXPR)
            {
                cmd.expr.len = next_cmd() - cmd.expr.buf;
            }
            else if (cmd.state == CMD_DONE)
            {
                scan_cmd(&cmd);         // Finish scanning command

                if (c != ESC || (c == ESC && cmd.expr.len != 0))
                {
                    print_cmd(&cmd);    // TODO: debugging only
                }
                else if (c == ESC)
                {
                    printf("<ESC>\r\n");
                    fflush(stdout);
                }

                init_expr();
                exec_expr(&cmd);        // Execute expression

                if (exec_cmd != NULL)
                {
                    (*exec_cmd)(&cmd);

                    cmd = null_cmd;
                }
            }
        }
        else
        {
            reset_cmd();

            // TODO: check for unclosed loop
            // print_err(E_UTC);
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
    cmd->got_m = false;
    cmd->got_n = false;

//    init_expr();

    if (cmd->c1 == ESC)
    {
        return;
    }

    const char *p = cmd->expr.buf;

    while (p < cmd->expr.buf + cmd->expr.len)
    {
        const struct cmd_table *table;
        uint c = *p++;

        if (c >= countof(cmd_table))
        {
            printc_err(E_ILL, c);
        }

        cmd->c1 = c;

        if (toupper(c) == 'E')          // E{x} command
        {
            cmd->c2 = *p++;

            assert(p <= cmd->expr.buf + cmd->expr.len);

            table = init_E(cmd);
        }
        else if (toupper(c) == 'F')     // F{x} command
        {
            cmd->c2 = *p++;

            assert(p <= cmd->expr.buf + cmd->expr.len);

            table = init_F(cmd);
        }
        else if (toupper(c) == '^')     // ^{x} command
        {
            cmd->c1 = *p++;

            assert(p <= cmd->expr.buf + cmd->expr.len);

            table = scan_caret(cmd);
        }
        else                            // Everything else
        {
            cmd->c2 = NUL;

            table = &cmd_table[toupper(c)];
        }

        void (*exec_expr)(struct cmd *cmd) = table->exec;

        if (exec_expr != NULL)
        {
            (*exec_expr)(cmd);
        }
        else if (isdigit(c))
        {
            --p;

            int maxchrs = cmd->expr.buf + cmd->expr.len - p;
            int ndigits = exec_digit(p, maxchrs);

            p += ndigits;

            assert(p <= cmd->expr.buf + cmd->expr.len);
        }
    }
}


///
///  @brief    Translate command starting with a caret (^). Most TECO commands
///            which are control characters (^A, ^B, etc) can also be entered
///            as a caret and letter combination. For example, control-A can
///            also be entered as caret-A.
///
///  @returns  true if command completed, else false.
///
////////////////////////////////////////////////////////////////////////////////

static const struct cmd_table *scan_caret(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c = cmd->c1;
    int ctrl = toupper(c) - 'A' + 1;    // Convert to control character

    if (ctrl <= NUL || ctrl >= SPACE)
    {
        printc_err(E_IUC, c);           // Illegal character following ^
    }

    cmd->c1 = ctrl;

    return &cmd_table[ctrl];
}


///
///  @brief    Set command options.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void set_opts(struct cmd *cmd, int opts)
{
    assert(cmd != NULL);

    if (opts & _A)  cmd->opt_atsign         = true;

    if (opts & _C)  cmd->opt_colon          = true;

    if (opts & _D)  cmd->opt_dcolon         = true;

    if (opts & _MN) cmd->opt_m = cmd->opt_n = true;

    if (opts & _N)  cmd->opt_n              = true;

    if (opts & _Q)  cmd->opt_qreg           = true;

    if (opts & _T1) cmd->opt_t1             = true;

    if (opts & _T2) cmd->opt_t2             = true;

    if (opts & _W)  cmd->opt_w              = true;
}
