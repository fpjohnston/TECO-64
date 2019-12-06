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

struct cmd_table cmd_table[] =
{
    [NUL]         = { NULL,           NULL,             ""             },
    [CTRL_A]      = { NULL,           exec_ctrl_a,      "@ 1"          },
    [CTRL_B]      = { scan_ctrl_b,    NULL,             ""             },
    [CTRL_C]      = { NULL,           exec_ctrl_c,      ""             },
    [CTRL_D]      = { NULL,           exec_ctrl_d,      ""             },
    [CTRL_E]      = { scan_ctrl_e,    exec_ctrl_e,      ""             },
    [CTRL_F]      = { scan_ctrl_f,    NULL,             "n"            },
    [CTRL_G]      = { scan_bad,       NULL,             ""             },
    [CTRL_H]      = { scan_ctrl_h,    NULL,             ""             },
    [CTRL_I]      = { NULL,           exec_ctrl_i,      "1"            },
    [LF]          = { NULL,           NULL,             ""             },
    [VT]          = { scan_bad,       NULL,             ""             },
    [FF]          = { NULL,           NULL,             ""             },
    [CR]          = { NULL,           NULL,             ""             },
    [CTRL_N]      = { scan_ctrl_n,    NULL,             ""             },
    [CTRL_O]      = { NULL,           exec_ctrl_o,      ""             },
    [CTRL_P]      = { scan_bad,       NULL,             ""             },
    [CTRL_Q]      = { NULL,           exec_ctrl_q,      ""             },
    [CTRL_R]      = { scan_ctrl_r,    exec_ctrl_r,      "n"            },
    [CTRL_S]      = { scan_ctrl_s,    NULL,             ""             },
    [CTRL_T]      = { scan_ctrl_t,    exec_ctrl_t,      "n :"          },
    [CTRL_U]      = { NULL,           exec_ctrl_u,      "n : @ q 1"    },
    [CTRL_V]      = { NULL,           exec_ctrl_v,      ""             },
    [CTRL_W]      = { NULL,           exec_ctrl_w,      ""             },
    [CTRL_X]      = { scan_ctrl_x,    exec_ctrl_x,      "n"            },
    [CTRL_Y]      = { scan_ctrl_y,    NULL,             ""             },
    [CTRL_Z]      = { scan_ctrl_z,    NULL,             ""             },
    [ESC]         = { NULL,           exec_escape,      "m n"          },
    ['\x1C']      = { scan_bad,       NULL,             ""             },
    ['\x1D']      = { scan_bad,       NULL,             ""             },
    ['\x1E']      = { NULL,           NULL,             ""             },
    ['\x1F']      = { scan_operator,  NULL,             ""             },
    [SPACE]       = { NULL,           NULL,             ""             },
    ['!']         = { NULL,           exec_bang,        "@ 1"          },
    ['"']         = { scan_quote,     exec_quote,       "n"            },
    ['#']         = { scan_operator,  NULL,             ""             },
    ['$']         = { NULL,           exec_escape,      "m n"          },
    ['%']         = { scan_pct,       NULL,             "n q"          },
    ['&']         = { scan_operator,  NULL,             ""             },
    ['\'']        = { NULL,           exec_apos,        ""             },
    ['(']         = { scan_operator,  NULL,             ""             },
    [')']         = { scan_operator,  NULL,             ""             },
    ['*']         = { scan_operator,  NULL,             ""             },
    ['+']         = { scan_operator,  NULL,             ""             },
    [',']         = { scan_comma,     NULL,             ""             },
    ['-']         = { scan_operator,  NULL,             ""             },
    ['.']         = { scan_dot,       NULL,             ""             },
    ['/']         = { scan_operator,  NULL,             ""             },
    ['0']         = { scan_digits,    NULL,             ""             },
    ['1']         = { scan_digits,    NULL,             ""             },
    ['2']         = { scan_digits,    NULL,             ""             },
    ['3']         = { scan_digits,    NULL,             ""             },
    ['4']         = { scan_digits,    NULL,             ""             },
    ['5']         = { scan_digits,    NULL,             ""             },
    ['6']         = { scan_digits,    NULL,             ""             },
    ['7']         = { scan_digits,    NULL,             ""             },
    ['8']         = { scan_digits,    NULL,             ""             },
    ['9']         = { scan_digits,    NULL,             ""             },
    [':']         = { scan_mod,       NULL,             ""             },
    [';']         = { NULL,           exec_semi,        "n :"          },
    ['<']         = { NULL,           exec_lt,          "n"            },
    ['=']         = { NULL,           exec_equals,      "n :"          },
    ['>']         = { NULL,           exec_gt,          ""             },
    ['?']         = { NULL,           exec_question,    ""             },
    ['@']         = { scan_mod,       NULL,             ""             },
    ['A']         = { scan_A,         exec_A,           "n :"          },
    ['B']         = { scan_B,         NULL,             ""             },
    ['C']         = { NULL,           exec_C,           "n :"          },
    ['D']         = { NULL,           exec_D,           "m n :"        },
    ['E']         = { NULL,           NULL,             ""             },
    ['F']         = { NULL,           NULL,             ""             },
    ['G']         = { NULL,           exec_G,           ":q"           },
    ['H']         = { scan_H,         NULL,             ""             },
    ['I']         = { NULL,           exec_I,           "n @ 1"        },
    ['J']         = { NULL,           exec_J,           "n :"          },
    ['K']         = { NULL,           exec_K,           "m n"          },
    ['L']         = { NULL,           exec_L,           "n"            },
    ['M']         = { NULL,           exec_M,           "m n : q"      },
    ['N']         = { NULL,           exec_N,           "n : @ 1"      },
    ['O']         = { NULL,           exec_O,           "n @ 1"        },
    ['P']         = { NULL,           exec_P,           "m n H : W"    },
    ['Q']         = { scan_Q,         NULL,             "n : q"        },
    ['R']         = { NULL,           exec_R,           "n :"          },
    ['S']         = { NULL,           exec_S,           "m n : :: @ 1" },
    ['T']         = { NULL,           exec_T,           "m n :"        },
    ['U']         = { NULL,           exec_U,           "m n q"        },
    ['V']         = { NULL,           exec_V,           "m n"          },
    ['W']         = { scan_W,         exec_W,           "m n :"        },
    ['X']         = { NULL,           exec_X,           "m n : q"      },
    ['Y']         = { NULL,           exec_Y,           "n :"          },
    ['Z']         = { scan_Z,         NULL,             ""             },
    ['[']         = { NULL,           exec_lbracket,    "q"            },
    ['\\']        = { scan_back,      exec_back,        "n"            },
    [']']         = { NULL,           exec_rbracket,    ": q"          },
    ['^']         = { NULL,           NULL,             ""             },
    ['_']         = { NULL,           exec_ubar,        "n : @ 1"      },
    ['`']         = { scan_bad,       NULL,             ""             },
    ['a']         = { NULL,           NULL,             ""             },
    ['b']         = { NULL,           NULL,             ""             },
    ['c']         = { NULL,           NULL,             ""             },
    ['d']         = { NULL,           NULL,             ""             },
    ['e']         = { NULL,           NULL,             ""             },
    ['f']         = { NULL,           NULL,             ""             },
    ['g']         = { NULL,           NULL,             ""             },
    ['h']         = { NULL,           NULL,             ""             },
    ['i']         = { NULL,           NULL,             ""             },
    ['j']         = { NULL,           NULL,             ""             },
    ['k']         = { NULL,           NULL,             ""             },
    ['l']         = { NULL,           NULL,             ""             },
    ['m']         = { NULL,           NULL,             ""             },
    ['n']         = { NULL,           NULL,             ""             },
    ['o']         = { NULL,           NULL,             ""             },
    ['p']         = { NULL,           NULL,             ""             },
    ['q']         = { NULL,           NULL,             ""             },
    ['r']         = { NULL,           NULL,             ""             },
    ['s']         = { NULL,           NULL,             ""             },
    ['t']         = { NULL,           NULL,             ""             },
    ['u']         = { NULL,           NULL,             ""             },
    ['v']         = { NULL,           NULL,             ""             },
    ['w']         = { NULL,           NULL,             ""             },
    ['x']         = { NULL,           NULL,             ""             },
    ['y']         = { NULL,           NULL,             ""             },
    ['z']         = { NULL,           NULL,             ""             },
    ['{']         = { scan_bad,       NULL,             ""             },
    ['|']         = { NULL,           exec_vbar,        ""             },
    ['}']         = { scan_bad,       NULL,             ""             },
    ['~']         = { scan_bad,       NULL,             ""             },
    [DEL]         = { scan_bad,       NULL,             ""             },
};

uint cmd_count = countof(cmd_table);    ///< No. of items in cmd_table[]


///  @var    cmd_e_table
///  @brief  Table for all commands starting with E.

struct cmd_table cmd_e_table[] =
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

uint cmd_e_count = countof(cmd_e_table); ///< No. of items in cmd_e_table[]


///  @var    cmd_f_table
///  @brief  Table for all commands starting with F.

struct cmd_table cmd_f_table[] =
{
    { NULL,  exec_FB,        "m n @ 1"      },
    { NULL,  exec_FC,        "m n @ 1 2"    },
    { NULL,  exec_FD,        "m n @ 1"      },
    { NULL,  exec_FK,        "n @ 1"        },
    { NULL,  exec_FN,        "n : @ 1 2"    },
    { NULL,  exec_FR,        "m n @ 1 2"    },
    { NULL,  exec_FS,        "m n :: @ 1 2" },
    { NULL,  exec_F_lt,      ""             },
    { NULL,  exec_F_gt,      ""             },
    { NULL,  exec_F_apos,    ""             },
    { NULL,  exec_F_ubar,    "n @ 1 2"      },
    { NULL,  exec_F_vbar,    ""             },
};

uint cmd_f_count = countof(cmd_f_table); ///< No. of items in cmd_f_table[]


///  @var    null_cmd
///  @brief  Initial command block values.

struct cmd null_cmd =
{
    .level      = 0,
    .flag       = false,
    .c1         = NUL,
    .c2         = NUL,
    .c3         = NUL,
    .m_arg      = 0,
    .n_arg      = 0,
    .paren      = 0,
    .delim      = ESC,
    .qreg       = NUL,
    .qlocal     = false,
    .expr       = { .buf = NULL, .len = 0 },
    .text1      = { .buf = NULL, .len = 0 },
    .text2      = { .buf = NULL, .len = 0 },
    .next       = NULL,
    .prev       = NULL,
};
