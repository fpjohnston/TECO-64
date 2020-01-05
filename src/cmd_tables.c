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
///  - conditionally take a value: e.g., A, <, ;
///  - implicitly take a value: C, %
///  - always take a value: e.g., =
///  - return a value: e.g., B, Z, H
///  - take & return a value: e.g., %
///  - take or return a value: e.g., ET
///  - consume a value: e.g., >, |, '
///  - pass through a value: e.g., [, ]

const struct cmd_table cmd_table[] =
{
    [NUL]         = { NULL,           NULL,             ""             },
    [CTRL_A]      = { NULL,           exec_ctrl_a,      "@ 1"          }, // consume value
    [CTRL_B]      = { scan_ctrl_b,    NULL,             "x"            }, // return value
    [CTRL_C]      = { NULL,           exec_ctrl_c,      ""             }, // consume value
    [CTRL_D]      = { NULL,           exec_ctrl_d,      ""             }, // disallow value
    [CTRL_E]      = { scan_ctrl_e,    NULL,             "n x"          },
    [CTRL_F]      = { scan_ctrl_f,    NULL,             "n x"          },
    [CTRL_G]      = { scan_bad,       NULL,             ""             },
    [CTRL_H]      = { scan_ctrl_h,    NULL,             "x"            }, // return value
    [CTRL_I]      = { NULL,           exec_ctrl_i,      "1"            }, // disallow value
    [LF]          = { NULL,           NULL,             ""             }, // ignore value
    [VT]          = { scan_bad,       NULL,             ""             },
    [FF]          = { NULL,           NULL,             ""             }, // ignore value
    [CR]          = { NULL,           NULL,             ""             }, // ignore value
    [CTRL_N]      = { scan_ctrl_n,    NULL,             "x"            }, // return value
    [CTRL_O]      = { NULL,           exec_ctrl_o,      ""             }, // consume value
    [CTRL_P]      = { scan_bad,       NULL,             ""             },
    [CTRL_Q]      = { scan_ctrl_q,    NULL,             "n x"          }, // take & return value
    [CTRL_R]      = { scan_ctrl_r,    NULL,             "x"            }, // take or return value
    [CTRL_S]      = { scan_ctrl_s,    NULL,             "x"            }, // return value
    [CTRL_T]      = { scan_ctrl_t,    NULL,             "n x :"        }, // allow & return value
    [CTRL_U]      = { NULL,           exec_ctrl_u,      "n : @ q 1"    }, // allow value
    [CTRL_V]      = { NULL,           exec_ctrl_v,      ""             },
    [CTRL_W]      = { NULL,           exec_ctrl_w,      ""             },
    [CTRL_X]      = { scan_ctrl_x,    NULL,             "x n"          }, // take or return value
    [CTRL_Y]      = { scan_ctrl_y,    NULL,             "x"            }, // return value
    [CTRL_Z]      = { scan_ctrl_z,    NULL,             "x"            }, // return value
    [ESC]         = { NULL,           exec_escape,      ""             }, // consume value
    ['\x1C']      = { scan_bad,       NULL,             ""             },
    ['\x1D']      = { scan_bad,       NULL,             ""             },
    ['\x1E']      = { NULL,           NULL,             ""             }, // return value
    ['\x1F']      = { scan_operator,  NULL,             "x"            }, // expression command
    [SPACE]       = { NULL,           NULL,             ""             }, // ignore value
    ['!']         = { NULL,           exec_bang,        "@ 1"          }, // prohibited value?
    ['"']         = { NULL,           exec_quote,       "n"            }, // required value
    ['#']         = { scan_operator,  NULL,             "x"            }, // expression command
    ['$']         = { scan_bad,       NULL,             ""             },
    ['%']         = { scan_pct,       NULL,             "n x q"        }, // allow & return value
    ['&']         = { scan_operator,  NULL,             "x"            }, // expression command
    ['\'']        = { NULL,           exec_apos,        ""             }, // consume value
    ['(']         = { scan_operator,  NULL,             "x"            }, // expression command
    [')']         = { scan_operator,  NULL,             "x"            }, // expression command
    ['*']         = { scan_operator,  NULL,             "x"            }, // expression command
    ['+']         = { scan_operator,  NULL,             "x"            }, // expression command
    [',']         = { scan_comma,     NULL,             "x"            }, // expression command
    ['-']         = { scan_operator,  NULL,             "x"            }, // expression command
    ['.']         = { scan_dot,       NULL,             "x"            }, // return value
    ['/']         = { scan_operator,  NULL,             "x"            }, // expression command
    ['0']         = { scan_digit,     NULL,             "x"            }, // return value
    ['1']         = { scan_digit,     NULL,             "x"            }, // return value
    ['2']         = { scan_digit,     NULL,             "x"            }, // return value
    ['3']         = { scan_digit,     NULL,             "x"            }, // return value
    ['4']         = { scan_digit,     NULL,             "x"            }, // return value
    ['5']         = { scan_digit,     NULL,             "x"            }, // return value
    ['6']         = { scan_digit,     NULL,             "x"            }, // return value
    ['7']         = { scan_digit,     NULL,             "x"            }, // return value
    ['8']         = { scan_digit,     NULL,             "x"            }, // return value
    ['9']         = { scan_digit,     NULL,             "x"            }, // return value
    [':']         = { scan_mod,       NULL,             ""             }, // command modifier
    [';']         = { NULL,           exec_semi,        "n :"          }, // allow value
    ['<']         = { NULL,           exec_lt,          "n"            }, // allow value
    ['=']         = { NULL,           exec_equals,      "n :"          }, // require value
    ['>']         = { NULL,           exec_gt,          ""             }, // consume value
    ['?']         = { NULL,           exec_question,    "i"            }, // consume value
    ['@']         = { scan_mod,       NULL,             ""             }, // command modifier
    ['A']         = { scan_A,         NULL,             "n x :"        }, // allow value
    ['B']         = { scan_B,         NULL,             "x"            }, // return value
    ['C']         = { NULL,           exec_C,           "n :"          }, // allow value
    ['D']         = { NULL,           exec_D,           "m,n :"        }, // allow value
    ['E']         = { NULL,           NULL,             "m,n"          }, // (see below)
    ['F']         = { NULL,           NULL,             ""             }, // (see below)
    ['G']         = { NULL,           exec_G,           ": q"          }, // consume value
    ['H']         = { scan_H,         NULL,             "x"            }, // return value(s)
    ['I']         = { NULL,           exec_I,           "n @ 1"        }, // allow value
    ['J']         = { NULL,           exec_J,           "n :"          }, // allow value
    ['K']         = { NULL,           exec_K,           "m,n"          }, // allow value
    ['L']         = { NULL,           exec_L,           "n"            }, // allow value
    ['M']         = { NULL,           exec_M,           "m,n : q"      }, // allow & return value(s)
    ['N']         = { NULL,           exec_N,           "n : @ 1"      }, // allow & return value
    ['O']         = { NULL,           exec_O,           "n @ 1"        }, // allow value
    ['P']         = { NULL,           exec_P,           ": W"          }, // allow value(s)
    ['Q']         = { scan_Q,         NULL,             "n x : q"      }, // allow value
    ['R']         = { NULL,           exec_R,           "n :"          }, // allow value
    ['S']         = { NULL,           exec_S,           "m,n : :: @ 1" }, // allow & return value
    ['T']         = { NULL,           exec_T,           "m,n :"        }, // allow value
    ['U']         = { NULL,           exec_U,           "m,n q"        }, // allow value
    ['V']         = { NULL,           exec_V,           "m,n"          }, // allow value
    ['W']         = { scan_W,         NULL,             "m,n x :"      }, // allow value(s)
    ['X']         = { NULL,           exec_X,           "m,n : q"      }, // allow value(s)
    ['Y']         = { NULL,           exec_Y,           ":"            }, // allow value
    ['Z']         = { scan_Z,         NULL,             "x"            }, // return value
    ['[']         = { NULL,           exec_lbracket,    "q"            }, // pass through value
    ['\\']        = { scan_back,      NULL,             "n x"          }, // allow value
    [']']         = { NULL,           exec_rbracket,    ": q"          }, // pass through value
    ['^']         = { NULL,           NULL,             ""             }, // return value
    ['_']         = { NULL,           exec_ubar,        "n : @ 1"      }, // allow value
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
    ['{']         = { scan_brace,     NULL,             ""             },
    ['|']         = { NULL,           exec_vbar,        ""             }, // consume value
    ['}']         = { scan_bad,       NULL,             ""             },
    ['~']         = { scan_tilde,     NULL,             ""             },
    [DEL]         = { scan_bad,       NULL,             ""             },
};

const uint cmd_count = countof(cmd_table); ///< No. of items in cmd_table[]


///  @var    cmd_e_table
///  @brief  Table for all commands starting with E.

const struct cmd_table cmd_e_table[] =
{
    { NULL,       exec_E_pct,   ": @ q 1" },
    { NULL,       NULL,         ""        }, // allow or return value
    { scan_E1,    NULL,         "n x"     }, // allow or return value
    { NULL,       NULL,         "n x"     }, // allow or return value
    { NULL,       NULL,         "n x"     }, // allow or return value
    { NULL,       NULL,         "n x"     }, // allow or return value
    { NULL,       NULL,         "n x"     }, // allow or return value
    { NULL,       NULL,         "n x"     }, // allow or return value
    { NULL,       NULL,         "n x"     }, // allow or return value
    { NULL,       NULL,         "n x"     }, // allow or return value
    { NULL,       NULL,         "n x"     }, // allow or return value
    { NULL,       exec_EA,      ""        }, // consume value
    { NULL,       exec_EB,      ": @ 1"   }, // consume & return value
    { NULL,       exec_EC,      "n"       }, // allow value
    { scan_ED,    NULL,         "m,n x"   }, // allow or return value
    { scan_EE,    NULL,         "n x"     }, // allow or return value
    { NULL,       exec_EF,      ""        }, // consume value
    { NULL,       exec_EG,      ": @ 1"   }, // consume value
    { scan_EH,    NULL,         "m,n x"   }, // allow or return value
    { NULL,       exec_EI,      ": @ 1"   }, // consume value
    { scan_EJ,    NULL,         "m,n x"   }, // allow & return value
    { NULL,       exec_EK,      ""        }, // consume value
    { NULL,       exec_EL,      "@ 2"     }, // consume value
    { NULL,       exec_EM,      "n"       }, // consume value
    { NULL,       exec_EN,      ": @ 1"   }, // consume value
    { scan_EO,    NULL,         "n x"     }, // allow & return value
    { NULL,       exec_EP,      ""        }, // consume value
    { NULL,       exec_EQ,      ": @ q 1" }, // consume value
    { NULL,       exec_ER,      ": @ 1"   }, // consume & return value
    { scan_ES,    NULL,         "n x"     },
    { scan_ET,    NULL,         "m,n x"   }, // allow or return value
    { scan_EU,    NULL,         "n x"     }, // allow or return value
    { scan_EV,    NULL,         "n x"     }, // allow or return value
    { NULL,       exec_EW,      ": @ 1"   }, // consume & return value
    { NULL,       exec_EX,      ""        }, // consume value
    { NULL,       exec_EY,      ":"       }, // disallow value
    { scan_EZ,    NULL,         "x"       },
    { NULL,       exec_E_ubar,  "n @ 1"   },
};

const uint cmd_e_count = countof(cmd_e_table); ///< No. of items in cmd_e_table[]


///  @var    cmd_f_table
///  @brief  Table for all commands starting with F.

const struct cmd_table cmd_f_table[] =
{
    { NULL,  exec_F_apos,    ""           }, // consume value
    { NULL,  exec_F_lt,      ""           }, // allow value
    { NULL,  exec_F_gt,      ""           }, // consume value
    { NULL,  exec_FB,        "m,n @ 1"    },
    { NULL,  exec_FC,        "m,n @ 2"    },
    { NULL,  exec_FD,        "n @ 1"      },
    { NULL,  exec_FK,        "n @ 1"      },
    { NULL,  exec_FN,        "n : @ 2"    },
    { NULL,  exec_FR,        "m,n @ 1"    },
    { NULL,  exec_FS,        "n : :: @ 2" },
    { NULL,  exec_F_ubar,    "n : @ 2"    },
    { NULL,  exec_F_vbar,    ""           }, // consume value
};

const uint cmd_f_count = countof(cmd_f_table); ///< No. of items in cmd_f_table[]
