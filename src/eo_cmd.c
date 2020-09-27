///
///  @file    eo_cmd.c
///  @brief   Execute EO command.
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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

#include "teco.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"


///
///  @brief    Scan EO command: read or set TECO version number.
///
///               EO - Major version.
///              :EO - Same as EO.
///             0:EO - Same as EO.
///            -1:EO - Minor version.
///            -2:EO - Patch version.
///
///            nEO - Error (not currently possible to set version number).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EO(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    // If not trying to set value, then just return major version

    if (!cmd->n_set)
    {
        push_expr(MAJOR_VERSION, EXPR_VALUE);

        return;
    }

    // If n specified without colon, that's an error

    if (!cmd->colon)                    // :EO?
    {
        if (cmd->n_arg == MAJOR_VERSION)
        {
            return;                     // Setting current version is okay
        }

        throw(E_NYI);                   // Not yet implemented
    }        

    // Here if we have n:EO - figure out which version to return

    switch (cmd->n_arg)
    {
        case -2:
            push_expr(PATCH_VERSION, EXPR_VALUE);

            break;

        case -1:
            push_expr(MINOR_VERSION, EXPR_VALUE);

            break;

        case 0:
        default:
            push_expr(MAJOR_VERSION, EXPR_VALUE);

            break;
    }
}
