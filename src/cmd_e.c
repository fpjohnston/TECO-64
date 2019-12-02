///
///  @file    cmd_e.c
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
#include <string.h>

#include "teco.h"
#include "errors.h"
#include "exec.h"

///  @var    cmd_e_table
///  @brief  Table for all commands starting with E.

static struct cmd_table cmd_e_table[] =
{
    { NULL,       exec_EA,      ""        },
    { NULL,       exec_EB,      ": @ 1"   },
    { scan_EC,    exec_EC,      "n"       },
    { scan_ED,    exec_ED,      "m n"     },
    { scan_EE,    exec_EE,      "n"       },
    { NULL,       exec_EF,      ""        },
    { NULL,       exec_EG,      ": @ 1"   },
    { scan_EH,    exec_EH,      "m n"     },
    { NULL,       exec_EI,      ": @ 1"   },
    { scan_EJ,    NULL,         "n"       },
    { NULL,       exec_EK,      ""        },
    { NULL,       exec_EL,      "@ 1"     },
    { NULL,       exec_EM,      "n"       },
    { NULL,       exec_EN,      ": @ 1"   },
    { scan_EO,    NULL,         "n"       },
    { NULL,       exec_EP,      ""        },
    { NULL,       exec_EQ,      ": @ q 1" },
    { NULL,       exec_ER,      ": @ 1"   },
    { scan_ES,    exec_ES,      "n"       },
    { scan_ET,    exec_ET,      "m n"     },
    { scan_EU,    exec_EU,      "n"       },
    { scan_EV,    exec_EV,      "n"       },
    { NULL,       exec_EW,      "@ 1"     },
    { NULL,       exec_EX,      ""        },
    { NULL,       exec_EY,      ":"       },
    { scan_EZ,    exec_EZ,      "m n"     },
    { NULL,       exec_E_pct,   ": @ q 1" },
    { NULL,       exec_E_ubar,  "n @ 1"   },
};


///
///  @brief    Scan E command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

struct cmd_table *scan_E(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c = cmd->c2;

    const char *e_cmds = "ABCDEFGHIJKLMNOPQRSTUVWXYZ%_";
    const char *e_cmd  = strchr(e_cmds, toupper(c));

    if (e_cmd == NULL)
    {
        printc_err(E_IEC, c);           // Illegal F character
    }

    cmd->c2 = (char)c;

    uint i = (uint)(e_cmd - e_cmds);

    assert(i < countof(cmd_e_table));

    return &cmd_e_table[i];
}
