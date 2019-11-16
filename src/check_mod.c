///
///  @file    check_mod.c
///  @brief   Check command for allowed modifiers.
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

#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "eflags.h"
#include "errors.h"


///
///  @brief    Check command for allowed modifiers.
///
///  @returns  Nothing (exits to main loop if error).
///
////////////////////////////////////////////////////////////////////////////////

void check_mod(int mod)
{
    if (!f.ei.strict)                   // If not in strict mode, skip this
    {
        return;
    }

    if (   (f.ei.atsign    && !(mod & MOD_A))
        || (f.ei.colon     && !(mod & MOD_C))
        || (f.ei.dcolon    && !(mod & MOD_D))
        || (f.ei.comma     && !(mod & MOD_M))
        || (operand_expr() && !(mod & MOD_N)))
    {
        print_err(E_MOD);               // Invalid modifier for command
    }

}
