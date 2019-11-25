///
///  @file    ej_cmd.c
///  @brief   Execute TECO EJ command; get operating environment information.
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
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "exec.h"


///
///  @brief    Execute EJ command; get operating environment characteristics:
///
///            -1EJ - This has the form (m << 8) + n, where m is a code for the
///                   computer hardware in use, and n is a code for the operating
///                   system, as follows:
///
///                    m   n    -1EJ  Hardware  Operating system
///                   --- ---   ----  --------  ----------------  
///                     0   0      0   PDP-11   RSX-11D
///                     0   1      1   PDP-11   RSX-11M
///                     0   2      2   PDP-11   RSX-11S
///                     0   3      3   PDP-11   IAS
///                     0   4      4   PDP-11   RSTS/E
///                     0   5      5   PDP-11   VAX/VMS (compatibility mode)
///                     0   6      6   PDP-11   RSM-11M+
///                     0   7      7   PDP-11   RT-11
///                     1   0    256   PDP-8    OS/8
///                     2   0    512   DEC-10   TOPS-10
///                     3   0    768   DEC-20   TOPS-20
///                     4   0   1024   VAX-11   VAX/VMS (native mode)
///                     4   1   1025   VAX-11   Ultrix
///                   100   0  25600   Sun      SunOS
///                   101   0  25856   x86      MS-DOS
///                   101   1  25857   x86      OS/2
///                   101   2  25858   x86      Linux
///
///             0EJ - Process number, 0 if single-process system.
///             1EJ - Terminal keyboard number, 0 if single-user system.
///             2EJ - User identification number.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EJ(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!cmd->got_n)                    // n argument?
    {
        cmd->n = 0;                     // No, so assume 0
    }

    cmd->n = teco_env(cmd->n);          // Do the system-dependent part

    push_expr(cmd->n, EXPR_OPERAND);
}
