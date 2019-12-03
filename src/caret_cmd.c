///
///  @file    caret_cmd.c
///  @brief   Execute command beginning with ^ (caret).
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
#include <ctype.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "exec.h"


///
///  @brief    Translate command starting with a caret (^). Most TECO commands
///            which are control characters (^A, ^B, etc) can also be entered
///            as a caret and letter combination. For example, control-A can
///            also be entered as caret-A.
///
///  @returns  Translated control character.
///
////////////////////////////////////////////////////////////////////////////////

int scan_caret(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c = cmd->c1;
    int ctrl = (toupper(c) - 'A') + 1;  // Convert to control character

    if (ctrl <= NUL || ctrl >= SPACE)
    {
        printc_err(E_IUC, c);           // Illegal character following ^
    }

    return ctrl;
}
