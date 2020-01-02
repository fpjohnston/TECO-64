///
///  @file    value_cmd.c
///  @brief   Execute commands that return values.
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
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "errors.h"
#include "exec.h"
#include "qreg.h"
#include "textbuf.h"


///
///  @brief    Scan B command: read first position in buffer (always 0).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_B(struct cmd *cmd)
{
    assert(cmd != NULL);

    push_expr(B, EXPR_VALUE);
}


///
///  @brief    Scan ^Q (CTRL/Q) command: get no. of characters between dot and
///            nth line terminator. n may be negative.
///
///  @returns  nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_ctrl_q(struct cmd *unused1)
{
    int nlines = 0;
    
    (void)pop_expr(&nlines);

    int nchrs = getdelta_tbuf(nlines);
    
    push_expr(nchrs, EXPR_VALUE);
}


///
///  @brief    Parse ^S (CTRL/S) command: return negative of last insert, string
///            found, or string inserted with a G command, whichever occurred
///            last.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_ctrl_s(struct cmd *cmd)
{
    assert(cmd != NULL);

    push_expr(v.ctrl_s, EXPR_VALUE);
}


///
///  @brief    Scan ^Y (CTRL/Y) command: equivalent to .+^S,.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_ctrl_y(struct cmd *cmd)
{
    assert(cmd != NULL);

    int dot = (int)getpos_tbuf();

    push_expr(dot + v.ctrl_s, EXPR_VALUE);
    push_expr(dot, EXPR_VALUE);
}


///
///  @brief    Scan ^Z (CTRL/Z) command: get no. of chrs. in all Q-registers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_ctrl_z(struct cmd *cmd)
{
    assert(cmd != NULL);

    uint n = get_qall();
    
    push_expr((int)n, EXPR_VALUE);
}



///
///  @brief    Scan . (dot) command: get current position in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_dot(struct cmd *cmd)
{
    assert(cmd != NULL);

    uint dot = getpos_tbuf();

    push_expr((int)dot, EXPR_VALUE);
}


///
///  @brief    Scan H command: equivalent to B,Z.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_H(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (scan.comma_set || cmd->h_set)   // Already seen comma or H?
    {
        print_err(E_ARG);               // Invalid arguments
    }

    uint Z = getsize_tbuf();
    
    cmd->h_set = true;

    push_expr(B, EXPR_VALUE);
    push_expr((int)Z, EXPR_VALUE);
}


///
///  @brief    Scan Z command: read last position in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_Z(struct cmd *cmd)
{
    assert(cmd != NULL);

    uint Z = getsize_tbuf();

    push_expr((int)Z, EXPR_VALUE);
}
