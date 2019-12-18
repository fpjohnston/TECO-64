///
///  @file    get_flag.c
///  @brief   Get and set flag values.
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
#include <stdlib.h>

#include "teco.h"
#include "exec.h"


///
///  @brief    Get specified mode control flag
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void get_flag(struct cmd *cmd, uint flag)
{
    assert(cmd != NULL);

    if (pop_expr(&cmd->n_arg))          // n argument?
    {
        cmd->n_set = true;

        scan_state = SCAN_DONE;
    }
    else
    {
        push_expr((int)flag, EXPR_VALUE); // Assume we're an operand
    }
}


///
///  @brief    Set specified mode control flag. The possible flags are ED, EH,
///            and ET.
///
///  @returns  New value of flag.
///
////////////////////////////////////////////////////////////////////////////////

uint set_flag(struct cmd *cmd, uint flag)
{
    assert(cmd != NULL);

    if (cmd->n_set)
    {
        if (!cmd->m_set)                // m argument too?
        {
            flag = (uint)cmd->n_arg;    // No, so just set flag
        }
        else                            // Both m and n were specified
        {
            if (cmd->m_arg != 0)
            {
                flag &= ~(uint)cmd->m_arg; // Turn off m bits
            }
            if (cmd->n_arg != 0)
            {
                flag |= (uint)cmd->n_arg; // Turn on n bits
            }
        }
    }

    return flag;
}
