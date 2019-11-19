///
///  @file    colon_cmd.c
///  @brief   Process colon modifier.
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
#include <stdlib.h>

#include "teco.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


///
///  @brief    Process ":" and "::" modifiers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_colon(struct cmd *cmd)
{
    assert(cmd != NULL);

    // If in strict mode, and we don't allow colons, then exit with error.

    if (f.ei.strict && !cmd->opt_colon)
    {
        print_err(E_MOD);
    }

    if (cmd->got_colon)                 // Second colon?
    {
        cmd->got_dcolon = true;
        cmd->got_colon = false;
    }
    else                                // Must be the first colon
    {
        cmd->got_colon = true;
    }

    cmd->state = CMD_MOD;
}
