///
///  @file    cmd_tables.c
///  @brief   Data tables used in executing command strings.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
///
///  Permission is hereby granted, free of charge, to any person obtaining a
///  copy of this software and associated documentation files (the "Software"),
///  to deal in the Software without restriction, including without limitation
///  the rights to use, copy, modify, merge, publish, distribute, sublicense,
///  and/or sell copies of the Software, and to permit persons to whom the
///  Software is furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIA-
///  BILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
///  THE SOFTWARE.
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

const struct cmd_table cmd_table[] =
{
    [NUL]         = { NULL,           ""          },
    [CTRL_A]      = { exec_ctrl_a,    ": @ 1"     },
    [CTRL_B]      = { exec_ctrl_b,    "x"         },
    [CTRL_C]      = { exec_ctrl_c,    ""          },
    [CTRL_D]      = { exec_ctrl_d,    ""          },
    [CTRL_E]      = { exec_ctrl_e,    "f n x"     },
    [CTRL_F]      = { exec_ctrl_f,    "f n x"     },
    [CTRL_G]      = { exec_bad,       ""          },
    [CTRL_H]      = { exec_ctrl_h,    "x"         },
    [CTRL_I]      = { exec_ctrl_i,    "1"         },
    [LF]          = { NULL,           ""          },
    [VT]          = { exec_bad,       ""          },
    [FF]          = { NULL,           ""          },
    [CR]          = { NULL,           ""          },
    [CTRL_N]      = { exec_ctrl_n,    "f x"       },
    [CTRL_O]      = { exec_ctrl_o,    ""          },
    [CTRL_P]      = { exec_bad,       ""          },
    [CTRL_Q]      = { exec_ctrl_q,    "x n"       },
    [CTRL_R]      = { exec_ctrl_r,    "x"         },
    [CTRL_S]      = { exec_ctrl_s,    "x"         },
    [CTRL_T]      = { exec_ctrl_t,    "m n :"     },
    [CTRL_U]      = { exec_ctrl_u,    "n : @ q 1" },
    [CTRL_V]      = { exec_ctrl_v,    "n"         },
    [CTRL_W]      = { exec_ctrl_w,    "n"         },
    [CTRL_X]      = { exec_ctrl_x,    "f n x"     },
    [CTRL_Y]      = { exec_ctrl_y,    "x"         },
    [CTRL_Z]      = { exec_ctrl_z,    "x"         },
    [ESC]         = { exec_escape,    "m"         },
    ['\x1C']      = { exec_bad,       ""          },
    ['\x1D']      = { exec_bad,       ""          },
    ['\x1E']      = { NULL,           ""          },
    ['\x1F']      = { exec_operator,  "x"         },
    [SPACE]       = { NULL,           ""          },
    ['!']         = { exec_bang,      "b @ 1"     },
    ['"']         = { exec_quote,     "n"         },
    ['#']         = { exec_operator,  "x"         },
    ['$']         = { exec_bad,       ""          },
    ['%']         = { exec_pct,       "n x q"     },
    ['&']         = { exec_operator,  "x"         },
    ['\'']        = { exec_apos,      ""          },
    ['(']         = { exec_operator,  "x"         },
    [')']         = { exec_operator,  "x"         },
    ['*']         = { exec_operator,  "x"         },
    ['+']         = { exec_operator,  "x"         },
    [',']         = { exec_comma,     "x"         },
    ['-']         = { exec_operator,  "x"         },
    ['.']         = { exec_dot,       "x"         },
    ['/']         = { exec_operator,  "x"         },
    ['0']         = { exec_digit,     "x"         },
    ['1']         = { exec_digit,     "x"         },
    ['2']         = { exec_digit,     "x"         },
    ['3']         = { exec_digit,     "x"         },
    ['4']         = { exec_digit,     "x"         },
    ['5']         = { exec_digit,     "x"         },
    ['6']         = { exec_digit,     "x"         },
    ['7']         = { exec_digit,     "x"         },
    ['8']         = { exec_digit,     "x"         },
    ['9']         = { exec_digit,     "x"         },
    [':']         = { exec_mod,       "a"         },
    [';']         = { exec_semi,      "n :"       },
    ['<']         = { exec_lt,        "b n"       },
    ['=']         = { exec_equals,    "b n : @ 1" },
    ['>']         = { exec_gt,        "b "        },
    ['?']         = { exec_question,  ":"         },
    ['@']         = { exec_mod,       "a"         },
    ['A']         = { exec_A,         "x n :"     },
    ['B']         = { exec_B,         "x"         },
    ['C']         = { exec_C,         "n :"       },
    ['D']         = { exec_D,         "m :"       },
    ['E']         = { NULL,           ""          },
    ['F']         = { NULL,           ""          },
    ['G']         = { exec_G,         ": q"       },
    ['H']         = { exec_H,         "x"         },
    ['I']         = { exec_I,         "n @ 1"     },
    ['J']         = { exec_J,         "n :"       },
    ['K']         = { exec_K,         "m"         },
    ['L']         = { exec_L,         "x n :"     },
    ['M']         = { exec_M,         "m : q"     },
    ['N']         = { exec_N,         "n : @ 1"   },
    ['O']         = { exec_O,         "n @ 1"     },
    ['P']         = { exec_P,         ": W"       },
    ['Q']         = { exec_Q,         "n x : q"   },
    ['R']         = { exec_R,         "n :"       },
    ['S']         = { exec_S,         "m :: @ 1"  },
    ['T']         = { exec_T,         "m :"       },
    ['U']         = { exec_U,         "m q"       },
    ['V']         = { exec_V,         "m"         },
    ['W']         = { exec_W,         "m x :"     },
    ['X']         = { exec_X,         "m : q"     },
    ['Y']         = { exec_Y,         ":"         },
    ['Z']         = { exec_Z,         "x"         },
    ['[']         = { exec_lbracket,  "m q"       },
    ['\\']        = { exec_back,      "n x"       },
    [']']         = { exec_rbracket,  "m : q"     },
    ['^']         = { NULL,           ""          },
    ['_']         = { exec_ubar,      "n : @ 1"   },
    ['`']         = { exec_bad,       ""          },
    ['a']         = { NULL,           ""          },
    ['b']         = { NULL,           ""          },
    ['c']         = { NULL,           ""          },
    ['d']         = { NULL,           ""          },
    ['e']         = { NULL,           ""          },
    ['f']         = { NULL,           ""          },
    ['g']         = { NULL,           ""          },
    ['h']         = { NULL,           ""          },
    ['i']         = { NULL,           ""          },
    ['j']         = { NULL,           ""          },
    ['k']         = { NULL,           ""          },
    ['l']         = { NULL,           ""          },
    ['m']         = { NULL,           ""          },
    ['n']         = { NULL,           ""          },
    ['o']         = { NULL,           ""          },
    ['p']         = { NULL,           ""          },
    ['q']         = { NULL,           ""          },
    ['r']         = { NULL,           ""          },
    ['s']         = { NULL,           ""          },
    ['t']         = { NULL,           ""          },
    ['u']         = { NULL,           ""          },
    ['v']         = { NULL,           ""          },
    ['w']         = { NULL,           ""          },
    ['x']         = { NULL,           ""          },
    ['y']         = { NULL,           ""          },
    ['z']         = { NULL,           ""          },
    ['{']         = { exec_brace,     "x"         },
    ['|']         = { exec_vbar,      ""          },
    ['}']         = { exec_bad,       "b x"       },
    ['~']         = { exec_bad,       "b x"       },
    [DEL]         = { exec_bad,       ""          },
};

const uint cmd_count = countof(cmd_table); ///< No. of items in cmd_table[]


///  @var    cmd_e_table
///  @brief  Table for all commands starting with E.

const struct cmd_table cmd_e_table[] =
{
    { exec_E1,      "f m x"    },
    { exec_E2,      "f m x"    },
    { exec_E3,      "f m x"    },
    { exec_E4,      "@ 1"      },
    { exec_E5,      "n"        },
    { exec_E6,      "n @ 2"    },
    { NULL,         "n x"      },       // Placeholder for E7
    { NULL,         "n x"      },       // Placeholder for E8
    { NULL,         "n x"      },       // Placeholder for E9
    { exec_EA,      ""         },
    { exec_EB,      ": @ 1"    },
    { exec_EC,      "n"        },
    { exec_ED,      "f m x"    },
    { exec_EE,      "f n x"    },
    { exec_EF,      ""         },
    { exec_EG,      ": :: @ 1" },
    { exec_EH,      "f m x"    },
    { exec_EI,      ": @ 1"    },
    { exec_EJ,      ": f m x"  },
    { exec_EK,      ""         },
    { exec_EL,      "@ 1"      },
    { exec_EM,      ""         },
    { exec_EN,      ": @ 1"    },
    { exec_EO,      "f n x"    },
    { exec_EP,      ""         },
    { exec_EQ,      ": @ q 1"  },
    { exec_ER,      ": @ 1"    },
    { exec_ES,      "f n x"    },
    { exec_ET,      "f m x"    },
    { exec_EU,      "f n x"    },
    { exec_EV,      "f n x"    },
    { exec_EW,      ": @ 1"    },
    { exec_EX,      ""         },
    { exec_EY,      ":"        },
    { exec_EZ,      ": @ q 1"  },
    { exec_E_ubar,  "n : @ 1"  },
};

const uint cmd_e_count = countof(cmd_e_table); ///< No. of items in cmd_e_table[]


///  @var    cmd_f_table
///  @brief  Table for all commands starting with F.

const struct cmd_table cmd_f_table[] =
{
    { exec_F_apos,  ""           },
    { exec_F_lt,    ""           },
    { exec_F_gt,    ""           },
    { exec_FB,      "m : @ 1"    },
    { exec_FC,      "m : @ 2"    },
    { exec_FD,      "n : @ 1"    },
    { exec_FK,      "n : @ 1"    },
    { exec_FL,      "m"          },
    { exec_FN,      "m : @ 2"    },
    { exec_FR,      "m : @ 1"    },
    { exec_FS,      "m : :: @ 2" },
    { exec_FU,      "m"          },
    { exec_F_ubar,  "m : @ 2"    },
    { exec_F_vbar,  ""           },
};

const uint cmd_f_count = countof(cmd_f_table); ///< No. of items in cmd_f_table[]
