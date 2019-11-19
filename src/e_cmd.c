///
///  @file    e_cmd.c
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


struct cmd_table
{
    void (*scan)(struct cmd *cmd);  // Scan function
    void (*exec)(struct cmd *cmd);  // Execution function
    enum cmd_opts opts;             // Command options
};

static const struct cmd_table cmd_table[] =
{
    { scan_done,  exec_EA,      0                   },
    { scan_done,  exec_EB,      _A | _C | _T1       },
    { scan_done,  exec_EC,      0                   },
    { scan_flag,  exec_ED,      _MN                 },
    { scan_flag,  exec_EE,      _N                  },
    { scan_done,  exec_EF,      0                   },
    { scan_done,  exec_EG,      _A | _C | _T1       },
    { scan_flag,  exec_EH,      _MN                 },
    { scan_done,  exec_EI,      _A | _T1            },
    { scan_flag,  exec_EJ,      _N                  },
    { scan_done,  exec_EK,      0                   },
    { scan_done,  exec_EL,      _A | _T1            },
    { scan_done,  exec_EM,      _N                  },
    { scan_done,  exec_EN,      _A | _C | _T1       },
    { scan_flag,  exec_EO,      _N                  },
    { scan_done,  exec_EP,      0                   },
    { scan_done,  exec_EQ,      _A | _C | _Q | _T1  },
    { scan_done,  exec_ER,      _A | _C | _T1       },
    { scan_flag,  exec_ES,      _N                  },
    { scan_flag,  exec_ET,      _MN                 },
    { scan_flag,  exec_EU,      _N                  },
    { scan_flag,  exec_EV,      _N                  },
    { scan_done,  exec_EW,      _A | _T1            },
    { scan_done,  exec_EX,      0                   },
    { scan_done,  exec_EY,      _C                  },
    { scan_done,  exec_EZ,      _A | _T1            },
    { scan_done,  exec_E_pct,   _A | _C | _Q | _T1  },
    { scan_done,  exec_E_ubar,  _A | _N | _T1       },
};


///
///  @brief    Initialize E command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

enum cmd_opts init_E(struct cmd *cmd)
{
    int c = fetch_cmd();                // Get character following E
    uint i = toupper(c);                // Index into cmd_table[]
    
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

    i -= 'A';                           // Make index zero-based

    assert(i < countof(cmd_table));

    cmd->c2   = c;
    cmd->scan = cmd_table[i].scan;
    cmd->exec = cmd_table[i].exec;

    return cmd_table[i].opts;
}
