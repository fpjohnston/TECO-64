///
///  @file    exec_e.c
///  @brief   General dispatcher for TECO E commands (e.g., EO, ER, ET).
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "exec.h"

static void (*commands[])(void) =
{
    exec_EA,                        // A
    exec_EB,                        // B
    exec_EC,                        // C
    exec_ED,                        // D
    exec_EE,                        // E
    exec_EF,                        // F
    exec_EG,                        // G
    exec_EH,                        // H
    exec_EI,                        // I
    exec_EJ,                        // J
    exec_EK,                        // K
    exec_EL,                        // L
    exec_EM,                        // M
    exec_EN,                        // N
    exec_EO,                        // O
    exec_EP,                        // P
    exec_EQ,                        // Q
    exec_ER,                        // R
    exec_ES,                        // S
    exec_ET,                        // T
    exec_EU,                        // U
    exec_EV,                        // V
    exec_EW,                        // W
    exec_EX,                        // X
    exec_EY,                        // Y
    exec_EZ,                        // Z
};


///
///  @brief    Execute E command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E(void)
{
    int c = fetch_cmd();                // Get character following E

    command.subcmd = c;

    switch (c)
    {
        case EOF:
            break;

        case '%':                       // E%q
            exec_E_pct();

            break;

        case '_':                       // nE_*
            exec_E_ubar();

            break;

        default:                        // EA through EZ
            if (!isalpha(c))
            {
                printc_err(E_IEC, c);   // Illegal E character
            }

            (*commands[toupper(c) - 'A'])();

            break;
    }
}


void exec_EN(void)
{
    printf("EN command\r\n");

    skip_arg1(ESC);
}


void exec_EQ(void)
{
    printf("EQ command\r\n");

    skip_one();
}


void exec_EY(void)
{
    printf("EY command\r\n");

    skip_cmd();
}


void exec_E_pct(void)
{
    printf("E%% command\r\n");

    skip_one();
}

