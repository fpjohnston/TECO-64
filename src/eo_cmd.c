///
///  @file    eo_cmd.c
///  @brief   Execute EO command.
///
///  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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
#include <stdio.h>

#include "teco.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "estack.h"
#include "exec.h"


///
///  @brief    Execute nEO command: set TECO version number. This is a no-op
///            unless we are trying to change the version, in which case we
///            issue an error.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EO(struct cmd *cmd)
{
    assert(cmd != NULL);
    assert(cmd->n_set);
    assert(!cmd->colon);

    if (cmd->n_arg != f.eo.major)       // Error unless current version
    {
        throw(E_NYI);                   // Not yet implemented
    }
}


///
///  @brief    Scan EO command.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_EO(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_M, NO_DCOLON, NO_ATSIGN);

    if (!cmd->n_set)                    // EO?
    {
        store_val(f.eo.major);

        return true;
    }
    else if (!cmd->colon)               // nEO?
    {
        return false;
    }
    else                                // n:EO
    {
        switch (cmd->n_arg)
        {
            case -2:
                store_val(f.eo.patch);

                break;

            case -1:
                store_val(f.eo.minor);

                break;

            case 0:
            default:
                store_val(f.eo.major);

                break;
        }

        return true;
    }
}
