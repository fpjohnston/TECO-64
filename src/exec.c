///
///  @file    exec.c
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

jmp_buf jump_command;

static void (*commands[])(void) =
{
    [NUL]    = exec_nul,            // NUL
    [CTRL_A] = exec_ctrl_a,         // ^A
    [CTRL_B] = exec_ctrl_b,         // ^B
    [CTRL_C] = exec_ctrl_c,         // ^C
    [CTRL_D] = exec_ctrl_d,         // ^D
    [CTRL_E] = exec_ctrl_e,         // ^E
    [CTRL_F] = exec_ctrl_f,         // ^F
    [CTRL_G] = exec_bad,            // ^G
    [BS]     = exec_ctrl_h,         // ^H
    [TAB]    = exec_ctrl_i,         // ^I
    [LF]     = exec_nul,            // LF
    [VT]     = exec_bad,            // VT
    [FF]     = exec_ctrl_l,         // ^L
    [CR]     = exec_nul,            // CR
    [CTRL_N] = exec_ctrl_n,         // ^N
    [CTRL_O] = exec_ctrl_o,         // ^O
    [CTRL_P] = exec_ctrl_p,         // ^P
    [CTRL_Q] = exec_ctrl_q,         // ^Q
    [CTRL_R] = exec_ctrl_r,         // ^R
    [CTRL_S] = exec_ctrl_s,         // ^S
    [CTRL_T] = exec_ctrl_t,         // ^T
    [CTRL_U] = exec_ctrl_u,         // ^U
    [CTRL_V] = exec_ctrl_v,         // ^V
    [CTRL_W] = exec_ctrl_w,         // ^W
    [CTRL_X] = exec_ctrl_x,         // ^X
    [CTRL_Y] = exec_ctrl_y,         // ^Y
    [CTRL_Z] = exec_ctrl_z,         // ^Z
    [ESC]    = exec_escape,         // ESC
    [FS]     = exec_bad,            /* ^\ */
    [GS]     = exec_bad,            // ^]
    [RS]     = exec_ctrl_caret,     // ^^
    [US]     = exec_ctrl_ubar,      // ^_
    [SPACE]  = exec_nul,            // SP
    ['!']    = exec_bang,
    ['"']    = exec_quote,
    ['#']    = exec_operator,
    ['$']    = exec_escape,
    ['%']    = exec_pct,
    ['&']    = exec_operator,
    ['\'']   = exec_nul,
    ['(']    = exec_operator,
    [')']    = exec_rparen,
    ['*']    = exec_operator,
    ['+']    = exec_operator,
    [',']    = exec_comma,
    ['-']    = exec_operator,
    ['.']    = exec_dot,
    ['/']    = exec_operator,
    ['0']    = exec_digit,
    ['1']    = exec_digit,
    ['2']    = exec_digit,
    ['3']    = exec_digit,
    ['4']    = exec_digit,
    ['5']    = exec_digit,
    ['6']    = exec_digit,
    ['7']    = exec_digit,
    ['8']    = exec_digit,
    ['9']    = exec_digit,
    [':']    = exec_colon,
    [';']    = exec_semi,
    ['<']    = exec_langle,
    ['=']    = exec_equals,
    ['>']    = exec_rangle,
    ['?']    = exec_question,
    ['@']    = exec_atsign,
    ['A']    = exec_A,
    ['B']    = exec_B,
    ['C']    = exec_C,
    ['D']    = exec_D,
    ['E']    = exec_E,
    ['F']    = exec_F,
    ['G']    = exec_G,
    ['H']    = exec_H,
    ['I']    = exec_I,
    ['J']    = exec_J,
    ['K']    = exec_K,
    ['L']    = exec_L,
    ['M']    = exec_M,
    ['N']    = exec_N,
    ['O']    = exec_O,
    ['P']    = exec_P,
    ['Q']    = exec_Q,
    ['R']    = exec_R,
    ['S']    = exec_S,
    ['T']    = exec_T,
    ['U']    = exec_U,
    ['V']    = exec_V,
    ['W']    = exec_W,
    ['X']    = exec_X,
    ['Y']    = exec_Y,
    ['Z']    = exec_Z,
    ['[']    = exec_lbracket,
    ['\\']   = exec_backslash,      /* \ */
    [']']    = exec_rbracket,
    ['^']    = exec_caret,
    ['_']    = exec_ubar,
    ['`']    = exec_bad,
    ['a']    = exec_A,
    ['b']    = exec_B,
    ['c']    = exec_C,
    ['d']    = exec_D,
    ['e']    = exec_E,
    ['f']    = exec_F,
    ['g']    = exec_G,
    ['h']    = exec_H,
    ['i']    = exec_I,
    ['j']    = exec_J,
    ['k']    = exec_K,
    ['l']    = exec_L,
    ['m']    = exec_M,
    ['n']    = exec_N,
    ['o']    = exec_O,
    ['p']    = exec_P,
    ['q']    = exec_Q,
    ['r']    = exec_R,
    ['s']    = exec_S,
    ['t']    = exec_T,
    ['u']    = exec_U,
    ['v']    = exec_V,
    ['w']    = exec_W,
    ['x']    = exec_X,
    ['y']    = exec_Y,
    ['z']    = exec_Z,
    ['{']    = exec_bad,
    ['|']    = exec_vbar,
    ['}']    = exec_bad,
    ['~']    = exec_bad,
    [DEL]    = exec_bad,
};

struct cmd cmd =
{
    .next   = NULL,
    .prev   = NULL,
    .cmd    = NUL,
    .subcmd = NUL,
    .arg1   = { .str = NUL, .count = 0 },
    .arg2   = { .str = NUL, .count = 0 },
    .exec   = NULL,
};
    

///
///  @brief    Execute command string.
///
///  @returns  EXIT_SUCCESS or EXIT_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////

void exec_cmd(void)
{
    int c;

    while ((c = fetch_cmd()) != EOF)
    {
        last_c = c;

        if (trace_mode)
        {
            echo_chr(c);
        }

        if ((uint)c >= countof(commands))
        {
            (void)exec_bad();
        }
        else if (setjmp(jump_command) == 0)
        {
            f.ei.exec = true;

            cmd.cmd        = c;
            cmd.subcmd     = NUL;
            cmd.arg1.str   = NUL;
            cmd.arg2.count = 0;
            cmd.arg2.str   = NUL;
            cmd.arg2.count = 0;

            (*commands[c])();

            if (f.ei.ctrl_c)
            {
                f.ei.ctrl_c = false;

                print_err(E_XAB);

                // TODO: do we need to return success instead of restarting?
            }
        }
        else                            // Here if an error occurs
        {
            init_expr();
            reset_cmd();
            // TODO: check for unclosed loop
            // print_err(E_UTC);
        }

        f.ei.exec = false;
    }
}


///
///  @brief    Translate command starting with a caret (^). Most TECO commands
///            which are control characters (^A, ^B, etc) can also be entered
///            as a caret and letter combination. For example, control-A can
///            also be entered as caret-A.
///
///  @returns  EXIT_SUCCESS or EXIT_FAILURE. (TODO: why?)
///
////////////////////////////////////////////////////////////////////////////////

void exec_caret(void)
{
    int c;

    if ((c = fetch_cmd()) == EOF)
    {
        printc_err(E_IUC, '?');         // No character following ^
    }

    int i = toupper(c) - 'A' + 1;       // Calculate index into array

    if (i <= NUL || i >= SPACE)
    {
        printc_err(E_IUC, c);           // Illegal character following ^
    }

    (*commands[i])();
}
