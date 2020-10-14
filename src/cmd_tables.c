///
///  @file    cmd_tables.c
///  @brief   Data tables used in executing command strings.
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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


///  @def    a1_opts
///  @brief  Commands that use @ and 1 text argument.

#define a1_opts     (OPT_A | OPT_T1)

///  @def    ac1_opts
///  @brief  Command options.

#define ac1_opts    (OPT_A | OPT_C | OPT_T1)

///  @def    acmq1_opts
///  @brief  Command options.

#define acmq1_opts  (OPT_A | OPT_C | OPT_M | OPT_N | OPT_Q | OPT_T1)

///  @def    acn1_opts
///  @brief  Command options.

#define acn1_opts   (OPT_A | OPT_C | OPT_N | OPT_T1)

///  @def    acq1_opts
///  @brief  Command options.

#define acq1_opts   (OPT_A | OPT_C | OPT_Q | OPT_T1)

///  @def    adm2_opts
///  @brief  Command options.

#define adm2_opts   (OPT_A | OPT_C | OPT_D | OPT_M | OPT_N | OPT_T1 | OPT_T2)

///  @def    ad1_opts
///  @brief  Command options.

#define ad1_opts    (OPT_A | OPT_C | OPT_D | OPT_T1)

///  @def    acm1_opts
///  @brief  Command options.

#define acm1_opts   (OPT_A | OPT_C | OPT_M | OPT_N | OPT_T1)

///  @def    acm2_opts
///  @brief  Command options.

#define acm2_opts   (OPT_A | OPT_C | OPT_M | OPT_N | OPT_T1 | OPT_T2)

///  @def    adm1_opts
///  @brief  Command options.

#define adm1_opts   (OPT_A | OPT_C | OPT_D | OPT_M | OPT_N | OPT_T1)

///  @def    am1_opts
///  @brief  Command options.

#define am1_opts    (OPT_A | OPT_M | OPT_N | OPT_T1)

///  @def    am2_opts
///  @brief  Command options.

#define am2_opts    (OPT_A | OPT_M | OPT_N | OPT_T1 | OPT_T2)

///  @def    an1_opts
///  @brief  Command options.

#define an1_opts    (OPT_A | OPT_N | OPT_T1)

///  @def    c_opts
///  @brief  Command options.

#define c_opts      (OPT_C)

///  @def    cf_opts
///  @brief  Command options.

#define cf_opts     (OPT_C | OPT_F | OPT_N)

///  @def    cfq_opts
///  @brief  Command options.

#define cfq_opts    (OPT_C | OPT_F | OPT_N | OPT_Q)

///  @def    cm_opts
///  @brief  Command options.

#define cm_opts     (OPT_C | OPT_M | OPT_N)

///  @def    cmq_opts
///  @brief  Command options.

#define cmq_opts    (OPT_C | OPT_M | OPT_N | OPT_Q)

///  @def    cn_opts
///  @brief  Command options.

#define cn_opts     (OPT_C | OPT_N)

///  @def    cnq_opts
///  @brief  Command options.

#define cnq_opts    (OPT_C | OPT_N | OPT_Q)

///  @def    f_opts
///  @brief  Command options.

#define f_opts      (OPT_F | OPT_N)

///  @def    fm_opts
///  @brief  Command options.

#define fm_opts     (OPT_F | OPT_M | OPT_N)

///  @def    m_opts
///  @brief  Command options.

#define m_opts      (OPT_M | OPT_N)

///  @def    mq_opts
///  @brief  Command options.

#define mq_opts     (OPT_M | OPT_N | OPT_Q)

///  @def    n_opts
///  @brief  Command options.

#define n_opts      (OPT_N)

///  @def    t1_opts
///  @brief  Command options.

#define t1_opts     (OPT_T1)


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

#define ENTRY(chr, func, opts) [chr] = { func, opts, #func }

#else

#define ENTRY(chr, func, opts) [chr] = { func, opts }

#endif

const struct cmd_table cmd_table[] =
{
    ENTRY(NUL,     NULL,           0),
    ENTRY(CTRL_A,  exec_ctrl_A,    acm1_opts),
    ENTRY(CTRL_B,  exec_ctrl_B,    OPT_S),
    ENTRY(CTRL_C,  exec_ctrl_C,    m_opts),
    ENTRY(CTRL_D,  exec_ctrl_D,    0),
    ENTRY(CTRL_E,  exec_ctrl_E,    f_opts),
    ENTRY(CTRL_F,  NULL,           OPT_B),
    ENTRY(CTRL_G,  NULL,           OPT_B),
    ENTRY(CTRL_H,  exec_ctrl_H,    OPT_S),
    ENTRY(CTRL_I,  exec_ctrl_I,    t1_opts),
    ENTRY(LF,      NULL,           0),
    ENTRY(VT,      NULL,           OPT_B),
    ENTRY(FF,      NULL,           0),
    ENTRY(CR,      NULL,           0),
    ENTRY(CTRL_N,  exec_ctrl_N,    n_opts),
    ENTRY(CTRL_O,  exec_ctrl_O,    0),
    ENTRY(CTRL_P,  exec_ctrl_P,    0),
    ENTRY(CTRL_Q,  exec_ctrl_Q,    n_opts),
    ENTRY(CTRL_R,  exec_ctrl_R,    f_opts),
    ENTRY(CTRL_S,  exec_ctrl_S,    OPT_S),
    ENTRY(CTRL_T,  exec_ctrl_T,    cm_opts),
    ENTRY(CTRL_U,  exec_ctrl_U,    acmq1_opts),
    ENTRY(CTRL_V,  exec_ctrl_V,    n_opts),
    ENTRY(CTRL_W,  exec_ctrl_W,    n_opts),
    ENTRY(CTRL_X,  exec_ctrl_X,    f_opts),
    ENTRY(CTRL_Y,  exec_ctrl_Y,    OPT_S),
    ENTRY(CTRL_Z,  exec_ctrl_Z,    OPT_S),
    ENTRY(ESC,     exec_escape,    m_opts),
    ENTRY('\x1C',  NULL,           OPT_B),
    ENTRY('\x1D',  NULL,           OPT_B),
    ENTRY('\x1E',  exec_ctrl_up,   OPT_S),
    ENTRY('\x1F',  exec_ctrl_ubar, OPT_S),
    ENTRY(SPACE,   NULL,           0),
    ENTRY('!',     exec_bang,      a1_opts),
    ENTRY('"',     exec_quote,     n_opts),
    ENTRY('#',     exec_oper,      OPT_S),
    ENTRY('$',     NULL,           OPT_B),
    ENTRY('%',     exec_pct,       cnq_opts),
    ENTRY('&',     exec_oper,      OPT_S),
    ENTRY('\'',    exec_apos,      m_opts),
    ENTRY('(',     exec_lparen,    OPT_S),
    ENTRY(')',     exec_rparen,    OPT_S),
    ENTRY('*',     exec_oper,      OPT_S),
    ENTRY('+',     exec_oper,      OPT_S),
    ENTRY(',',     exec_comma,     OPT_S),
    ENTRY('-',     exec_oper,      OPT_S),
    ENTRY('.',     exec_dot,       OPT_S),
    ENTRY('/',     exec_oper,      OPT_S),
    ENTRY('0',     exec_number,    OPT_S),
    ENTRY('1',     exec_number,    OPT_S),
    ENTRY('2',     exec_number,    OPT_S),
    ENTRY('3',     exec_number,    OPT_S),
    ENTRY('4',     exec_number,    OPT_S),
    ENTRY('5',     exec_number,    OPT_S),
    ENTRY('6',     exec_number,    OPT_S),
    ENTRY('7',     exec_number,    OPT_S),
    ENTRY('8',     exec_number,    OPT_S),
    ENTRY('9',     exec_number,    OPT_S),
    ENTRY(':',     NULL,           0),
    ENTRY(';',     exec_semi,      cn_opts),
    ENTRY('<',     exec_lt,        n_opts),
    ENTRY('=',     exec_equals,    acn1_opts),
    ENTRY('>',     exec_gt,        m_opts),
    ENTRY('?',     exec_trace,     c_opts),
    ENTRY('@',     NULL,           0),
    ENTRY('A',     exec_A,         cf_opts),
    ENTRY('a',     exec_A,         cf_opts),
    ENTRY('B',     exec_B,         OPT_S),
    ENTRY('b',     exec_B,         OPT_S),
    ENTRY('C',     exec_C,         cn_opts),
    ENTRY('c',     exec_C,         cn_opts),
    ENTRY('D',     exec_D,         cm_opts),
    ENTRY('d',     exec_D,         cm_opts),
    ENTRY('E',     NULL,           0),
    ENTRY('e',     NULL,           0),
    ENTRY('F',     NULL,           0),
    ENTRY('f',     NULL,           0),
    ENTRY('G',     exec_G,         cnq_opts),
    ENTRY('g',     exec_G,         cnq_opts),
    ENTRY('H',     exec_H,         OPT_S),
    ENTRY('h',     exec_H,         OPT_S),
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
    ENTRY('P',     exec_P,         cm_opts),
    ENTRY('p',     exec_P,         cm_opts),
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
    ENTRY('Z',     exec_Z,         OPT_S),
    ENTRY('z',     exec_Z,         OPT_S),
    ENTRY('[',     exec_lbracket,  mq_opts),
    ENTRY('\\',    exec_bslash,    n_opts),
    ENTRY(']',     exec_rbracket,  cmq_opts),
    ENTRY('^',     NULL,           OPT_B),
    ENTRY('_',     exec_ubar,      acn1_opts),
    ENTRY('`',     NULL,           OPT_B),
    ENTRY('{',     NULL,           OPT_B),
    ENTRY('|',     exec_vbar,      m_opts),
    ENTRY('}',     NULL,           OPT_B),
    ENTRY('~',     NULL,           OPT_B),
    ENTRY(DEL,     NULL,           OPT_B),
};

const uint cmd_max = countof(cmd_table); ///< Maximum command


///  @var    e_table
///  @brief  Table for all commands starting with E.

const struct cmd_table e_table[] =
{
    ENTRY('%',  exec_E_pct,   acq1_opts),
    ENTRY('1',  exec_E1,      fm_opts),
    ENTRY('2',  exec_E2,      fm_opts),
    ENTRY('3',  exec_E3,      fm_opts),
    ENTRY('4',  exec_E4,      fm_opts),
    ENTRY('A',  exec_EA,      0),
    ENTRY('a',  exec_EA,      0),
    ENTRY('B',  exec_EB,      ac1_opts),
    ENTRY('b',  exec_EB,      ac1_opts),
    ENTRY('C',  exec_EC,      n_opts),
    ENTRY('c',  exec_EC,      n_opts),
    ENTRY('D',  exec_ED,      fm_opts),
    ENTRY('d',  exec_ED,      fm_opts),
    ENTRY('E',  exec_EE,      f_opts),
    ENTRY('e',  exec_EE,      f_opts),
    ENTRY('F',  exec_EF,      0),
    ENTRY('f',  exec_EF,      0),
    ENTRY('G',  exec_EG,      ad1_opts),
    ENTRY('g',  exec_EG,      ad1_opts),
    ENTRY('H',  exec_EH,      fm_opts),
    ENTRY('h',  exec_EH,      fm_opts),
    ENTRY('I',  exec_EI,      ac1_opts),
    ENTRY('i',  exec_EI,      ac1_opts),
    ENTRY('J',  exec_EJ,      cm_opts),
    ENTRY('j',  exec_EJ,      cm_opts),
    ENTRY('K',  exec_EK,      0),
    ENTRY('k',  exec_EK,      0),
    ENTRY('L',  exec_EL,      ac1_opts),
    ENTRY('l',  exec_EL,      ac1_opts),
    ENTRY('N',  exec_EN,      ac1_opts),
    ENTRY('n',  exec_EN,      ac1_opts),
    ENTRY('O',  exec_EO,      cf_opts),
    ENTRY('o',  exec_EO,      cf_opts),
    ENTRY('P',  exec_EP,      0),
    ENTRY('p',  exec_EP,      0),
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
    ENTRY('X',  exec_EX,      0),
    ENTRY('x',  exec_EX,      0),
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
    ENTRY('1',   exec_F1,      am2_opts),
    ENTRY('2',   exec_F2,      am2_opts),
    ENTRY('3',   exec_F3,      am2_opts),
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
