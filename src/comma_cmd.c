///
///  @file    comma_cmd.c
///  @brief   Process comma argument separator.
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
#include "errors.h"
#include "exec.h"


///
///  @brief    Process "," (comma argument separator).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_comma(void)
{
    if (f.ei.comma)                     // Already seen m argument?
    {
        print_err(E_ARG);               // Yes, two commas aren't allowed
    }

    if (empty_expr())                   // If nothing on stack,
    {
        print_err(E_NAC);               // No argument before ,
    }

    // If we've seen a comma, then what's on the expression is an "m" argument,
    // not an "n" argument (numeric arguments can take the form m,n).

    m_arg = get_n_arg();                // Save the m argument

    f.ei.comma = true;                  // And say we have one
}
