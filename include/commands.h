///
///  @file    commands.h
///  @brief   Data tables used in executing command strings.
///
///  *** Automatically generated file. DO NOT MODIFY. ***
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

#define ENTRY(chr, parse, scan, exec) [chr] = { parse, scan, exec, #scan, #exec }

#else

#define ENTRY(chr, parse, scan, exec) [chr] = { parse, scan, exec }

#endif

static const struct cmd_table cmd_table[] =
{
    ENTRY(NUL,     NULL,            scan_bad,        NULL           ),
    ENTRY(CTRL_A,  NULL,            parse_c1,        exec_ctrl_A    ),
    ENTRY(CTRL_B,  parse_oper,      scan_ctrl_B,     NULL           ),
    ENTRY(CTRL_C,  NULL,            parse_escape,    exec_ctrl_C    ),
    ENTRY(CTRL_D,  NULL,            parse_X,         exec_ctrl_D    ),
    ENTRY(CTRL_E,  NULL,            parse_flag1,     exec_ctrl_E    ),
    ENTRY(CTRL_F,  NULL,            scan_bad,        NULL           ),
    ENTRY(CTRL_G,  NULL,            scan_bad,        NULL           ),
    ENTRY(CTRL_H,  parse_oper,      scan_ctrl_H,     NULL           ),
    ENTRY(CTRL_I,  NULL,            parse_1,         exec_ctrl_I    ),
    ENTRY(LF,      NULL,            scan_nop,        NULL           ),
    ENTRY(VT,      NULL,            scan_nop,        NULL           ),
    ENTRY(FF,      NULL,            scan_nop,        NULL           ),
    ENTRY(CR,      NULL,            scan_nop,        NULL           ),
    ENTRY(CTRL_N,  NULL,            parse_flag1,     NULL           ),
    ENTRY(CTRL_O,  NULL,            parse_X,         exec_ctrl_O    ),
    ENTRY(CTRL_P,  parse_oper,      scan_ctrl_P,     NULL           ),
    ENTRY(CTRL_Q,  parse_oper,      scan_ctrl_Q,     NULL           ),
    ENTRY(CTRL_R,  NULL,            parse_n,         exec_ctrl_R    ),
    ENTRY(CTRL_S,  parse_oper,      scan_ctrl_S,     NULL           ),
    ENTRY(CTRL_T,  NULL,            parse_Mc,        exec_ctrl_T    ),
    ENTRY(CTRL_U,  NULL,            parse_Mcq1,      exec_ctrl_U    ),
    ENTRY(CTRL_V,  NULL,            parse_n,         exec_ctrl_V    ),
    ENTRY(CTRL_W,  NULL,            parse_n,         exec_ctrl_W    ),
    ENTRY(CTRL_X,  NULL,            parse_flag1,     exec_ctrl_X    ),
    ENTRY(CTRL_Y,  parse_oper,      scan_ctrl_Y,     NULL           ),
    ENTRY(CTRL_Z,  parse_oper,      scan_ctrl_Z,     NULL           ),
    ENTRY(ESC,     NULL,            parse_escape,    exec_escape    ),
    ENTRY('\x1C',  NULL,            scan_bad,        NULL           ),
    ENTRY('\x1D',  NULL,            scan_bad,        NULL           ),
    ENTRY('\x1E',  NULL,            scan_ctrl_up,    NULL           ),
    ENTRY('\x1F',  parse_oper,      scan_ctrl_ubar,  NULL           ),
    ENTRY(SPACE,   NULL,            scan_nop,        NULL           ),
    ENTRY('!',     NULL,            scan_bang,       exec_bang      ),
    ENTRY('"',     parse_n,         scan_quote,      exec_quote     ),
    ENTRY('#',     parse_oper,      scan_oper,       NULL           ),
    ENTRY('$',     NULL,            scan_bad,        NULL           ),
    ENTRY('%',     parse_ncq,       scan_pct,        exec_pct       ),
    ENTRY('&',     parse_oper,      scan_oper,       NULL           ),
    ENTRY('\'',    NULL,            parse_escape,    exec_apos      ),
    ENTRY('(',     parse_oper,      scan_lparen,     NULL           ),
    ENTRY(')',     parse_oper,      scan_rparen,     NULL           ),
    ENTRY('*',     parse_oper,      scan_oper,       NULL           ),
    ENTRY('+',     parse_oper,      scan_oper,       NULL           ),
    ENTRY(',',     parse_oper,      scan_comma,      NULL           ),
    ENTRY('-',     parse_oper,      scan_oper,       NULL           ),
    ENTRY('.',     parse_oper,      scan_dot,        NULL           ),
    ENTRY('/',     parse_oper,      scan_div,        NULL           ),
    ENTRY('0',     parse_oper,      scan_number,     NULL           ),
    ENTRY('1',     parse_oper,      scan_number,     NULL           ),
    ENTRY('2',     parse_oper,      scan_number,     NULL           ),
    ENTRY('3',     parse_oper,      scan_number,     NULL           ),
    ENTRY('4',     parse_oper,      scan_number,     NULL           ),
    ENTRY('5',     parse_oper,      scan_number,     NULL           ),
    ENTRY('6',     parse_oper,      scan_number,     NULL           ),
    ENTRY('7',     parse_oper,      scan_number,     NULL           ),
    ENTRY('8',     parse_oper,      scan_number,     NULL           ),
    ENTRY('9',     parse_oper,      scan_number,     NULL           ),
    ENTRY(':',     NULL,            scan_colon,      NULL           ),
    ENTRY(';',     NULL,            parse_nc,        exec_semi      ),
    ENTRY('<',     parse_n,         scan_lt,         exec_lt        ),
    ENTRY('=',     NULL,            scan_equals,     exec_equals    ),
    ENTRY('>',     parse_X,         scan_gt,         exec_gt        ),
    ENTRY('?',     NULL,            parse_X,         exec_trace     ),
    ENTRY('@',     NULL,            scan_atsign,     NULL           ),
    ENTRY('A',     NULL,            scan_A,          exec_A         ),
    ENTRY('a',     NULL,            scan_A,          exec_A         ),
    ENTRY('B',     parse_oper,      scan_B,          NULL           ),
    ENTRY('b',     parse_oper,      scan_B,          NULL           ),
    ENTRY('C',     NULL,            parse_nc,        exec_C         ),
    ENTRY('c',     NULL,            parse_nc,        exec_C         ),
    ENTRY('D',     NULL,            parse_Mc,        exec_D         ),
    ENTRY('d',     NULL,            parse_Mc,        exec_D         ),
    ENTRY('E',     NULL,            NULL,            NULL           ),
    ENTRY('e',     NULL,            NULL,            NULL           ),
    ENTRY('F',     NULL,            NULL,            NULL           ),
    ENTRY('f',     NULL,            NULL,            NULL           ),
    ENTRY('G',     NULL,            parse_ncq,       exec_G         ),
    ENTRY('g',     NULL,            parse_ncq,       exec_G         ),
    ENTRY('H',     parse_oper,      scan_H,          NULL           ),
    ENTRY('h',     parse_oper,      scan_H,          NULL           ),
    ENTRY('I',     NULL,            parse_M1,        exec_I         ),
    ENTRY('i',     NULL,            parse_M1,        exec_I         ),
    ENTRY('J',     NULL,            parse_nc,        exec_J         ),
    ENTRY('j',     NULL,            parse_nc,        exec_J         ),
    ENTRY('K',     NULL,            parse_M,         exec_K         ),
    ENTRY('k',     NULL,            parse_M,         exec_K         ),
    ENTRY('L',     NULL,            parse_nc,        exec_L         ),
    ENTRY('l',     NULL,            parse_nc,        exec_L         ),
    ENTRY('M',     NULL,            parse_mcq,       exec_M         ),
    ENTRY('m',     NULL,            parse_mcq,       exec_M         ),
    ENTRY('N',     NULL,            parse_nc1,       exec_N         ),
    ENTRY('n',     NULL,            parse_nc1,       exec_N         ),
    ENTRY('O',     NULL,            parse_N1,        exec_O         ),
    ENTRY('o',     NULL,            parse_N1,        exec_O         ),
    ENTRY('P',     parse_Mc,        scan_P,          exec_P         ),
    ENTRY('p',     parse_Mc,        scan_P,          exec_P         ),
    ENTRY('Q',     parse_ncq,       scan_Q,          NULL           ),
    ENTRY('q',     parse_ncq,       scan_Q,          NULL           ),
    ENTRY('R',     NULL,            parse_nc,        exec_R         ),
    ENTRY('r',     NULL,            parse_nc,        exec_R         ),
    ENTRY('S',     NULL,            parse_md1,       exec_S         ),
    ENTRY('s',     NULL,            parse_md1,       exec_S         ),
    ENTRY('T',     NULL,            parse_Mc,        exec_T         ),
    ENTRY('t',     NULL,            parse_Mc,        exec_T         ),
    ENTRY('U',     NULL,            parse_mq,        exec_U         ),
    ENTRY('u',     NULL,            parse_mq,        exec_U         ),
    ENTRY('V',     NULL,            parse_M,         exec_V         ),
    ENTRY('v',     NULL,            parse_M,         exec_V         ),
    ENTRY('W',     parse_mc,        scan_W,          exec_W         ),
    ENTRY('w',     parse_mc,        scan_W,          exec_W         ),
    ENTRY('X',     NULL,            parse_Mcq,       exec_X         ),
    ENTRY('x',     NULL,            parse_Mcq,       exec_X         ),
    ENTRY('Y',     NULL,            parse_c,         exec_Y         ),
    ENTRY('y',     NULL,            parse_c,         exec_Y         ),
    ENTRY('Z',     parse_oper,      scan_Z,          NULL           ),
    ENTRY('z',     parse_oper,      scan_Z,          NULL           ),
    ENTRY('[',     NULL,            parse_mq,        exec_lbracket  ),
    ENTRY('\\',    NULL,            parse_n,         exec_bslash    ),
    ENTRY(']',     NULL,            parse_mcq,       exec_rbracket  ),
    ENTRY('^',     NULL,            NULL,            NULL           ),
    ENTRY('_',     NULL,            parse_nc1,       exec_ubar      ),
    ENTRY('`',     NULL,            scan_bad,        NULL           ),
    ENTRY('{',     NULL,            scan_bad,        NULL           ),
    ENTRY('|',     NULL,            parse_escape,    exec_vbar      ),
    ENTRY('}',     NULL,            scan_bad,        NULL           ),
    ENTRY('~',     parse_oper,      scan_tilde,      NULL           ),
    ENTRY(DEL,     NULL,            scan_bad,        NULL           ),
};

static const uint cmd_max = countof(cmd_table); ///< Maximum command


///  @var    e_table
///  @brief  Table for all commands starting with E.

static const struct cmd_table e_table[] =
{
    ENTRY('%',     NULL,            parse_cq1,       exec_E_pct     ),
    ENTRY('1',     NULL,            parse_flag2,     exec_E1        ),
    ENTRY('2',     NULL,            parse_flag2,     exec_E2        ),
    ENTRY('3',     NULL,            parse_flag2,     exec_E3        ),
    ENTRY('4',     NULL,            parse_flag2,     exec_E4        ),
    ENTRY('A',     NULL,            parse_X,         exec_EA        ),
    ENTRY('a',     NULL,            parse_X,         exec_EA        ),
    ENTRY('B',     NULL,            parse_c1,        exec_EB        ),
    ENTRY('b',     NULL,            parse_c1,        exec_EB        ),
    ENTRY('C',     NULL,            parse_n,         exec_EC        ),
    ENTRY('c',     NULL,            parse_n,         exec_EC        ),
    ENTRY('D',     NULL,            parse_flag2,     exec_ED        ),
    ENTRY('d',     NULL,            parse_flag2,     exec_ED        ),
    ENTRY('E',     NULL,            parse_flag1,     exec_EE        ),
    ENTRY('e',     NULL,            parse_flag1,     exec_EE        ),
    ENTRY('F',     NULL,            parse_X,         exec_EF        ),
    ENTRY('f',     NULL,            parse_X,         exec_EF        ),
    ENTRY('G',     NULL,            parse_d1,        exec_EG        ),
    ENTRY('g',     NULL,            parse_d1,        exec_EG        ),
    ENTRY('H',     NULL,            parse_flag2,     exec_EH        ),
    ENTRY('h',     NULL,            parse_flag2,     exec_EH        ),
    ENTRY('I',     NULL,            parse_mc1,       exec_EI        ),
    ENTRY('i',     NULL,            parse_mc1,       exec_EI        ),
    ENTRY('J',     parse_mc,        scan_EJ,         exec_nop       ),
    ENTRY('j',     parse_mc,        scan_EJ,         exec_nop       ),
    ENTRY('K',     NULL,            parse_X,         exec_EK        ),
    ENTRY('k',     NULL,            parse_X,         exec_EK        ),
    ENTRY('L',     NULL,            parse_c1,        exec_EL        ),
    ENTRY('l',     NULL,            parse_c1,        exec_EL        ),
    ENTRY('M',     NULL,            parse_nq,        exec_EM        ),
    ENTRY('m',     NULL,            parse_nq,        exec_EM        ),
    ENTRY('N',     NULL,            parse_c1,        exec_EN        ),
    ENTRY('n',     NULL,            parse_c1,        exec_EN        ),
    ENTRY('O',     NULL,            parse_nc,        exec_EO        ),
    ENTRY('o',     NULL,            parse_nc,        exec_EO        ),
    ENTRY('P',     NULL,            parse_X,         exec_EP        ),
    ENTRY('p',     NULL,            parse_X,         exec_EP        ),
    ENTRY('Q',     NULL,            parse_cq1,       exec_EQ        ),
    ENTRY('q',     NULL,            parse_cq1,       exec_EQ        ),
    ENTRY('R',     NULL,            parse_c1,        exec_ER        ),
    ENTRY('r',     NULL,            parse_c1,        exec_ER        ),
    ENTRY('S',     NULL,            parse_flag2,     exec_ES        ),
    ENTRY('s',     NULL,            parse_flag2,     exec_ES        ),
    ENTRY('T',     NULL,            parse_flag2,     exec_ET        ),
    ENTRY('t',     NULL,            parse_flag2,     exec_ET        ),
    ENTRY('U',     NULL,            parse_flag1,     exec_EU        ),
    ENTRY('u',     NULL,            parse_flag1,     exec_EU        ),
    ENTRY('V',     NULL,            parse_flag2,     exec_EV        ),
    ENTRY('v',     NULL,            parse_flag2,     exec_EV        ),
    ENTRY('W',     NULL,            parse_c1,        exec_EW        ),
    ENTRY('w',     NULL,            parse_c1,        exec_EW        ),
    ENTRY('X',     NULL,            parse_X,         exec_EX        ),
    ENTRY('x',     NULL,            parse_X,         exec_EX        ),
    ENTRY('Y',     NULL,            parse_c,         exec_EY        ),
    ENTRY('y',     NULL,            parse_c,         exec_EY        ),
    ENTRY('_',     NULL,            parse_nc1,       exec_E_ubar    ),
};

static const uint e_max = countof(e_table); ///< Maximum E command


///  @var    f_table
///  @brief  Table for all commands starting with F.

static const struct cmd_table f_table[] =
{
    ENTRY('\'',    NULL,            parse_escape,    exec_F_apos    ),
    ENTRY('0',     parse_X,         scan_F0,         exec_nop       ),
    ENTRY('1',     NULL,            parse_m2,        exec_F1        ),
    ENTRY('2',     NULL,            parse_m2,        exec_F2        ),
    ENTRY('3',     NULL,            parse_m2,        exec_F3        ),
    ENTRY('<',     NULL,            parse_escape,    exec_F_lt      ),
    ENTRY('>',     NULL,            parse_escape,    exec_F_gt      ),
    ENTRY('B',     NULL,            parse_Mc1,       exec_FB        ),
    ENTRY('b',     NULL,            parse_Mc1,       exec_FB        ),
    ENTRY('C',     NULL,            parse_Mc2,       exec_FC        ),
    ENTRY('c',     NULL,            parse_Mc2,       exec_FC        ),
    ENTRY('D',     NULL,            parse_nc1,       exec_FD        ),
    ENTRY('d',     NULL,            parse_nc1,       exec_FD        ),
    ENTRY('H',     parse_X,         scan_FH,         exec_nop       ),
    ENTRY('h',     parse_X,         scan_FH,         exec_nop       ),
    ENTRY('K',     NULL,            parse_nc1,       exec_FK        ),
    ENTRY('k',     NULL,            parse_nc1,       exec_FK        ),
    ENTRY('L',     NULL,            parse_M,         exec_FL        ),
    ENTRY('l',     NULL,            parse_M,         exec_FL        ),
    ENTRY('M',     NULL,            parse_2,         exec_FM        ),
    ENTRY('m',     NULL,            parse_2,         exec_FM        ),
    ENTRY('N',     NULL,            parse_nc1,       exec_FN        ),
    ENTRY('n',     NULL,            parse_nc1,       exec_FN        ),
    ENTRY('Q',     NULL,            parse_q1,        exec_FQ        ),
    ENTRY('q',     NULL,            parse_q1,        exec_FQ        ),
    ENTRY('R',     NULL,            parse_Mc1,       exec_FR        ),
    ENTRY('r',     NULL,            parse_Mc1,       exec_FR        ),
    ENTRY('S',     NULL,            parse_md2,       exec_FS        ),
    ENTRY('s',     NULL,            parse_md2,       exec_FS        ),
    ENTRY('U',     NULL,            parse_M,         exec_FU        ),
    ENTRY('u',     NULL,            parse_M,         exec_FU        ),
    ENTRY('Z',     parse_X,         scan_FZ,         exec_nop       ),
    ENTRY('z',     parse_X,         scan_FZ,         exec_nop       ),
    ENTRY('_',     NULL,            parse_mc2,       exec_F_ubar    ),
    ENTRY('|',     NULL,            parse_escape,    exec_F_vbar    ),
};

static const uint f_max = countof(f_table); ///< Maximum F command
