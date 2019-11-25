///
///  @file    get_flag.c
///  @brief   Get and set flag values.
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
#include "exec.h"


///
///  @brief    Get or set specified mode control flag. The possible flags are
///            ED, EH, ES, ET, EU, EV, EZ and ^X. The EO flag can only be
///            examined, not set, so it does not use this function. (TODO?)
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void get_flag(uint *flag, struct cmd *cmd)
{
    assert(flag != NULL);
    assert(cmd != NULL);

    if (cmd->got_n)                     // n argument?
    {
        if (!cmd->got_m)                // m argument too?
        {
            *flag = (uint)cmd->n;       // No, so just set flag
        }
        else                            // Both m and n were specified
        {
            if (cmd->m != 0)
            {
                *flag &= ~(uint)cmd->m; // Turn off m bits
            }
            if (cmd->n != 0)
            {
                *flag |= (uint)cmd->n;  // Turn on n bits
            }
        }
    }
    else
    {
        push_expr((int)*flag, EXPR_OPERAND);
    }
}
