///
///  @file    et_cmd.c
///  @brief   Execute TECO ET command.
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
#include "exec.h"


///
///  @brief    Execute ET command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ET(void)
{
    // If any of the following are changed, we need to take further action.

    bool eightbit = f.et.eightbit;
    bool scope    = f.et.scope;
    bool refresh  = f.et.refresh;

    f.et.flag = get_flag(f.et.flag);

    // If user set or cleared 8-bit flag, tell operating system.

    if (eightbit && !f.et.eightbit)
    {
        // TODO: tell operating system we cleared bit?
    }
    else if (!eightbit && f.et.eightbit)
    {
        // TODO: tell operating system we set bit?
    }

    // The following bits cannot be changed by the user.

    f.et.scope   = scope;
    f.et.refresh = refresh;
}

