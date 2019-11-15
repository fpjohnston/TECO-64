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

#include <stdlib.h>

#include "teco.h"
#include "eflags.h"


///
///  @brief    Get or set specified mode control flag. The possible flags are
///            ED, EH, ES, ET, EU, EV, EZ and ^X. The EO flag can only be
///            examined, not set, so it does not use this function. (TODO?)
///
///  @returns  Mode control flag (possibly modified).
///
////////////////////////////////////////////////////////////////////////////////

int get_flag(int flag)
{
    if (!operand_expr())                // Is there an operand available?
    {
        push_expr(flag, EXPR_OPERAND);

        return flag;
    }

    int n_arg = get_n_arg();

    if (!f.ei.comma)                    // m or n argument?
    {
        flag = n_arg;
    }
    else if (m_arg !=0 && n_arg != 0)   // It's m,n<flag>
    {
        flag &= ~m_arg;                 // Turn off m bits
        flag |= n_arg;                  // Turn on n bits
    }
    else if (m_arg == 0 && n_arg != 0)  // It's 0,n<flag>
    {
        flag |= n_arg;                  // Turn on n bits
    }
    else if (m_arg != 0 && n_arg == 0)  // It's m,0<flag>
    {
        flag &= ~m_arg;                 // Turn off m bits
    }

    init_expr();

    return flag;
}
