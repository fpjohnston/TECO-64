///
///  @file    env.c
///  @brief   System-specific environment functions for Linux.
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
#include <unistd.h>

#include "teco.h"
#include "eflags.h"
#include "errors.h"


///
///  @brief    Initialize environment (read environment variables, logical names,
///            etc.)
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_env(void)
{
    const char *env;

    // TODO: add more functionality here

    if ((env = getenv("TECO_PROMPT")) != NULL)
    {
        prompt = env;                   // Change TECO prompt
    }
}


///
///  @brief    Get information environment about our environment.
///
///  @returns  Result for requested value.
///
////////////////////////////////////////////////////////////////////////////////

#define TECO_HW         101             // x86 platform
#define TECO_OS           2             // Linux O/S

int teco_env(int n_arg)
{
    if (f.ei.comma)                     // Do we have m argument?
    {
        print_err(E_NIH);               // Yes, we can't do that here
    }

    switch (n_arg)
    {
        case -1:
            return (TECO_HW << 8) | TECO_OS;

        case 0:
            return getpid();

        case 1:
        case 2:
            return 0;

        default:
            print_err(E_NIH);
    }
}
