///
///  @file    f_cmd.c
///  @brief   General dispatcher for TECO F commands (e.g., FR, FS).
///
///           nFB    Search, bounded by n lines
///           m,nFB  Search between locations m and n
///           nFC    Search and replace over n lines
///           m,nFC  Search and replace between locations m and n
///           nFD    Search and delete string
///           nFK    Search and delete intervening text
///           nFN    Global string replace
///           FR     Replace last string
///           nFS    Local string replace
///           F'     Flow to end of conditional
///           F<     Flow to start of iteration
///           F>     Flow to end of iteration
///           nF_    Destructive search and replace
///           F|     Flow to ELSE part of conditional
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
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


static const struct cmd_table cmd_table[] =
{
    { scan_done,  exec_FB,        "m n @ 1"      },
    { scan_done,  exec_FC,        "m n @ 1 2"    },
    { scan_done,  exec_FD,        "m n @ 1"      },
    { scan_done,  exec_FK,        "n @ 1"        },
    { scan_done,  exec_FN,        "n : @ 1 2"    },
    { scan_done,  exec_FR,        "m n @ 1 2"    },
    { scan_done,  exec_FS,        "m n :: @ 1 2" },
    { scan_done,  exec_F_langle,  ""             },
    { scan_done,  exec_F_rangle,  ""             },
    { scan_done,  exec_F_apos,    ""             },
    { scan_done,  exec_F_ubar,    "n @ 1 2"      },
    { scan_done,  exec_F_vbar,    ""             },
};



///
///  @brief    Initialize for F command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

const struct cmd_table *init_F(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c = cmd->c2;

    const char *f_cmds = "BCDKNRS<>\\_|";
    const char *f_cmd  = strchr(f_cmds, toupper(c));

    if (f_cmd == NULL)
    {
        printc_err(E_IFC, c);           // Illegal F character
    }

    cmd->c2 = c;

    return &cmd_table[f_cmd - f_cmds];
}
