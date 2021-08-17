///
///  @file    commands.h
///  @brief   Data tables used in executing command strings.
///
///  *** Automatically generated from template file. DO NOT MODIFY. ***
///
///  @copyright 2020-2021 Franklin P. Johnston / Nowwith Treble Software
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

#include "ascii.h"

// Command table definitions

///  @typedef parse_func
///  @brief   Function to parse command arguments.

typedef bool (parse_func)(struct cmd *cmd);

///  @typedef scan_func
///  @brief   Function to scan command.

typedef bool (scan_func)(struct cmd *cmd);

///  @typedef exec_func
///  @brief   Function to execute command.

typedef void (exec_func)(struct cmd *cmd);

///  @struct cmd_table
///
///  @brief  Format of command tables used to parse and execute commands.

struct cmd_table
{
    parse_func *parse;              ///< Parse function format
    scan_func *scan;                ///< Scan function
    exec_func *exec;                ///< Execute function
    bool mn_args;                   ///< true if exec preserves m and n args

#if     defined(TECO_TRACE)

    const char *scan_name;          ///< Name of scan function
    const char *exec_name;          ///< Name of execute function

#endif

};

///
///  @var    cmd_table
///
///  @brief  Dispatch table, defining functions to scan and execute commands.
///

/// @def    ENTRY(chr, scan, exec)
/// @brief  Defines entries in command table.

#if     defined(TECO_TRACE)

#define ENTRY(chr, parse, scan, exec, mn) [chr] = { parse, scan, exec, mn, #scan, #exec }

#else

#define ENTRY(chr, parse, scan, exec, mn) [chr] = { parse, scan, exec, mn }

#endif

static const struct cmd_table cmd_table[] =
{
    ENTRY(NUL,     NULL,            scan_bad,        NULL,            false),
    ENTRY(CTRL_A,  NULL,            parse_c1,        exec_ctrl_A,     false),
    ENTRY(CTRL_B,  parse_oper,      scan_ctrl_B,     NULL,            false),
    ENTRY(CTRL_C,  NULL,            parse_escape,    exec_ctrl_C,     false),
    ENTRY(CTRL_D,  NULL,            parse_X,         exec_ctrl_D,     false),
    ENTRY(CTRL_E,  NULL,            parse_flag1,     exec_ctrl_E,     false),
    ENTRY(CTRL_F,  NULL,            scan_bad,        NULL,            false),
    ENTRY(CTRL_G,  NULL,            scan_bad,        NULL,            false),
    ENTRY(CTRL_H,  parse_oper,      scan_ctrl_H,     NULL,            false),
    ENTRY(CTRL_I,  NULL,            parse_1,         exec_ctrl_I,     false),
    ENTRY(LF,      NULL,            scan_nop,        NULL,            false),
    ENTRY(VT,      NULL,            scan_nop,        NULL,            false),
    ENTRY(FF,      NULL,            scan_nop,        NULL,            false),
    ENTRY(CR,      NULL,            scan_nop,        NULL,            false),
    ENTRY(CTRL_N,  NULL,            parse_flag1,     NULL,            false),
    ENTRY(CTRL_O,  NULL,            parse_X,         exec_ctrl_O,     false),
    ENTRY(CTRL_P,  parse_oper,      scan_ctrl_P,     NULL,            false),
    ENTRY(CTRL_Q,  parse_oper,      scan_ctrl_Q,     NULL,            false),
    ENTRY(CTRL_R,  NULL,            parse_n,         exec_ctrl_R,     false),
    ENTRY(CTRL_S,  parse_oper,      scan_ctrl_S,     NULL,            false),
    ENTRY(CTRL_T,  NULL,            parse_Mc,        exec_ctrl_T,     false),
    ENTRY(CTRL_U,  NULL,            parse_Mcq1,      exec_ctrl_U,     false),
    ENTRY(CTRL_V,  NULL,            parse_n,         exec_ctrl_V,     false),
    ENTRY(CTRL_W,  NULL,            parse_n,         exec_ctrl_W,     false),
    ENTRY(CTRL_X,  NULL,            parse_flag1,     exec_ctrl_X,     false),
    ENTRY(CTRL_Y,  parse_oper,      scan_ctrl_Y,     NULL,            false),
    ENTRY(CTRL_Z,  parse_oper,      scan_ctrl_Z,     NULL,            false),
    ENTRY(ESC,     NULL,            parse_escape,    exec_escape,     true),
    ENTRY('\x1C',  NULL,            scan_bad,        NULL,            false),
    ENTRY('\x1D',  NULL,            scan_bad,        NULL,            false),
    ENTRY('\x1E',  NULL,            scan_ctrl_up,    NULL,            false),
    ENTRY('\x1F',  parse_oper,      scan_ctrl_ubar,  NULL,            false),
    ENTRY(SPACE,   NULL,            scan_nop,        NULL,            false),
    ENTRY('!',     NULL,            scan_bang,       exec_bang,       true),
    ENTRY('"',     parse_n,         scan_quote,      exec_quote,      false),
    ENTRY('#',     parse_oper,      scan_oper,       NULL,            false),
    ENTRY('$',     NULL,            scan_bad,        NULL,            false),
    ENTRY('%',     parse_ncq,       scan_pct,        exec_pct,        false),
    ENTRY('&',     parse_oper,      scan_oper,       NULL,            false),
    ENTRY('\'',    NULL,            parse_escape,    exec_apos,       false),
    ENTRY('(',     parse_oper,      scan_lparen,     NULL,            false),
    ENTRY(')',     parse_oper,      scan_rparen,     NULL,            false),
    ENTRY('*',     parse_oper,      scan_oper,       NULL,            false),
    ENTRY('+',     parse_oper,      scan_oper,       NULL,            false),
    ENTRY(',',     parse_oper,      scan_comma,      NULL,            false),
    ENTRY('-',     parse_oper,      scan_oper,       NULL,            false),
    ENTRY('.',     parse_oper,      scan_dot,        NULL,            false),
    ENTRY('/',     parse_oper,      scan_div,        NULL,            false),
    ENTRY('0',     parse_oper,      scan_number,     NULL,            false),
    ENTRY('1',     parse_oper,      scan_number,     NULL,            false),
    ENTRY('2',     parse_oper,      scan_number,     NULL,            false),
    ENTRY('3',     parse_oper,      scan_number,     NULL,            false),
    ENTRY('4',     parse_oper,      scan_number,     NULL,            false),
    ENTRY('5',     parse_oper,      scan_number,     NULL,            false),
    ENTRY('6',     parse_oper,      scan_number,     NULL,            false),
    ENTRY('7',     parse_oper,      scan_number,     NULL,            false),
    ENTRY('8',     parse_oper,      scan_number,     NULL,            false),
    ENTRY('9',     parse_oper,      scan_number,     NULL,            false),
    ENTRY(':',     NULL,            scan_colon,      NULL,            false),
    ENTRY(';',     NULL,            parse_nc,        exec_semi,       false),
    ENTRY('<',     parse_n,         scan_lt,         exec_lt,         false),
    ENTRY('=',     NULL,            scan_equals,     exec_equals,     false),
    ENTRY('>',     parse_X,         scan_gt,         exec_gt,         false),
    ENTRY('?',     NULL,            parse_X,         exec_trace,      false),
    ENTRY('@',     NULL,            scan_atsign,     NULL,            false),
    ENTRY('A',     NULL,            scan_A,          exec_A,          false),
    ENTRY('a',     NULL,            scan_A,          exec_A,          false),
    ENTRY('B',     parse_oper,      scan_B,          NULL,            false),
    ENTRY('b',     parse_oper,      scan_B,          NULL,            false),
    ENTRY('C',     NULL,            parse_nc,        exec_C,          false),
    ENTRY('c',     NULL,            parse_nc,        exec_C,          false),
    ENTRY('D',     NULL,            parse_Mc,        exec_D,          false),
    ENTRY('d',     NULL,            parse_Mc,        exec_D,          false),
    ENTRY('E',     NULL,            NULL,            NULL,            false),
    ENTRY('e',     NULL,            NULL,            NULL,            false),
    ENTRY('F',     NULL,            NULL,            NULL,            false),
    ENTRY('f',     NULL,            NULL,            NULL,            false),
    ENTRY('G',     NULL,            parse_ncq,       exec_G,          false),
    ENTRY('g',     NULL,            parse_ncq,       exec_G,          false),
    ENTRY('H',     parse_oper,      scan_H,          NULL,            false),
    ENTRY('h',     parse_oper,      scan_H,          NULL,            false),
    ENTRY('I',     NULL,            parse_M1,        exec_I,          false),
    ENTRY('i',     NULL,            parse_M1,        exec_I,          false),
    ENTRY('J',     NULL,            parse_nc,        exec_J,          false),
    ENTRY('j',     NULL,            parse_nc,        exec_J,          false),
    ENTRY('K',     NULL,            parse_M,         exec_K,          false),
    ENTRY('k',     NULL,            parse_M,         exec_K,          false),
    ENTRY('L',     NULL,            parse_nc,        exec_L,          false),
    ENTRY('l',     NULL,            parse_nc,        exec_L,          false),
    ENTRY('M',     NULL,            parse_mcq,       exec_M,          true),
    ENTRY('m',     NULL,            parse_mcq,       exec_M,          true),
    ENTRY('N',     NULL,            parse_nc1,       exec_N,          false),
    ENTRY('n',     NULL,            parse_nc1,       exec_N,          false),
    ENTRY('O',     NULL,            parse_N1,        exec_O,          false),
    ENTRY('o',     NULL,            parse_N1,        exec_O,          false),
    ENTRY('P',     parse_Mc,        scan_P,          exec_P,          false),
    ENTRY('p',     parse_Mc,        scan_P,          exec_P,          false),
    ENTRY('Q',     parse_ncq,       scan_Q,          NULL,            false),
    ENTRY('q',     parse_ncq,       scan_Q,          NULL,            false),
    ENTRY('R',     NULL,            parse_nc,        exec_R,          false),
    ENTRY('r',     NULL,            parse_nc,        exec_R,          false),
    ENTRY('S',     NULL,            parse_md1,       exec_S,          false),
    ENTRY('s',     NULL,            parse_md1,       exec_S,          false),
    ENTRY('T',     NULL,            parse_Mc,        exec_T,          false),
    ENTRY('t',     NULL,            parse_Mc,        exec_T,          false),
    ENTRY('U',     NULL,            parse_mcq,       exec_U,          false),
    ENTRY('u',     NULL,            parse_mcq,       exec_U,          false),
    ENTRY('V',     NULL,            parse_M,         exec_V,          false),
    ENTRY('v',     NULL,            parse_M,         exec_V,          false),
    ENTRY('W',     parse_mc,        scan_W,          exec_W,          false),
    ENTRY('w',     parse_mc,        scan_W,          exec_W,          false),
    ENTRY('X',     NULL,            parse_Mcq,       exec_X,          false),
    ENTRY('x',     NULL,            parse_Mcq,       exec_X,          false),
    ENTRY('Y',     NULL,            parse_c,         exec_Y,          false),
    ENTRY('y',     NULL,            parse_c,         exec_Y,          false),
    ENTRY('Z',     parse_oper,      scan_Z,          NULL,            false),
    ENTRY('z',     parse_oper,      scan_Z,          NULL,            false),
    ENTRY('[',     NULL,            parse_mq,        exec_lbracket,   true),
    ENTRY('\\',    NULL,            parse_n,         exec_bslash,     false),
    ENTRY(']',     NULL,            parse_mcq,       exec_rbracket,   true),
    ENTRY('^',     NULL,            NULL,            NULL,            false),
    ENTRY('_',     NULL,            parse_nc1,       exec_ubar,       false),
    ENTRY('`',     NULL,            scan_bad,        NULL,            false),
    ENTRY('{',     NULL,            scan_bad,        NULL,            false),
    ENTRY('|',     NULL,            parse_escape,    exec_vbar,       false),
    ENTRY('}',     NULL,            scan_bad,        NULL,            false),
    ENTRY('~',     parse_oper,      scan_tilde,      NULL,            false),
    ENTRY(DEL,     NULL,            scan_bad,        NULL,            false),
};

static const uint cmd_max = countof(cmd_table); ///< Maximum command


///  @var    e_table
///  @brief  Table for all commands starting with E.

static const struct cmd_table e_table[] =
{
    ENTRY('%',     NULL,            parse_cq1,       exec_E_pct,      false),
    ENTRY('1',     NULL,            parse_flag2,     exec_E1,         false),
    ENTRY('2',     NULL,            parse_flag2,     exec_E2,         false),
    ENTRY('3',     NULL,            parse_flag2,     exec_E3,         false),
    ENTRY('4',     NULL,            parse_flag2,     exec_E4,         false),
    ENTRY('A',     NULL,            parse_X,         exec_EA,         false),
    ENTRY('a',     NULL,            parse_X,         exec_EA,         false),
    ENTRY('B',     NULL,            parse_c1,        exec_EB,         false),
    ENTRY('b',     NULL,            parse_c1,        exec_EB,         false),
    ENTRY('C',     NULL,            parse_n,         exec_EC,         false),
    ENTRY('c',     NULL,            parse_n,         exec_EC,         false),
    ENTRY('D',     NULL,            parse_flag2,     exec_ED,         false),
    ENTRY('d',     NULL,            parse_flag2,     exec_ED,         false),
    ENTRY('E',     NULL,            parse_flag1,     exec_EE,         false),
    ENTRY('e',     NULL,            parse_flag1,     exec_EE,         false),
    ENTRY('F',     NULL,            parse_X,         exec_EF,         false),
    ENTRY('f',     NULL,            parse_X,         exec_EF,         false),
    ENTRY('G',     NULL,            parse_d1,        exec_EG,         false),
    ENTRY('g',     NULL,            parse_d1,        exec_EG,         false),
    ENTRY('H',     NULL,            parse_flag2,     exec_EH,         false),
    ENTRY('h',     NULL,            parse_flag2,     exec_EH,         false),
    ENTRY('I',     NULL,            parse_mc1,       exec_EI,         false),
    ENTRY('i',     NULL,            parse_mc1,       exec_EI,         false),
    ENTRY('J',     parse_mc,        scan_EJ,         exec_nop,        false),
    ENTRY('j',     parse_mc,        scan_EJ,         exec_nop,        false),
    ENTRY('K',     NULL,            parse_X,         exec_EK,         false),
    ENTRY('k',     NULL,            parse_X,         exec_EK,         false),
    ENTRY('L',     NULL,            parse_c1,        exec_EL,         false),
    ENTRY('l',     NULL,            parse_c1,        exec_EL,         false),
    ENTRY('M',     NULL,            parse_nq,        exec_EM,         false),
    ENTRY('m',     NULL,            parse_nq,        exec_EM,         false),
    ENTRY('N',     NULL,            parse_c1,        exec_EN,         false),
    ENTRY('n',     NULL,            parse_c1,        exec_EN,         false),
    ENTRY('O',     NULL,            parse_nc,        exec_EO,         false),
    ENTRY('o',     NULL,            parse_nc,        exec_EO,         false),
    ENTRY('P',     NULL,            parse_X,         exec_EP,         false),
    ENTRY('p',     NULL,            parse_X,         exec_EP,         false),
    ENTRY('Q',     NULL,            parse_cq1,       exec_EQ,         false),
    ENTRY('q',     NULL,            parse_cq1,       exec_EQ,         false),
    ENTRY('R',     NULL,            parse_c1,        exec_ER,         false),
    ENTRY('r',     NULL,            parse_c1,        exec_ER,         false),
    ENTRY('S',     NULL,            parse_flag2,     exec_ES,         false),
    ENTRY('s',     NULL,            parse_flag2,     exec_ES,         false),
    ENTRY('T',     NULL,            parse_flag2,     exec_ET,         false),
    ENTRY('t',     NULL,            parse_flag2,     exec_ET,         false),
    ENTRY('U',     NULL,            parse_flag1,     exec_EU,         false),
    ENTRY('u',     NULL,            parse_flag1,     exec_EU,         false),
    ENTRY('V',     NULL,            parse_flag2,     exec_EV,         false),
    ENTRY('v',     NULL,            parse_flag2,     exec_EV,         false),
    ENTRY('W',     NULL,            parse_c1,        exec_EW,         false),
    ENTRY('w',     NULL,            parse_c1,        exec_EW,         false),
    ENTRY('X',     NULL,            parse_X,         exec_EX,         false),
    ENTRY('x',     NULL,            parse_X,         exec_EX,         false),
    ENTRY('Y',     NULL,            parse_c,         exec_EY,         false),
    ENTRY('y',     NULL,            parse_c,         exec_EY,         false),
    ENTRY('_',     NULL,            parse_nc1,       exec_E_ubar,     false),
};

static const uint e_max = countof(e_table); ///< Maximum E command


///  @var    f_table
///  @brief  Table for all commands starting with F.

static const struct cmd_table f_table[] =
{
    ENTRY('\'',    NULL,            parse_escape,    exec_F_apos,     false),
    ENTRY('0',     parse_X,         scan_F0,         exec_nop,        false),
    ENTRY('1',     NULL,            parse_m2,        exec_F1,         false),
    ENTRY('2',     NULL,            parse_m2,        exec_F2,         false),
    ENTRY('3',     NULL,            parse_m2,        exec_F3,         false),
    ENTRY('<',     NULL,            parse_escape,    exec_F_lt,       false),
    ENTRY('>',     NULL,            parse_escape,    exec_F_gt,       false),
    ENTRY('B',     NULL,            parse_Mc1,       exec_FB,         false),
    ENTRY('b',     NULL,            parse_Mc1,       exec_FB,         false),
    ENTRY('C',     NULL,            parse_Mc2,       exec_FC,         false),
    ENTRY('c',     NULL,            parse_Mc2,       exec_FC,         false),
    ENTRY('D',     NULL,            parse_nc1,       exec_FD,         false),
    ENTRY('d',     NULL,            parse_nc1,       exec_FD,         false),
    ENTRY('F',     NULL,            parse_n1,        exec_FF,         false),
    ENTRY('f',     NULL,            parse_n1,        exec_FF,         false),
    ENTRY('H',     parse_X,         scan_FH,         exec_nop,        false),
    ENTRY('h',     parse_X,         scan_FH,         exec_nop,        false),
    ENTRY('K',     NULL,            parse_nc1,       exec_FK,         false),
    ENTRY('k',     NULL,            parse_nc1,       exec_FK,         false),
    ENTRY('L',     NULL,            parse_M,         exec_FL,         false),
    ENTRY('l',     NULL,            parse_M,         exec_FL,         false),
    ENTRY('M',     NULL,            parse_c2,        exec_FM,         false),
    ENTRY('m',     NULL,            parse_c2,        exec_FM,         false),
    ENTRY('N',     NULL,            parse_nc1,       exec_FN,         false),
    ENTRY('n',     NULL,            parse_nc1,       exec_FN,         false),
    ENTRY('Q',     NULL,            parse_cq1,       exec_FQ,         false),
    ENTRY('q',     NULL,            parse_cq1,       exec_FQ,         false),
    ENTRY('R',     NULL,            parse_Mc1,       exec_FR,         false),
    ENTRY('r',     NULL,            parse_Mc1,       exec_FR,         false),
    ENTRY('S',     NULL,            parse_md2,       exec_FS,         false),
    ENTRY('s',     NULL,            parse_md2,       exec_FS,         false),
    ENTRY('U',     NULL,            parse_M,         exec_FU,         false),
    ENTRY('u',     NULL,            parse_M,         exec_FU,         false),
    ENTRY('Z',     parse_X,         scan_FZ,         exec_nop,        false),
    ENTRY('z',     parse_X,         scan_FZ,         exec_nop,        false),
    ENTRY('_',     NULL,            parse_mc2,       exec_F_ubar,     false),
    ENTRY('|',     NULL,            parse_escape,    exec_F_vbar,     false),
};

static const uint f_max = countof(f_table); ///< Maximum F command
