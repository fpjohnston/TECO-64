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

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "exec.h"

struct e_cmds
{
    void (*exec)(void);
    int nargs;
    int mods;
};
    
static struct e_cmds e_cmds[] =
{
    { exec_EA,     0, MOD_NONE },
    { exec_EB,     1, MOD_AC   },
    { exec_EC,     0, MOD_NONE },
    { exec_ED,     0, MOD_MN   },
    { exec_EE,     0, MOD_MN   },
    { exec_EF,     0, MOD_NONE },
    { exec_EG,     1, MOD_AC   },
    { exec_EH,     0, MOD_MN   },
    { exec_EI,     1, MOD_AC   },
    { exec_EJ,     0, MOD_MN   },
    { exec_EK,     0, MOD_NONE },
    { exec_EL,     1, MOD_ACQ  },
    { exec_EM,     0, MOD_AC   },
    { exec_EN,     1, MOD_NONE },
    { exec_EO,     0, MOD_NONE },
    { exec_EP,     0, MOD_NONE },
    { exec_EQ,     1, MOD_ACQ  },
    { exec_ER,     1, MOD_AC   },
    { exec_ES,     0, MOD_MN   },
    { exec_ET,     0, MOD_MN   },
    { exec_EU,     0, MOD_MN   },
    { exec_EV,     0, MOD_MN   },
    { exec_EW,     1, MOD_AC   },
    { exec_EX,     0, MOD_NONE },
    { exec_EY,     0, MOD_ACQ  },
    { exec_EZ,     1, MOD_AC   },
    { exec_E_pct,  1, MOD_ACQ  },
    { exec_E_ubar, 1, MOD_AC   },
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

    if (c == EOF)
    {
        print_err(E_UTC);               // Unterminated command
    }

    uint i = toupper(c);
    
    if (!isalpha(c))
    {
        if (c == '%')
        {
            i = 'Z' + 1;
        }
        else if (c == '_')
        {
            i = 'Z' + 2;
        }
        else
        {
            printc_err(E_IEC, c);       // Illegal E character
        }
    }

    cmd.c2 = c;
    i -= 'A';                           // Make index zero-based

    assert(i < countof(e_cmds));

    int mod = e_cmds[i].mods;           // Get command modifiers

    check_mod(mod);

    (*e_cmds[i].exec)();
}
