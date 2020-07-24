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

static const union cmd_opts no_opts     = { .bits=0 };

//lint -save -e708 -e785

static const union cmd_opts acnqx1_opts = {{ .a=1, .c=1, .n=1, .q=1, .t1=1, .x=1 }};

static const union cmd_opts acnx1_opts  = {{ .a=1, .c=1, .n=1, .t1=1, .x=1 }};

static const union cmd_opts acq1_opts   = {{ .a=1, .c=1, .q=1, .t1=1 }};

static const union cmd_opts acx1_opts   = {{ .a=1, .c=1, .t1=1, .x=1 }};

static const union cmd_opts admv1_opts  = {{ .a=1, .d=1, .m=1, .t1=1, .v=1 }};

static const union cmd_opts admv2_opts  = {{ .a=1, .d=1, .m=1, .t2=1, .v=1 }};

static const union cmd_opts ad1_opts    = {{ .a=1, .d=1, .t1=1 }};

static const union cmd_opts amv1_opts   = {{ .a=1, .m=1, .t1=1, .v=1 }};

static const union cmd_opts amv2_opts   = {{ .a=1, .m=1, .t2=1, .v=1 }};

static const union cmd_opts anx1_opts   = {{ .a=1, .n=1, .t1=1, .x=1 }};

static const union cmd_opts anv1_opts   = {{ .a=1, .n=1, .t1=1, .v=1 }};

static const union cmd_opts aqv1_opts   = {{ .a=1, .q=1, .t1=1, .v=1 }};

static const union cmd_opts a1_opts     = {{ .a=1, .t1=1 }};

static const union cmd_opts av1_opts    = {{ .a=1, .t1=1, .v=1 }};

static const union cmd_opts ax1_opts    = {{ .a=1, .t1=1, .x=1 }};

static const union cmd_opts cfm_opts    = {{ .m=1, .f=1, .c=1}};

static const union cmd_opts cm_opts     = {{ .c=1, .m=1 }};

static const union cmd_opts cmq_opts    = {{ .c=1, .m=1, .q=1 }};

static const union cmd_opts cmqx_opts   = {{ .c=1, .m=1, .q=1, .x=1 }};

static const union cmd_opts cmx_opts    = {{ .c=1, .m=1, .x=1 }};

static const union cmd_opts cnq_opts    = {{ .c=1, .n=1, .q=1 }};

static const union cmd_opts cnx_opts    = {{ .c=1, .n=1, .x=1 }};

static const union cmd_opts cqx_opts    = {{ .c=1, .q=1, .x=1 }};

static const union cmd_opts cx_opts     = {{ .c=1, .x=1 }};

static const union cmd_opts f_opts      = {{ .f=1 }};

static const union cmd_opts mq_opts     = {{ .m=1, .q=1 }};

static const union cmd_opts mqv_opts    = {{ .m=1, .q=1, .v=1 }};

static const union cmd_opts mv_opts     = {{ .m=1, .v=1 }};

static const union cmd_opts mvw_opts    = {{ .m=1, .v=1, .w=1 }};

static const union cmd_opts mx_opts     = {{ .m=1, .x=1 }};

static const union cmd_opts n_opts      = {{ .n=1 }};

static const union cmd_opts n2_opts     = {{ .n=1, .t2=1 }};

static const union cmd_opts nv_opts     = {{ .n=1, .v=1 }};

static const union cmd_opts nx_opts     = {{ .n=1, .x=1 }};

static const union cmd_opts v_opts      = {{ .v=1 }};

static const union cmd_opts x_opts      = {{ .x=1 }};

static const union cmd_opts x1_opts     = {{ .t1=1, .x=1 }};

//lint -restore

///
///  @var    cmd_table
///
///  @brief  Dispatch table, defining functions to scan execute input
///          characters, as well as the options for each command.
///
///          Note that to avoid duplication, lower-case letters can be omitted,
///          because we translate characters to upper-case before indexing into
///          this table. Also, we handle E, F, and ^ commands specially, as they
///          include a secondary character.
///

//  Command characters may be categorized as one of the following:
//
//  1. Characters that are ignored (e.g., space).
//  2. Characters that are part of an expression (e.g., digits or operators).
//  3. 

const struct cmd_table cmd_table[] =
{
    [NUL]    = { exec_space,     &no_opts      },
    [CTRL_A] = { exec_ctrl_a,    &acx1_opts    },
    [CTRL_B] = { exec_ctrl_b,    &no_opts      },
    [CTRL_C] = { exec_ctrl_c,    &x_opts       },
    [CTRL_D] = { exec_ctrl_d,    &no_opts      },
    [CTRL_E] = { exec_ctrl_e,    &f_opts       },
    [CTRL_F] = { exec_bad,       &no_opts      },
    [CTRL_G] = { exec_bad,       &no_opts      },
    [CTRL_H] = { exec_ctrl_h,    &no_opts      },
    [CTRL_I] = { exec_ctrl_i,    &x1_opts      },
    [LF]     = { exec_space,     &no_opts      },
    [VT]     = { exec_bad,       &no_opts      },
    [FF]     = { exec_space,     &no_opts      },
    [CR]     = { exec_space,     &no_opts      },
    [CTRL_N] = { exec_ctrl_n,    &n_opts       },
    [CTRL_O] = { exec_ctrl_o,    &no_opts      },
    [CTRL_P] = { exec_bad,       &no_opts      },
    [CTRL_Q] = { exec_ctrl_q,    &n_opts       },
    [CTRL_R] = { exec_ctrl_r,    &f_opts       },
    [CTRL_S] = { exec_ctrl_s,    &no_opts      },
    [CTRL_T] = { exec_ctrl_t,    &cfm_opts     },
    [CTRL_U] = { exec_ctrl_u,    &acnqx1_opts  },
    [CTRL_V] = { exec_ctrl_v,    &nx_opts      },
    [CTRL_W] = { exec_ctrl_w,    &nx_opts      },
    [CTRL_X] = { exec_ctrl_x,    &f_opts       },
    [CTRL_Y] = { exec_ctrl_y,    &no_opts      },
    [CTRL_Z] = { exec_ctrl_z,    &no_opts      },
    [ESC]    = { exec_escape,    &mx_opts      },
    ['\x1C'] = { exec_bad,       &no_opts      },
    ['\x1D'] = { exec_bad,       &no_opts      },
    ['\x1E'] = { exec_ctl_up,    &no_opts      },
    ['\x1F'] = { exec_ctl_ubar,  &no_opts      },
    [SPACE]  = { exec_space,     &no_opts      },
    ['!']    = { exec_bang,      &ax1_opts     },
    ['"']    = { exec_quote,     &nx_opts      },
    ['#']    = { exec_oper,      &no_opts      },
    ['$']    = { exec_bad,       &no_opts      },
    ['%']    = { exec_pct,       &cnq_opts     },
    ['&']    = { exec_oper,      &no_opts      },
    ['\'']   = { exec_apos,      &x_opts       },
    ['(']    = { exec_lparen,    &no_opts      },
    [')']    = { exec_rparen,    &no_opts      },
    ['*']    = { exec_oper,      &no_opts      },
    ['+']    = { exec_oper,      &no_opts      },
    [',']    = { exec_comma,     &n_opts       },
    ['-']    = { exec_oper,      &no_opts      },
    ['.']    = { exec_dot,       &no_opts      },
    ['/']    = { exec_oper,      &no_opts      },
    ['0']    = { exec_digit,     &no_opts      },
    ['1']    = { exec_digit,     &no_opts      },
    ['2']    = { exec_digit,     &no_opts      },
    ['3']    = { exec_digit,     &no_opts      },
    ['4']    = { exec_digit,     &no_opts      },
    ['5']    = { exec_digit,     &no_opts      },
    ['6']    = { exec_digit,     &no_opts      },
    ['7']    = { exec_digit,     &no_opts      },
    ['8']    = { exec_digit,     &no_opts      },
    ['9']    = { exec_digit,     &no_opts      },
    [':']    = { exec_colon,     &no_opts      },
    [';']    = { exec_semi,      &cnx_opts     },
    ['<']    = { exec_lt,        &nx_opts      },
    ['=']    = { exec_equals,    &acnx1_opts   },
    ['>']    = { exec_gt,        &x_opts       },
    ['?']    = { exec_trace,     &cx_opts      },
    ['@']    = { exec_atsign,    &no_opts      },
    ['A']    = { exec_A,         &nv_opts      },
    ['B']    = { exec_B,         &no_opts      },
    ['C']    = { exec_C,         &nv_opts      },
    ['D']    = { exec_D,         &mv_opts      },
    ['E']    = { NULL,           &no_opts      },
    ['F']    = { NULL,           &no_opts      },
    ['G']    = { exec_G,         &cqx_opts     },
    ['H']    = { exec_H,         &no_opts      },
    ['I']    = { exec_I,         &anx1_opts    },
    ['J']    = { exec_J,         &nv_opts      },
    ['K']    = { exec_K,         &mx_opts      },
    ['L']    = { exec_L,         &nv_opts      },
    ['M']    = { exec_M,         &cmq_opts     },
    ['N']    = { exec_N,         &anv1_opts    },
    ['O']    = { exec_O,         &anx1_opts    },
    ['P']    = { exec_P,         &mvw_opts     },
    ['Q']    = { exec_Q,         &cnq_opts     },
    ['R']    = { exec_R,         &nv_opts      },
    ['S']    = { exec_S,         &admv1_opts   },
    ['T']    = { exec_T,         &cmx_opts     },
    ['U']    = { exec_U,         &mq_opts      },
    ['V']    = { exec_V,         &mx_opts      },
    ['W']    = { exec_W,         &cm_opts      },
    ['X']    = { exec_X,         &cmqx_opts    },
    ['Y']    = { exec_Y,         &v_opts       },
    ['Z']    = { exec_Z,         &no_opts      },
    ['[']    = { exec_lbracket,  &mq_opts      },
    ['\\']   = { exec_back,      &n_opts       },
    [']']    = { exec_rbracket,  &mqv_opts     },
    ['^']    = { NULL,           &no_opts      },
    ['_']    = { exec_ubar,      &anv1_opts    },
    ['`']    = { exec_bad,       &no_opts      },
    ['{']    = { exec_bad,       &no_opts      },
    ['|']    = { exec_vbar,      &x_opts       },
    ['}']    = { exec_bad,       &no_opts      },
    ['~']    = { exec_bad,       &no_opts      },
    [DEL]    = { exec_bad,       &no_opts      },
};

const uint cmd_count = countof(cmd_table); ///< No. of general commands

///  @var    e_table
///  @brief  Table for all commands starting with E.

const struct cmd_table e_table[] =
{
    { exec_E1,      &f_opts     },
    { exec_E2,      &f_opts     },
    { exec_E3,      &f_opts     },
    { exec_E4,      &f_opts     },
    { exec_EA,      &no_opts    },
    { exec_EB,      &av1_opts   },
    { exec_EC,      &n_opts     },
    { exec_ED,      &f_opts     },
    { exec_EE,      &f_opts     },
    { exec_EF,      &no_opts    },
    { exec_EG,      &ad1_opts   },
    { exec_EH,      &f_opts     },
    { exec_EI,      &av1_opts   },
    { exec_EJ,      &mv_opts    },
    { exec_EK,      &no_opts    },
    { exec_EL,      &a1_opts    },
    { exec_EM,      &no_opts    },
    { exec_EN,      &av1_opts   },
    { exec_EO,      &f_opts     },
    { exec_EP,      &no_opts    },
    { exec_EQ,      &aqv1_opts  },
    { exec_ER,      &av1_opts   },
    { exec_ES,      &f_opts     },
    { exec_ET,      &f_opts     },
    { exec_EU,      &f_opts     },
    { exec_EV,      &f_opts     },
    { exec_EW,      &av1_opts   },
    { exec_EX,      &no_opts    },
    { exec_EY,      &v_opts     },
    { exec_EZ,      &acq1_opts  },
    { exec_E_ubar,  &anv1_opts  },
};

const uint e_count = countof(e_table); ///< No. of E commands


///  @var    f_table
///  @brief  Table for all commands starting with F.

const struct cmd_table f_table[] =
{
    { exec_F_apos,  &x_opts     },
    { exec_F1,      &a1_opts    },
    { exec_F2,      &n_opts     },
    { exec_F3,      &n2_opts    },
    { exec_F_lt,    &x_opts     },
    { exec_F_gt,    &x_opts     },
    { exec_FB,      &amv1_opts  },
    { exec_FC,      &amv2_opts  },
    { exec_FD,      &anv1_opts  },
    { exec_FK,      &anv1_opts  },
    { exec_FL,      &mx_opts    },
    { exec_FN,      &amv2_opts  },
    { exec_FR,      &amv1_opts  },
    { exec_FS,      &admv2_opts },
    { exec_FU,      &mx_opts    },
    { exec_F_ubar,  &amv2_opts  },
    { exec_F_vbar,  &x_opts     },
};

const uint f_count = countof(f_table); ///< No. of F commands
