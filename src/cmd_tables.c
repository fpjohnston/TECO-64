///
///  @file    cmd_tables.c
///  @brief   Data tables used in executing command strings.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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

static const union cmd_opts a1_opts = {{ .a=1, .t1=1 }};

///  @var    ac1_opts
///  @brief  Options for commands that use @, :, and 1 text argument.

static const union cmd_opts ac1_opts = {{ .a=1, .c=1, .t1=1 }};

///  @var    acmq1_opts
///  @brief  Commands that use @, m, n, Q-registers, and 1 text argument.

static const union cmd_opts acmq1_opts = {{ .a=1, .c=1, .m=1, .n=1, .q=1, .t1=1 }};

///  @var    acn1_opts
///  @brief  Commands that use :, n, and 1 text argument.

static const union cmd_opts acn1_opts = {{ .a=1, .c=1, .n=1, .t1=1 }};

///  @var    acq1_opts
///  @brief  Commands that use @, :, Q-registers, and 1 text argument.

static const union cmd_opts acq1_opts = {{ .a=1, .c=1, .q=1, .t1=1 }};

///  @var    adm2_opts
///  @brief  Commands that use @, :, ::, m, n, and 2 text argument3.

static const union cmd_opts adm2_opts = {{ .a=1, .c=1, .d=1, .m=1, .n=1, .t1=1, .t2=1 }};

///  @var    ad1_opts
///  @brief  Commands that use @, :, ::, and 1 text argument.

static const union cmd_opts ad1_opts = {{ .a=1, .c=1, .d=1, .t1=1 }};

///  @var    acm1_opts
///  @brief  Commands that use @, :, m, n, and 1 text argument.

static const union cmd_opts acm1_opts = {{ .a=1, .c=1, .m=1, .n=1, .t1=1 }};

///  @var    acm2_opts
///  @brief  Commands that use @, :, m, n, and 2 text arguments.

static const union cmd_opts acm2_opts = {{ .a=1, .c=1, .m=1, .n=1, .t1=1, .t2=1 }};

///  @var    adm1_opts
///  @brief  Commands that use @, :, ::, m, n, and 1 text argument.

static const union cmd_opts adm1_opts = {{ .a=1, .c=1, .d=1, .m=1, .n=1, .t1=1 }};

///  @var    am1_opts
///  @brief  Commands that use @, m, n, and 1 text argument.

static const union cmd_opts am1_opts = {{ .a=1, .m=1, .n=1, .t1=1 }};

///  @var    an1_opts
///  @brief  Commands that use @, n, and 1 text argument.

static const union cmd_opts an1_opts = {{ .a=1, .n=1, .t1=1 }};

///  @var    c_opts
///  @brief  Commands that use :.

static const union cmd_opts c_opts = {{ .c=1 }};

///  @var    cf_opts
///  @brief  Flag commands that use :.

static const union cmd_opts cf_opts = {{ .c=1, .f=1, .n=1 }};

///  @var    cfq_opts
///  @brief  Flag commands that use : and Q-registers.

static const union cmd_opts cfq_opts = {{ .c=1, .f=1, .n=1, .q=1 }};

///  @var    cm_opts
///  @brief  Commands that use :, m, and n.

static const union cmd_opts cm_opts = {{ .c=1, .m=1, .n=1 }};

///  @var    cmq_opts
///  @brief  Commands that use :, m, n, and Q-registers.

static const union cmd_opts cmq_opts = {{ .c=1, .m=1, .n=1, .q=1 }};

///  @var    cmw_opts.
///  @brief  Commands that use :, m, n, and W.

static const union cmd_opts cmw_opts = {{ .c=1, .m=1, .n=1, .w=1 }};

///  @var    cn_opts
///  @brief  Commands that use : and n.

static const union cmd_opts cn_opts = {{ .c=1, .n=1 }};

///  @var    cnq_opts
///  @brief  Commands that use :, n, and Q-registers.

static const union cmd_opts cnq_opts = {{ .c=1, .n=1, .q=1 }};

///  @var    f_opts
///  @brief  Flag commands.

static const union cmd_opts f_opts = {{ .f=1, .n=1 }};

///  @var    fm_opts
///  @brief  Flag commands.

static const union cmd_opts fm_opts = {{ .f=1, .m=1, .n=1 }};

///  @var    m_opts
///  @brief  Commands that use m and n.

static const union cmd_opts m_opts = {{ .m=1, .n=1 }};

///  @var    m2_opts
///  @brief  Commands that use m and n, and 2 text arguments.

static const union cmd_opts m2_opts = {{ .m=1, .n=1, .t1=1, .t2=1 }};

///  @var    mq_opts
///  @brief  Commands that use m, n, and Q-registers.

static const union cmd_opts mq_opts = {{ .m=1, .n=1, .q=1 }};

///  @var    n_opts
///  @brief  Commands that use n.

static const union cmd_opts n_opts = {{ .n=1 }};

///  @var    t1_opts
///  @brief  Commands that use 1 text argument.

static const union cmd_opts t1_opts = {{ .t1=1 }};

///  @var    z_opts
///  @brief  Commands that use no arguments (but which need to be
///          executed as though they did, such as EX).

static const union cmd_opts z_opts = {{ .z=1 }};

///  @var    null_opts
///  @brief  Commands such as operators that don't use arguments.

static const union cmd_opts null_opts = { .bits=0 };

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
///          8. Commands that allow but ignore expressions (e.g., ESCape).
///          9. Commands that pass expressions through to next command
///             (e.g., [ or ]).
///

/// @def    ENTRY(chr, func, opts)
/// @brief  Defines entries in command table.

#if     defined(TECO_TRACE)

#define ENTRY(chr, func, opts) [chr] = { func, &opts, #func }

#else

#define ENTRY(chr, func, opts) [chr] = { func, &opts }

#endif

const struct cmd_table cmd_table[] =
{
    ENTRY(NUL,     NULL,           null_opts),
    ENTRY(CTRL_A,  exec_ctrl_A,    acm1_opts),
    ENTRY(CTRL_B,  exec_ctrl_B,    null_opts),
    ENTRY(CTRL_C,  exec_ctrl_C,    m_opts),
    ENTRY(CTRL_D,  exec_ctrl_D,    z_opts),
    ENTRY(CTRL_E,  exec_ctrl_E,    f_opts),
    ENTRY(CTRL_F,  exec_bad,       null_opts),
    ENTRY(CTRL_G,  exec_bad,       null_opts),
    ENTRY(CTRL_H,  exec_ctrl_H,    null_opts),
    ENTRY(CTRL_I,  exec_ctrl_I,    t1_opts),
    ENTRY(LF,      NULL,           null_opts),
    ENTRY(VT,      exec_bad,       null_opts),
    ENTRY(FF,      NULL,           null_opts),
    ENTRY(CR,      NULL,           null_opts),
    ENTRY(CTRL_N,  exec_ctrl_N,    n_opts),
    ENTRY(CTRL_O,  exec_ctrl_O,    z_opts),
    ENTRY(CTRL_P,  exec_bad,       null_opts),
    ENTRY(CTRL_Q,  exec_ctrl_Q,    n_opts),
    ENTRY(CTRL_R,  exec_ctrl_R,    f_opts),
    ENTRY(CTRL_S,  exec_ctrl_S,    null_opts),
    ENTRY(CTRL_T,  exec_ctrl_T,    cm_opts),
    ENTRY(CTRL_U,  exec_ctrl_U,    acmq1_opts),
    ENTRY(CTRL_V,  exec_ctrl_V,    n_opts),
    ENTRY(CTRL_W,  exec_ctrl_W,    n_opts),
    ENTRY(CTRL_X,  exec_ctrl_X,    f_opts),
    ENTRY(CTRL_Y,  exec_ctrl_Y,    null_opts),
    ENTRY(CTRL_Z,  exec_ctrl_Z,    null_opts),
    ENTRY(ESC,     exec_escape,    m_opts),
    ENTRY('\x1C',  exec_bad,       null_opts),
    ENTRY('\x1D',  exec_bad,       null_opts),
    ENTRY('\x1E',  exec_ctrl_up,   null_opts),
    ENTRY('\x1F',  exec_ctrl_ubar, null_opts),
    ENTRY(SPACE,   NULL,           null_opts),
    ENTRY('!',     exec_bang,      a1_opts),
    ENTRY('"',     exec_quote,     n_opts),
    ENTRY('#',     exec_oper,      null_opts),
    ENTRY('$',     exec_bad,       null_opts),
    ENTRY('%',     exec_pct,       cnq_opts),
    ENTRY('&',     exec_oper,      null_opts),
    ENTRY('\'',    exec_apos,      n_opts),
    ENTRY('(',     exec_lparen,    null_opts),
    ENTRY(')',     exec_rparen,    null_opts),
    ENTRY('*',     exec_oper,      null_opts),
    ENTRY('+',     exec_oper,      null_opts),
    ENTRY(',',     exec_comma,     null_opts),
    ENTRY('-',     exec_oper,      null_opts), 
    ENTRY('.',     exec_dot,       null_opts), 
    ENTRY('/',     exec_oper,      null_opts), 
    ENTRY('0',     exec_number,    null_opts), 
    ENTRY('1',     exec_number,    null_opts),
    ENTRY('2',     exec_number,    null_opts),
    ENTRY('3',     exec_number,    null_opts),
    ENTRY('4',     exec_number,    null_opts),
    ENTRY('5',     exec_number,    null_opts),
    ENTRY('6',     exec_number,    null_opts),
    ENTRY('7',     exec_number,    null_opts),
    ENTRY('8',     exec_number,    null_opts),
    ENTRY('9',     exec_number,    null_opts),
    ENTRY(':',     exec_colon,     null_opts),
    ENTRY(';',     exec_semi,      cn_opts),
    ENTRY('<',     exec_lt,        n_opts),
    ENTRY('=',     exec_equals,    acn1_opts),
    ENTRY('>',     exec_gt,        m_opts),
    ENTRY('?',     exec_trace,     c_opts),
    ENTRY('@',     exec_atsign,    null_opts),
    ENTRY('A',     exec_A,         cf_opts),
    ENTRY('a',     exec_A,         cf_opts),
    ENTRY('B',     exec_B,         null_opts),
    ENTRY('b',     exec_B,         null_opts),
    ENTRY('C',     exec_C,         cn_opts),
    ENTRY('c',     exec_C,         cn_opts),
    ENTRY('D',     exec_D,         cm_opts),
    ENTRY('d',     exec_D,         cm_opts),
    ENTRY('E',     NULL,           null_opts),
    ENTRY('e',     NULL,           null_opts),
    ENTRY('F',     NULL,           null_opts),
    ENTRY('f',     NULL,           null_opts),
    ENTRY('G',     exec_G,         cnq_opts),
    ENTRY('g',     exec_G,         cnq_opts),
    ENTRY('H',     exec_H,         null_opts),
    ENTRY('h',     exec_H,         null_opts),
    ENTRY('I',     exec_I,         am1_opts),
    ENTRY('i',     exec_I,         am1_opts),
    ENTRY('J',     exec_J,         cn_opts),
    ENTRY('j',     exec_J,         cn_opts),
    ENTRY('K',     exec_K,         m_opts),
    ENTRY('k',     exec_K,         m_opts),
    ENTRY('L',     exec_L,         cn_opts),
    ENTRY('l',     exec_L,         cn_opts),
    ENTRY('M',     exec_M,         cmq_opts),
    ENTRY('m',     exec_M,         cmq_opts),
    ENTRY('N',     exec_N,         acn1_opts),
    ENTRY('n',     exec_N,         acn1_opts),
    ENTRY('O',     exec_O,         an1_opts),
    ENTRY('o',     exec_O,         an1_opts),
    ENTRY('P',     exec_P,         cmw_opts),
    ENTRY('p',     exec_P,         cmw_opts),
    ENTRY('Q',     exec_Q,         cfq_opts),
    ENTRY('q',     exec_Q,         cfq_opts),
    ENTRY('R',     exec_R,         cn_opts),
    ENTRY('r',     exec_R,         cn_opts),
    ENTRY('S',     exec_S,         adm1_opts),
    ENTRY('s',     exec_S,         adm1_opts),
    ENTRY('T',     exec_T,         cm_opts),
    ENTRY('t',     exec_T,         cm_opts),
    ENTRY('U',     exec_U,         mq_opts),
    ENTRY('u',     exec_U,         mq_opts),
    ENTRY('V',     exec_V,         m_opts),
    ENTRY('v',     exec_V,         m_opts),
    ENTRY('W',     exec_W,         cm_opts),
    ENTRY('w',     exec_W,         cm_opts),
    ENTRY('X',     exec_X,         cmq_opts),
    ENTRY('x',     exec_X,         cmq_opts),
    ENTRY('Y',     exec_Y,         c_opts),
    ENTRY('y',     exec_Y,         c_opts),
    ENTRY('Z',     exec_Z,         null_opts),
    ENTRY('z',     exec_Z,         null_opts),
    ENTRY('[',     exec_lbracket,  mq_opts),
    ENTRY('\\',    exec_bslash,    n_opts),
    ENTRY(']',     exec_rbracket,  cmq_opts),
    ENTRY('^',     NULL,           null_opts),
    ENTRY('_',     exec_ubar,      acn1_opts),
    ENTRY('`',     exec_bad,       null_opts),
    ENTRY('{',     exec_bad,       null_opts),
    ENTRY('|',     exec_vbar,      m_opts),
    ENTRY('}',     exec_bad,       null_opts),
    ENTRY('~',     exec_bad,       null_opts),
    ENTRY(DEL,     exec_bad,       null_opts),
};

const uint cmd_max = countof(cmd_table); ///< Maximum command


///  @var    e_table
///  @brief  Table for all commands starting with E.

const struct cmd_table e_table[] =
{
    ENTRY('1',  exec_E1,      fm_opts),
    ENTRY('2',  exec_E2,      fm_opts),
    ENTRY('3',  exec_E3,      fm_opts),
    ENTRY('4',  exec_E4,      fm_opts),
    ENTRY('A',  exec_EA,      z_opts),
    ENTRY('a',  exec_EA,      z_opts),
    ENTRY('B',  exec_EB,      ac1_opts),
    ENTRY('b',  exec_EB,      ac1_opts),
    ENTRY('C',  exec_EC,      n_opts),
    ENTRY('c',  exec_EC,      n_opts),
    ENTRY('D',  exec_ED,      fm_opts),
    ENTRY('d',  exec_ED,      fm_opts),
    ENTRY('E',  exec_EE,      f_opts),
    ENTRY('e',  exec_EE,      f_opts),
    ENTRY('F',  exec_EF,      z_opts),
    ENTRY('f',  exec_EF,      z_opts),
    ENTRY('G',  exec_EG,      ad1_opts),
    ENTRY('g',  exec_EG,      ad1_opts),
    ENTRY('H',  exec_EH,      fm_opts),
    ENTRY('h',  exec_EH,      fm_opts),
    ENTRY('I',  exec_EI,      ad1_opts),
    ENTRY('i',  exec_EI,      ad1_opts),
    ENTRY('J',  exec_EJ,      cm_opts),
    ENTRY('j',  exec_EJ,      cm_opts),
    ENTRY('K',  exec_EK,      z_opts),
    ENTRY('k',  exec_EK,      z_opts),
    ENTRY('L',  exec_EL,      a1_opts),
    ENTRY('l',  exec_EL,      a1_opts),
    ENTRY('M',  exec_EM,      acq1_opts),
    ENTRY('m',  exec_EM,      acq1_opts),
    ENTRY('N',  exec_EN,      ac1_opts),
    ENTRY('n',  exec_EN,      ac1_opts),
    ENTRY('O',  exec_EO,      f_opts),
    ENTRY('o',  exec_EO,      f_opts),
    ENTRY('P',  exec_EP,      z_opts),
    ENTRY('p',  exec_EP,      z_opts),
    ENTRY('Q',  exec_EQ,      acq1_opts),
    ENTRY('q',  exec_EQ,      acq1_opts),
    ENTRY('R',  exec_ER,      ac1_opts),
    ENTRY('r',  exec_ER,      ac1_opts),
    ENTRY('S',  exec_ES,      f_opts), 
    ENTRY('s',  exec_ES,      f_opts),
    ENTRY('T',  exec_ET,      fm_opts),
    ENTRY('t',  exec_ET,      fm_opts),
    ENTRY('U',  exec_EU,      f_opts),
    ENTRY('u',  exec_EU,      f_opts), 
    ENTRY('V',  exec_EV,      f_opts),
    ENTRY('v',  exec_EV,      f_opts),
    ENTRY('W',  exec_EW,      ac1_opts),
    ENTRY('w',  exec_EW,      ac1_opts),
    ENTRY('X',  exec_EX,      z_opts),
    ENTRY('x',  exec_EX,      z_opts),
    ENTRY('Y',  exec_EY,      c_opts),
    ENTRY('y',  exec_EY,      c_opts),
    ENTRY('_',  exec_E_ubar,  acn1_opts),
};

const uint e_max = countof(e_table); ///< Maximum E command


///  @var    f_table
///  @brief  Table for all commands starting with F.

const struct cmd_table f_table[] =
{
    ENTRY('\'',  exec_F_apos,  m_opts),
    ENTRY('1',   exec_F1,      m2_opts),
    ENTRY('2',   exec_F2,      m2_opts),
    ENTRY('3',   exec_F3,      m2_opts),
    ENTRY('<',   exec_F_lt,    m_opts),
    ENTRY('>',   exec_F_gt,    m_opts),
    ENTRY('B',   exec_FB,      acm1_opts),
    ENTRY('b',   exec_FB,      acm1_opts),
    ENTRY('C',   exec_FC,      acm2_opts),
    ENTRY('c',   exec_FC,      acm2_opts),
    ENTRY('D',   exec_FD,      acn1_opts),
    ENTRY('d',   exec_FD,      acn1_opts),
    ENTRY('K',   exec_FK,      acn1_opts),
    ENTRY('k',   exec_FK,      acn1_opts),
    ENTRY('L',   exec_FL,      m_opts),
    ENTRY('l',   exec_FL,      m_opts),
    ENTRY('N',   exec_FN,      acm2_opts),
    ENTRY('n',   exec_FN,      acm2_opts),
    ENTRY('R',   exec_FR,      acm1_opts),
    ENTRY('r',   exec_FR,      acm1_opts),
    ENTRY('S',   exec_FS,      adm2_opts),
    ENTRY('s',   exec_FS,      adm2_opts),
    ENTRY('U',   exec_FU,      m_opts),
    ENTRY('u',   exec_FU,      m_opts),
    ENTRY('_',   exec_F_ubar,  acm2_opts),
    ENTRY('|',   exec_F_vbar,  m_opts),
};

const uint f_max = countof(f_table); ///< Maximum F command
