///
///  @file    stubs.c
///  @brief   Temporary stub functions for TECO.
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
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "exec.h"


///  @brief    Temporary stub function.
///  @returns  Nothing.

void exec_ctrl_v(struct cmd *unused1)
{
    prints("%s() not yet completed\r\n", __func__);

    return;
}


///  @brief    Temporary stub function.
///  @returns  Nothing.

void exec_ctrl_w(struct cmd *unused1)
{
    prints("%s() not yet completed\r\n", __func__);

    return;
}


///  @brief    Temporary stub function.
///  @returns  Nothing.

void exec_FB(struct cmd *unused1)
{
    prints("%s() not yet completed\r\n", __func__);

    return;
}


///  @brief    Temporary stub function.
///  @returns  Nothing.

void exec_ubar(struct cmd *unused1)
{
    prints("%s() not yet completed\r\n", __func__);

    return;
}
