///
///  @file    cmd_tables.c
///  @brief   Data tables used in executing command strings.
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

#include "teco.h"
#include "ascii.h"
#include "exec.h"


///
///  @var    cmd_table
///
///  @brief  Dispatch table, defining functions to scan execute input
///          characters, as well as the options for each command.
///
///          Note that to avoid duplication, lower-case letters can be omitted,
///          because we translate characters to upper-case before indexing into
///          this table. Also, we handle E and F commands specially, as they
///          involve a 2nd character.
///

///  Types of commands regarding arguments:
///
///  1. used/not used
///  2. consumed/not consumed
///  3. terminal/not terminal
///
///  examples:
///
///  used, consumed, terminal: C, D
///  used, consumed, not terminal: ET, %
///  used, not consumed, terminal:
///  used, not consumed, not terminal:
///  not used, consumed, terminal: >, ", |, '
///  not used, consumed, not terminal:
///  not used, not consumed, terminal: [, ]
///  not used, not consumed, not terminal:
///
///  argument preceding command:
///
///  - can be used and consumed: e.g., 22U0
///  - can be used and passed through: e.g., 47%A
///  - can be consumed without use: e.g., >, ", |, ', ESC
///  - can be passed through: e.g., [, ]
///  - can be an error; e.g., B followed by Z
///
///  commands that:
///
///  - are a value
///  - take a value
///  - make a value
///  - junk a value
///  - ignore a value

const struct cmd_table cmd_table[] =
{
    [NUL]         = { NULL,           NULL,             ""         },
    [CTRL_A]      = { NULL,           exec_ctrl_a,      "@ 1"      },
    [CTRL_B]      = { scan_ctrl_b,    NULL,             ""         },
    [CTRL_C]      = { NULL,           exec_ctrl_c,      ""         },
    [CTRL_D]      = { NULL,           exec_ctrl_d,      ""         },
    [CTRL_E]      = { scan_ctrl_e,    exec_ctrl_e,      ""         },
    [CTRL_F]      = { scan_ctrl_f,    NULL,             ""         },
    [CTRL_G]      = { scan_bad,       NULL,             ""         },
    [CTRL_H]      = { scan_ctrl_h,    NULL,             ""         },
    [CTRL_I]      = { NULL,           exec_ctrl_i,      "1"        },
    [LF]          = { NULL,           NULL,             ""         },
    [VT]          = { scan_bad,       NULL,             ""         },
    [FF]          = { NULL,           NULL,             ""         },
    [CR]          = { NULL,           NULL,             ""         },
    [CTRL_N]      = { scan_ctrl_n,    NULL,             ""         },
    [CTRL_O]      = { NULL,           exec_ctrl_o,      ""         },
    [CTRL_P]      = { scan_bad,       NULL,             ""         },
    [CTRL_Q]      = { scan_ctrl_q,    NULL,             ""         },
    [CTRL_R]      = { scan_ctrl_r,    exec_ctrl_r,      ""         },
    [CTRL_S]      = { scan_ctrl_s,    NULL,             ""         },
    [CTRL_T]      = { scan_ctrl_t,    exec_ctrl_t,      ":"        },
    [CTRL_U]      = { NULL,           exec_ctrl_u,      ": @ q 1"  },
    [CTRL_V]      = { NULL,           exec_ctrl_v,      ""         },
    [CTRL_W]      = { NULL,           exec_ctrl_w,      ""         },
    [CTRL_X]      = { scan_ctrl_x,    exec_ctrl_x,      ""         },
    [CTRL_Y]      = { scan_ctrl_y,    NULL,             ""         },
    [CTRL_Z]      = { scan_ctrl_z,    NULL,             ""         },
    [ESC]         = { NULL,           NULL,             ""         },
    ['\x1C']      = { scan_bad,       NULL,             ""         },
    ['\x1D']      = { scan_bad,       NULL,             ""         },
    ['\x1E']      = { NULL,           NULL,             ""         },
    ['\x1F']      = { scan_operator,  NULL,             ""         },
    [SPACE]       = { NULL,           NULL,             ""         },
    ['!']         = { NULL,           exec_bang,        "@ 1"      },
    ['"']         = { scan_quote,     exec_quote,       ""         },
    ['#']         = { scan_operator,  NULL,             ""         },
    ['$']         = { scan_bad,       NULL,             ""         },
    ['%']         = { scan_pct,       exec_pct,         "q"        },
    ['&']         = { scan_operator,  NULL,             ""         },
    ['\'']        = { NULL,           exec_apos,        ""         },
    ['(']         = { scan_operator,  NULL,             ""         },
    [')']         = { scan_operator,  NULL,             ""         },
    ['*']         = { scan_operator,  NULL,             ""         },
    ['+']         = { scan_operator,  NULL,             ""         },
    [',']         = { scan_comma,     NULL,             ""         },
    ['-']         = { scan_operator,  NULL,             ""         },
    ['.']         = { scan_dot,       NULL,             ""         },
    ['/']         = { scan_operator,  NULL,             ""         },
    ['0']         = { NULL,           NULL,             ""         },
    ['1']         = { NULL,           NULL,             ""         },
    ['2']         = { NULL,           NULL,             ""         },
    ['3']         = { NULL,           NULL,             ""         },
    ['4']         = { NULL,           NULL,             ""         },
    ['5']         = { NULL,           NULL,             ""         },
    ['6']         = { NULL,           NULL,             ""         },
    ['7']         = { NULL,           NULL,             ""         },
    ['8']         = { NULL,           NULL,             ""         },
    ['9']         = { NULL,           NULL,             ""         },
    [':']         = { scan_mod,       NULL,             ""         },
    [';']         = { NULL,           exec_semi,        ":"        },
    ['<']         = { NULL,           exec_lt,          ""         },
    ['=']         = { NULL,           exec_equals,      ":"        },
    ['>']         = { NULL,           exec_gt,          ""         },
    ['?']         = { NULL,           exec_question,    "i"        },
    ['@']         = { scan_mod,       NULL,             ""         },
    ['A']         = { scan_A,         exec_A,           ":"        },
    ['B']         = { scan_B,         NULL,             ""         },
    ['C']         = { NULL,           exec_C,           ":"        },
    ['D']         = { NULL,           exec_D,           ":"        },
    ['E']         = { NULL,           NULL,             ""         },
    ['F']         = { NULL,           NULL,             ""         },
    ['G']         = { NULL,           exec_G,           ":q"       },
    ['H']         = { scan_H,         NULL,             ""         },
    ['I']         = { NULL,           exec_I,           "@ 1"      },
    ['J']         = { NULL,           exec_J,           ":"        },
    ['K']         = { NULL,           exec_K,           ""         },
    ['L']         = { NULL,           exec_L,           ""         },
    ['M']         = { NULL,           exec_M,           ": q"      },
    ['N']         = { NULL,           exec_N,           ": @ 1"    },
    ['O']         = { NULL,           exec_O,           "@ 1"      },
    ['P']         = { NULL,           exec_P,           ": W"      },
    ['Q']         = { scan_Q,         NULL,             ": q"      },
    ['R']         = { NULL,           exec_R,           ":"        },
    ['S']         = { NULL,           exec_S,           ": :: @ 1" },
    ['T']         = { NULL,           exec_T,           ":"        },
    ['U']         = { NULL,           exec_U,           "q"        },
    ['V']         = { NULL,           exec_V,           ""         },
    ['W']         = { scan_W,         exec_W,           ":"        },
    ['X']         = { NULL,           exec_X,           ": q"      },
    ['Y']         = { NULL,           exec_Y,           ":"        },
    ['Z']         = { scan_Z,         NULL,             ""         },
    ['[']         = { NULL,           exec_lbracket,    "q i"      },
    ['\\']        = { scan_back,      exec_back,        ""         },
    [']']         = { NULL,           exec_rbracket,    ": q i"    },
    ['^']         = { NULL,           NULL,             ""         },
    ['_']         = { NULL,           exec_ubar,        ": @ 1"    },
    ['`']         = { scan_bad,       NULL,             ""         },
    ['a']         = { NULL,           NULL,             ""         },
    ['b']         = { NULL,           NULL,             ""         },
    ['c']         = { NULL,           NULL,             ""         },
    ['d']         = { NULL,           NULL,             ""         },
    ['e']         = { NULL,           NULL,             ""         },
    ['f']         = { NULL,           NULL,             ""         },
    ['g']         = { NULL,           NULL,             ""         },
    ['h']         = { NULL,           NULL,             ""         },
    ['i']         = { NULL,           NULL,             ""         },
    ['j']         = { NULL,           NULL,             ""         },
    ['k']         = { NULL,           NULL,             ""         },
    ['l']         = { NULL,           NULL,             ""         },
    ['m']         = { NULL,           NULL,             ""         },
    ['n']         = { NULL,           NULL,             ""         },
    ['o']         = { NULL,           NULL,             ""         },
    ['p']         = { NULL,           NULL,             ""         },
    ['q']         = { NULL,           NULL,             ""         },
    ['r']         = { NULL,           NULL,             ""         },
    ['s']         = { NULL,           NULL,             ""         },
    ['t']         = { NULL,           NULL,             ""         },
    ['u']         = { NULL,           NULL,             ""         },
    ['v']         = { NULL,           NULL,             ""         },
    ['w']         = { NULL,           NULL,             ""         },
    ['x']         = { NULL,           NULL,             ""         },
    ['y']         = { NULL,           NULL,             ""         },
    ['z']         = { NULL,           NULL,             ""         },
    ['{']         = { scan_bad,       NULL,             ""         },
    ['|']         = { NULL,           exec_vbar,        ""         },
    ['}']         = { scan_bad,       NULL,             ""         },
    ['~']         = { scan_bad,       NULL,             ""         },
    [DEL]         = { scan_bad,       NULL,             ""         },
};

const uint cmd_count = countof(cmd_table); ///< No. of items in cmd_table[]


///  @var    cmd_e_table
///  @brief  Table for all commands starting with E.

const struct cmd_table cmd_e_table[] =
{
    { NULL,       exec_E_pct,   ": @ q 1" },
    { NULL,       exec_EA,      ""        },
    { NULL,       exec_EB,      ": @ 1"   },
    { scan_EC,    exec_EC,      ""        },
    { scan_ED,    exec_ED,      ""        },
    { scan_EE,    exec_EE,      ""        },
    { NULL,       exec_EF,      ""        },
    { NULL,       exec_EG,      ": @ 1"   },
    { scan_EH,    exec_EH,      ""        },
    { NULL,       exec_EI,      ": @ 1"   },
    { scan_EJ,    NULL,         ""        },
    { NULL,       exec_EK,      ""        },
    { NULL,       exec_EL,      "@ 1 2"   },
    { NULL,       exec_EM,      ""        },
    { NULL,       exec_EN,      ": @ 1"   },
    { scan_EO,    NULL,         ""        },
    { NULL,       exec_EP,      ""        },
    { NULL,       exec_EQ,      ": @ q 1" },
    { NULL,       exec_ER,      ": @ 1"   },
    { scan_ES,    exec_ES,      ""        },
    { scan_ET,    exec_ET,      ""        },
    { scan_EU,    exec_EU,      ""        },
    { scan_EV,    exec_EV,      ""        },
    { NULL,       exec_EW,      ": @ 1"   },
    { NULL,       exec_EX,      ""        },
    { NULL,       exec_EY,      ":"       },
    { scan_EZ,    exec_EZ,      ""        },
    { NULL,       exec_E_ubar,  "@ 1"     },
};

const uint cmd_e_count = countof(cmd_e_table); ///< No. of items in cmd_e_table[]


///  @var    cmd_f_table
///  @brief  Table for all commands starting with F.

const struct cmd_table cmd_f_table[] =
{
    { NULL,  exec_F_apos,    ""           },
    { NULL,  exec_F_lt,      ""           },
    { NULL,  exec_F_gt,      ""           },
    { NULL,  exec_FB,        "@ 1"        },
    { NULL,  exec_FC,        "@ 1 2"      },
    { NULL,  exec_FD,        "@ 1"        },
    { NULL,  exec_FK,        "@ 1"        },
    { NULL,  exec_FN,        ": @ 1 2"    },
    { NULL,  exec_FR,        "@ 1"        },
    { NULL,  exec_FS,        ": :: @ 1 2" },
    { NULL,  exec_F_ubar,    ": @ 1 2"    },
    { NULL,  exec_F_vbar,    ""           },
};

const uint cmd_f_count = countof(cmd_f_table); ///< No. of items in cmd_f_table[]
