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

//lint -save -e708 -e785

///  @var    a1_opts
///  @brief  Options for commands that use @ and 1 text argument.

static const union cmd_opts a1_opts     = {{ .a=1, .t1=1 }};

///  @var    ac1_opts
///  @brief  Options for commands that use @, :, and 1 text argument.

static const union cmd_opts ac1_opts    = {{ .a=1, .c=1, .t1=1 }};

///  @var    acmq1_opts
///  @brief  Commands that use @, m, n, Q-registers, and 1 text argument.

static const union cmd_opts acmq1_opts  = {{ .a=1, .c=1, .m=1, .q=1, .t1=1 }};

///  @var    acn1_opts
///  @brief  Commands that use :, n, and 1 text argument.

static const union cmd_opts acn1_opts   = {{ .a=1, .c=1, .n=1, .t1=1 }};

///  @var    acq1_opts
///  @brief  Commands that use @, :, Q-registers, and 1 text argument.

static const union cmd_opts acq1_opts   = {{ .a=1, .c=1, .q=1, .t1=1 }};

///  @var    adm1_opts
///  @brief  Commands that use @, :, ::, m, n, and 1 text argument.

static const union cmd_opts adm1_opts   = {{ .a=1, .d=1, .m=1, .t1=1 }};

///  @var    adm2_opts
///  @brief  Commands that use @, :, ::, m, n, and 2 text argument3.

static const union cmd_opts adm2_opts   = {{ .a=1, .d=1, .m=1, .t2=1 }};

///  @var    ad1_opts
///  @brief  Commands that use @, :, ::, and 1 text argument.

static const union cmd_opts ad1_opts    = {{ .a=1, .d=1, .t1=1 }};

///  @var    acm1_opts
///  @brief  Commands that use @, :, m, n, and 1 text argument.

static const union cmd_opts acm1_opts   = {{ .a=1, .c=1, .m=1, .t1=1 }};

///  @var    acm2_opts
///  @brief  Commands that use @, :, m, n, and 2 text arguments.

static const union cmd_opts acm2_opts   = {{ .a=1, .c=1, .m=1, .t2=1 }};

///  @var    an1_opts
///  @brief  Commands that use @, n, and 1 text argument.

static const union cmd_opts an1_opts    = {{ .a=1, .n=1, .t1=1 }};

///  @var    c_opts
///  @brief  Commands that use :.

static const union cmd_opts c_opts      = {{ .c=1 }};

///  @var    cf_opts
///  @brief  Flag commands that use :.

static const union cmd_opts cf_opts     = {{ .c=1, .f=1 }};

///  @var    cfq_opts
///  @brief  Flag commands that use : and Q-registers.

static const union cmd_opts cfq_opts    = {{ .c=1, .f=1, .q=1 }};

///  @var    cm_opts
///  @brief  Commands that use :, m, and n.

static const union cmd_opts cm_opts     = {{ .c=1, .m=1 }};

///  @var    cmq_opts
///  @brief  Commands that use :, m, n, and Q-registers.

static const union cmd_opts cmq_opts    = {{ .c=1, .m=1, .q=1 }};

///  @var    cmw_opts.
///  @brief  Commands that use :, m, n, and W.

static const union cmd_opts cmw_opts    = {{ .c=1, .m=1, .w=1 }};

///  @var    cn_opts
///  @brief  Commands that use : and n.

static const union cmd_opts cn_opts     = {{ .c=1, .n=1 }};

///  @var    cnq_opts
///  @brief  Commands that use :, n, and Q-registers.

static const union cmd_opts cnq_opts    = {{ .c=1, .n=1, .q=1 }};

///  @var    f_opts
///  @brief  Flag commands.

static const union cmd_opts f_opts      = {{ .f=1 }};

///  @var    fm_opts
///  @brief  Flag commands.

static const union cmd_opts fm_opts      = {{ .f=1, .m=1 }};

///  @var    m_opts
///  @brief  Commands that use m and n.

static const union cmd_opts m_opts      = {{ .m=1 }};

///  @var    mq_opts
///  @brief  Commands that use m, n, and Q-registers.

static const union cmd_opts mq_opts     = {{ .m=1, .q=1 }};

///  @var    n_opts
///  @brief  Commands that use n.

static const union cmd_opts n_opts      = {{ .n=1 }};

///  @var    n2_opts
///  @brief  Commands that use n, and 1 text argument.

static const union cmd_opts n2_opts     = {{ .n=1, .t2=1 }};

///  @var    t1_opts
///  @brief  Commands that use 1 text argument.

static const union cmd_opts t1_opts     = {{ .t1=1 }};

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
///          Commands may be categorized as one of the following:
///
///          1. Commands that are ignored (e.g., space).
///          2. Commands that are part of an expression (e.g., digits or
///             operators).
///          3. Commands that never return a value.
///          4. Commands that always return a value.
///          5. Commands that might return a value (e.g., M).
///          6. Commands that return a value if colon-modified.
///          7. Commands that return a value unless preceded by an expression
///             (e.g., ET).
///          8. Commands that allow but ignore expressions (e.g., <ESC>).
///          9. Commands that pass expressions through to next command
///             (e.g., [ or ]).
///

const struct cmd_table cmd_table[] =
{
    [NUL]    = { NULL,            NULL          },
    [CTRL_A] = { exec_ctrl_A,     &acm1_opts    },
    [CTRL_B] = { exec_ctrl_B,     NULL          },
    [CTRL_C] = { exec_ctrl_C,     &m_opts       },
    [CTRL_D] = { exec_ctrl_D,     NULL          },
    [CTRL_E] = { exec_ctrl_E,     &f_opts       },
    [CTRL_F] = { exec_bad,        NULL          },
    [CTRL_G] = { exec_bad,        NULL          },
    [CTRL_H] = { exec_ctrl_H,     NULL          },
    [CTRL_I] = { exec_ctrl_I,     &t1_opts      },
    [LF]     = { NULL,            NULL          },
    [VT]     = { exec_bad,        NULL          },
    [FF]     = { NULL,            NULL          },
    [CR]     = { NULL,            NULL          },
    [CTRL_N] = { exec_ctrl_N,     &n_opts       },
    [CTRL_O] = { exec_ctrl_O,     NULL          },
    [CTRL_P] = { exec_bad,        NULL          },
    [CTRL_Q] = { exec_ctrl_Q,     &n_opts       },
    [CTRL_R] = { exec_ctrl_R,     &f_opts       },
    [CTRL_S] = { exec_ctrl_S,     NULL          },
    [CTRL_T] = { exec_ctrl_T,     &cm_opts      },
    [CTRL_U] = { exec_ctrl_U,     &acmq1_opts   },
    [CTRL_V] = { exec_ctrl_V,     &n_opts       },
    [CTRL_W] = { exec_ctrl_W,     &n_opts       },
    [CTRL_X] = { exec_ctrl_X,     &f_opts       },
    [CTRL_Y] = { exec_ctrl_Y,     NULL          },
    [CTRL_Z] = { exec_ctrl_Z,     NULL          },
    [ESC]    = { exec_escape,     &m_opts       },
    ['\x1C'] = { exec_bad,        NULL          },
    ['\x1D'] = { exec_bad,        NULL          },
    ['\x1E'] = { exec_ctrl_up,    NULL          },
    ['\x1F'] = { exec_ctrl_ubar,  NULL          },
    [SPACE]  = { NULL,            NULL          },
    ['!']    = { exec_bang,       &a1_opts      },
    ['"']    = { exec_quote,      &n_opts       },
    ['#']    = { exec_oper,       NULL          },
    ['$']    = { exec_bad,        NULL          },
    ['%']    = { exec_pct,        &cnq_opts     },
    ['&']    = { exec_oper,       NULL          },
    ['\'']   = { exec_apos,       &n_opts       },
    ['(']    = { exec_lparen,     NULL          },
    [')']    = { exec_rparen,     NULL          },
    ['*']    = { exec_oper,       NULL          },
    ['+']    = { exec_oper,       NULL          },
    [',']    = { exec_comma,      NULL          },
    ['-']    = { exec_oper,       NULL          }, 
    ['.']    = { exec_dot,        NULL          }, 
    ['/']    = { exec_oper,       NULL          }, 
    ['0']    = { exec_digit,      NULL          }, 
    ['1']    = { exec_digit,      NULL          },
    ['2']    = { exec_digit,      NULL          },
    ['3']    = { exec_digit,      NULL          },
    ['4']    = { exec_digit,      NULL          },
    ['5']    = { exec_digit,      NULL          },
    ['6']    = { exec_digit,      NULL          },
    ['7']    = { exec_digit,      NULL          },
    ['8']    = { exec_digit,      NULL          },
    ['9']    = { exec_digit,      NULL          },
    [':']    = { exec_colon,      NULL          },
    [';']    = { exec_semi,       &cn_opts      },
    ['<']    = { exec_lt,         &n_opts       },
    ['=']    = { exec_equals,     &acn1_opts    },
    ['>']    = { exec_gt,         &m_opts       },
    ['?']    = { exec_trace,      &c_opts       },
    ['@']    = { exec_atsign,     NULL          },
    ['A']    = { exec_A,          &cf_opts      },
    ['B']    = { exec_B,          NULL          },
    ['C']    = { exec_C,          &cn_opts      },
    ['D']    = { exec_D,          &cm_opts      },
    ['E']    = { NULL,            NULL          },
    ['F']    = { NULL,            NULL          },
    ['G']    = { exec_G,          &cnq_opts     },
    ['H']    = { exec_H,          NULL          },
    ['I']    = { exec_I,          &an1_opts     },
    ['J']    = { exec_J,          &cn_opts      },
    ['K']    = { exec_K,          &m_opts       },
    ['L']    = { exec_L,          &cn_opts      },
    ['M']    = { exec_M,          &cmq_opts     },
    ['N']    = { exec_N,          &acn1_opts    },
    ['O']    = { exec_O,          &an1_opts     },
    ['P']    = { exec_P,          &cmw_opts     },
    ['Q']    = { exec_Q,          &cfq_opts     },
    ['R']    = { exec_R,          &cn_opts      },
    ['S']    = { exec_S,          &adm1_opts    },
    ['T']    = { exec_T,          &cm_opts      },
    ['U']    = { exec_U,          &mq_opts      },
    ['V']    = { exec_V,          &m_opts       },
    ['W']    = { exec_W,          &cm_opts      },
    ['X']    = { exec_X,          &cmq_opts     },
    ['Y']    = { exec_Y,          &c_opts       },
    ['Z']    = { exec_Z,          NULL          },
    ['[']    = { exec_lbracket,   &mq_opts      },
    ['\\']   = { exec_back,       &n_opts       },
    [']']    = { exec_rbracket,   &cmq_opts     },
    ['^']    = { NULL,            NULL          },
    ['_']    = { exec_ubar,       &acn1_opts    },
    ['`']    = { exec_bad,        NULL          },
    ['{']    = { exec_bad,        NULL          },
    ['|']    = { exec_vbar,       &m_opts       },
    ['}']    = { exec_bad,        NULL          },
    ['~']    = { exec_bad,        NULL          },
    [DEL]    = { exec_bad,        NULL          },
};

const uint cmd_count = countof(cmd_table); ///< No. of general commands

///  @var    e_table
///  @brief  Table for all commands starting with E.

const struct cmd_table e_table[] =
{
    { exec_E1,      &fm_opts    },
    { exec_E2,      &fm_opts    },
    { exec_E3,      &fm_opts    },
    { exec_E4,      &fm_opts    },
    { exec_EA,      NULL        },
    { exec_EB,      &ac1_opts   },
    { exec_EC,      &n_opts     },
    { exec_ED,      &fm_opts    },
    { exec_EE,      &f_opts     },
    { exec_EF,      NULL        },
    { exec_EG,      &ad1_opts   },
    { exec_EH,      &fm_opts    },
    { exec_EI,      &ac1_opts   },
    { exec_EJ,      &cm_opts    },
    { exec_EK,      NULL        },
    { exec_EL,      &a1_opts    },
    { exec_EM,      NULL        },
    { exec_EN,      &ac1_opts   },
    { exec_EO,      &f_opts     },
    { exec_EP,      NULL        },
    { exec_EQ,      &acq1_opts  },
    { exec_ER,      &ac1_opts   },
    { exec_ES,      &f_opts     },
    { exec_ET,      &fm_opts    },
    { exec_EU,      &f_opts     },
    { exec_EV,      &f_opts     },
    { exec_EW,      &ac1_opts   },
    { exec_EX,      NULL        },
    { exec_EY,      &c_opts     },
    { exec_EZ,      &acq1_opts  },
    { exec_E_ubar,  &acn1_opts  },
};

const uint e_count = countof(e_table); ///< No. of E commands


///  @var    f_table
///  @brief  Table for all commands starting with F.

const struct cmd_table f_table[] =
{
    { exec_F_apos,  &m_opts     },
    { exec_F1,      &a1_opts    },
    { exec_F2,      &n_opts     },
    { exec_F3,      &n2_opts    },
    { exec_F_lt,    &m_opts     },
    { exec_F_gt,    &m_opts     },
    { exec_FB,      &acm1_opts  },
    { exec_FC,      &acm2_opts  },
    { exec_FD,      &acn1_opts  },
    { exec_FK,      &acn1_opts  },
    { exec_FL,      &m_opts     },
    { exec_FN,      &acm2_opts  },
    { exec_FR,      &acm1_opts  },
    { exec_FS,      &adm2_opts  },
    { exec_FU,      &m_opts     },
    { exec_F_ubar,  &acm2_opts  },
    { exec_F_vbar,  &m_opts     },
};

const uint f_count = countof(f_table); ///< No. of F commands
