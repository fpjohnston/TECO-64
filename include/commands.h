///
///  @file    commands.h
///  @brief   Data tables used in executing command strings.
///
///  *** Automatically generated file. DO NOT MODIFY. ***
///
///  @copyright 2020 Franklin P. Johnston / Nowwith Treble Software
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
    ENTRY(CTRL_A,  parse_c1,        NULL,            exec_ctrl_A    ),
    ENTRY(CTRL_B,  parse_oper,      scan_ctrl_B,     NULL           ),
    ENTRY(CTRL_C,  parse_escape,    NULL,            exec_ctrl_C    ),
    ENTRY(CTRL_D,  parse_X,         NULL,            exec_ctrl_D    ),
    ENTRY(CTRL_E,  parse_flag1,     NULL,            exec_ctrl_E    ),
    ENTRY(CTRL_F,  NULL,            scan_bad,        NULL           ),
    ENTRY(CTRL_G,  NULL,            scan_bad,        NULL           ),
    ENTRY(CTRL_H,  parse_oper,      scan_ctrl_H,     NULL           ),
    ENTRY(CTRL_I,  parse_1,         NULL,            exec_ctrl_I    ),
    ENTRY(LF,      NULL,            scan_nop,        NULL           ),
    ENTRY(VT,      NULL,            scan_nop,        NULL           ),
    ENTRY(FF,      NULL,            scan_nop,        NULL           ),
    ENTRY(CR,      NULL,            scan_nop,        NULL           ),
    ENTRY(CTRL_N,  parse_flag1,     NULL,            NULL           ),
    ENTRY(CTRL_O,  parse_X,         NULL,            exec_ctrl_O    ),
    ENTRY(CTRL_P,  parse_oper,      scan_ctrl_P,     NULL           ),
    ENTRY(CTRL_Q,  parse_oper,      scan_ctrl_Q,     NULL           ),
    ENTRY(CTRL_R,  parse_n,         NULL,            exec_ctrl_R    ),
    ENTRY(CTRL_S,  parse_oper,      scan_ctrl_S,     NULL           ),
    ENTRY(CTRL_T,  parse_Mc,        NULL,            exec_ctrl_T    ),
    ENTRY(CTRL_U,  parse_Mcq1,      NULL,            exec_ctrl_U    ),
    ENTRY(CTRL_V,  parse_n,         NULL,            exec_ctrl_V    ),
    ENTRY(CTRL_W,  parse_n,         NULL,            exec_ctrl_W    ),
    ENTRY(CTRL_X,  parse_flag1,     NULL,            exec_ctrl_X    ),
    ENTRY(CTRL_Y,  parse_oper,      scan_ctrl_Y,     NULL           ),
    ENTRY(CTRL_Z,  parse_oper,      scan_ctrl_Z,     NULL           ),
    ENTRY(ESC,     parse_escape,    NULL,            exec_escape    ),
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
    ENTRY('\'',    parse_escape,    NULL,            exec_apos      ),
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
    ENTRY(';',     parse_nc,        NULL,            exec_semi      ),
    ENTRY('<',     parse_n,         scan_lt,         exec_lt        ),
    ENTRY('=',     NULL,            scan_equals,     exec_equals    ),
    ENTRY('>',     parse_escape,    scan_gt,         exec_gt        ),
    ENTRY('?',     parse_X,         NULL,            exec_trace     ),
    ENTRY('@',     NULL,            scan_atsign,     NULL           ),
    ENTRY('A',     NULL,            scan_A,          exec_A         ),
    ENTRY('a',     NULL,            scan_A,          exec_A         ),
    ENTRY('B',     parse_oper,      scan_B,          NULL           ),
    ENTRY('b',     parse_oper,      scan_B,          NULL           ),
    ENTRY('C',     parse_nc,        NULL,            exec_C         ),
    ENTRY('c',     parse_nc,        NULL,            exec_C         ),
    ENTRY('D',     parse_Mc,        NULL,            exec_D         ),
    ENTRY('d',     parse_Mc,        NULL,            exec_D         ),
    ENTRY('E',     NULL,            NULL,            NULL           ),
    ENTRY('e',     NULL,            NULL,            NULL           ),
    ENTRY('F',     NULL,            NULL,            NULL           ),
    ENTRY('f',     NULL,            NULL,            NULL           ),
    ENTRY('G',     parse_ncq,       scan_G,          exec_G         ),
    ENTRY('g',     parse_ncq,       scan_G,          exec_G         ),
    ENTRY('H',     parse_oper,      scan_H,          NULL           ),
    ENTRY('h',     parse_oper,      scan_H,          NULL           ),
    ENTRY('I',     parse_M1,        NULL,            exec_I         ),
    ENTRY('i',     parse_M1,        NULL,            exec_I         ),
    ENTRY('J',     parse_nc,        NULL,            exec_J         ),
    ENTRY('j',     parse_nc,        NULL,            exec_J         ),
    ENTRY('K',     parse_M,         NULL,            exec_K         ),
    ENTRY('k',     parse_M,         NULL,            exec_K         ),
    ENTRY('L',     parse_nc,        NULL,            exec_L         ),
    ENTRY('l',     parse_nc,        NULL,            exec_L         ),
    ENTRY('M',     parse_mcq,       NULL,            exec_M         ),
    ENTRY('m',     parse_mcq,       NULL,            exec_M         ),
    ENTRY('N',     parse_nc1,       NULL,            exec_N         ),
    ENTRY('n',     parse_nc1,       NULL,            exec_N         ),
    ENTRY('O',     parse_N1,        NULL,            exec_O         ),
    ENTRY('o',     parse_N1,        NULL,            exec_O         ),
    ENTRY('P',     parse_Mc,        scan_P,          exec_P         ),
    ENTRY('p',     parse_Mc,        scan_P,          exec_P         ),
    ENTRY('Q',     parse_ncq,       scan_Q,          NULL           ),
    ENTRY('q',     parse_ncq,       scan_Q,          NULL           ),
    ENTRY('R',     parse_nc,        NULL,            exec_R         ),
    ENTRY('r',     parse_nc,        NULL,            exec_R         ),
    ENTRY('S',     parse_md1,       NULL,            exec_S         ),
    ENTRY('s',     parse_md1,       NULL,            exec_S         ),
    ENTRY('T',     parse_Mc,        NULL,            exec_T         ),
    ENTRY('t',     parse_Mc,        NULL,            exec_T         ),
    ENTRY('U',     parse_mq,        NULL,            exec_U         ),
    ENTRY('u',     parse_mq,        NULL,            exec_U         ),
    ENTRY('V',     parse_M,         NULL,            exec_V         ),
    ENTRY('v',     parse_M,         NULL,            exec_V         ),
    ENTRY('W',     parse_mc,        scan_W,          exec_W         ),
    ENTRY('w',     parse_mc,        scan_W,          exec_W         ),
    ENTRY('X',     parse_Mcq,       NULL,            exec_X         ),
    ENTRY('x',     parse_Mcq,       NULL,            exec_X         ),
    ENTRY('Y',     parse_c,         NULL,            exec_Y         ),
    ENTRY('y',     parse_c,         NULL,            exec_Y         ),
    ENTRY('Z',     parse_oper,      scan_Z,          NULL           ),
    ENTRY('z',     parse_oper,      scan_Z,          NULL           ),
    ENTRY('[',     parse_mq,        NULL,            exec_lbracket  ),
    ENTRY('\\',    parse_n,         NULL,            exec_bslash    ),
    ENTRY(']',     parse_mcq,       NULL,            exec_rbracket  ),
    ENTRY('^',     NULL,            NULL,            NULL           ),
    ENTRY('_',     parse_nc1,       NULL,            exec_ubar      ),
    ENTRY('`',     NULL,            scan_bad,        NULL           ),
    ENTRY('{',     NULL,            scan_bad,        NULL           ),
    ENTRY('|',     parse_escape,    NULL,            exec_vbar      ),
    ENTRY('}',     NULL,            scan_bad,        NULL           ),
    ENTRY('~',     parse_oper,      scan_tilde,      NULL           ),
    ENTRY(DEL,     NULL,            scan_bad,        NULL           ),
};

static const uint cmd_max = countof(cmd_table); ///< Maximum command


///  @var    e_table
///  @brief  Table for all commands starting with E.

static const struct cmd_table e_table[] =
{
    ENTRY('%',     parse_cq1,       NULL,            exec_E_pct     ),
    ENTRY('1',     parse_flag2,     NULL,            exec_E1        ),
    ENTRY('2',     parse_flag2,     NULL,            exec_E2        ),
    ENTRY('3',     parse_flag2,     NULL,            exec_E3        ),
    ENTRY('4',     parse_flag2,     NULL,            exec_E4        ),
    ENTRY('A',     parse_X,         NULL,            exec_EA        ),
    ENTRY('a',     parse_X,         NULL,            exec_EA        ),
    ENTRY('B',     parse_c1,        NULL,            exec_EB        ),
    ENTRY('b',     parse_c1,        NULL,            exec_EB        ),
    ENTRY('C',     parse_n,         NULL,            exec_EC        ),
    ENTRY('c',     parse_n,         NULL,            exec_EC        ),
    ENTRY('D',     parse_flag2,     NULL,            exec_ED        ),
    ENTRY('d',     parse_flag2,     NULL,            exec_ED        ),
    ENTRY('E',     parse_flag1,     NULL,            exec_EE        ),
    ENTRY('e',     parse_flag1,     NULL,            exec_EE        ),
    ENTRY('F',     parse_X,         NULL,            exec_EF        ),
    ENTRY('f',     parse_X,         NULL,            exec_EF        ),
    ENTRY('G',     parse_d1,        NULL,            exec_EG        ),
    ENTRY('g',     parse_d1,        NULL,            exec_EG        ),
    ENTRY('H',     parse_flag2,     NULL,            exec_EH        ),
    ENTRY('h',     parse_flag2,     NULL,            exec_EH        ),
    ENTRY('I',     parse_mc1,       NULL,            exec_EI        ),
    ENTRY('i',     parse_mc1,       NULL,            exec_EI        ),
    ENTRY('J',     parse_mc,        scan_EJ,         exec_nop       ),
    ENTRY('j',     parse_mc,        scan_EJ,         exec_nop       ),
    ENTRY('K',     parse_X,         NULL,            exec_EK        ),
    ENTRY('k',     parse_X,         NULL,            exec_EK        ),
    ENTRY('L',     parse_c1,        NULL,            exec_EL        ),
    ENTRY('l',     parse_c1,        NULL,            exec_EL        ),
    ENTRY('N',     parse_c1,        NULL,            exec_EN        ),
    ENTRY('n',     parse_c1,        NULL,            exec_EN        ),
    ENTRY('O',     parse_nc,        NULL,            exec_EO        ),
    ENTRY('o',     parse_nc,        NULL,            exec_EO        ),
    ENTRY('P',     parse_X,         NULL,            exec_EP        ),
    ENTRY('p',     parse_X,         NULL,            exec_EP        ),
    ENTRY('Q',     parse_cq1,       NULL,            exec_EQ        ),
    ENTRY('q',     parse_cq1,       NULL,            exec_EQ        ),
    ENTRY('R',     parse_c1,        NULL,            exec_ER        ),
    ENTRY('r',     parse_c1,        NULL,            exec_ER        ),
    ENTRY('S',     parse_flag2,     NULL,            exec_ES        ),
    ENTRY('s',     parse_flag2,     NULL,            exec_ES        ),
    ENTRY('T',     parse_flag2,     NULL,            exec_ET        ),
    ENTRY('t',     parse_flag2,     NULL,            exec_ET        ),
    ENTRY('U',     parse_flag1,     NULL,            exec_EU        ),
    ENTRY('u',     parse_flag1,     NULL,            exec_EU        ),
    ENTRY('V',     parse_flag2,     NULL,            exec_EV        ),
    ENTRY('v',     parse_flag2,     NULL,            exec_EV        ),
    ENTRY('W',     parse_c1,        NULL,            exec_EW        ),
    ENTRY('w',     parse_c1,        NULL,            exec_EW        ),
    ENTRY('X',     parse_X,         NULL,            exec_EX        ),
    ENTRY('x',     parse_X,         NULL,            exec_EX        ),
    ENTRY('Y',     parse_c,         NULL,            exec_EY        ),
    ENTRY('y',     parse_c,         NULL,            exec_EY        ),
    ENTRY('_',     parse_nc1,       NULL,            exec_E_ubar    ),
};

static const uint e_max = countof(e_table); ///< Maximum E command


///  @var    f_table
///  @brief  Table for all commands starting with F.

static const struct cmd_table f_table[] =
{
    ENTRY('\'',    parse_escape,    NULL,            exec_F_apos    ),
    ENTRY('0',     parse_X,         scan_F0,         exec_nop       ),
    ENTRY('1',     parse_m2,        NULL,            exec_F1        ),
    ENTRY('2',     parse_m2,        NULL,            exec_F2        ),
    ENTRY('3',     parse_m2,        NULL,            exec_F3        ),
    ENTRY('<',     parse_escape,    NULL,            exec_F_lt      ),
    ENTRY('>',     parse_escape,    NULL,            exec_F_gt      ),
    ENTRY('B',     parse_Mc1,       NULL,            exec_FB        ),
    ENTRY('b',     parse_Mc1,       NULL,            exec_FB        ),
    ENTRY('C',     parse_Mc2,       NULL,            exec_FC        ),
    ENTRY('c',     parse_Mc2,       NULL,            exec_FC        ),
    ENTRY('D',     parse_nc1,       NULL,            exec_FD        ),
    ENTRY('d',     parse_nc1,       NULL,            exec_FD        ),
    ENTRY('H',     parse_X,         scan_FH,         exec_nop       ),
    ENTRY('h',     parse_X,         scan_FH,         exec_nop       ),
    ENTRY('K',     parse_nc1,       NULL,            exec_FK        ),
    ENTRY('k',     parse_nc1,       NULL,            exec_FK        ),
    ENTRY('L',     parse_M,         NULL,            exec_FL        ),
    ENTRY('l',     parse_M,         NULL,            exec_FL        ),
    ENTRY('M',     parse_2,         NULL,            exec_FM        ),
    ENTRY('m',     parse_2,         NULL,            exec_FM        ),
    ENTRY('N',     parse_nc1,       NULL,            exec_FN        ),
    ENTRY('n',     parse_nc1,       NULL,            exec_FN        ),
    ENTRY('Q',     parse_q1,        NULL,            exec_FQ        ),
    ENTRY('q',     parse_q1,        NULL,            exec_FQ        ),
    ENTRY('R',     parse_Mc1,       NULL,            exec_FR        ),
    ENTRY('r',     parse_Mc1,       NULL,            exec_FR        ),
    ENTRY('S',     parse_md2,       NULL,            exec_FS        ),
    ENTRY('s',     parse_md2,       NULL,            exec_FS        ),
    ENTRY('U',     parse_M,         NULL,            exec_FU        ),
    ENTRY('u',     parse_M,         NULL,            exec_FU        ),
    ENTRY('Z',     parse_X,         scan_FZ,         exec_nop       ),
    ENTRY('z',     parse_X,         scan_FZ,         exec_nop       ),
    ENTRY('_',     parse_mc2,       NULL,            exec_F_ubar    ),
    ENTRY('|',     parse_escape,    NULL,            exec_F_vbar    ),
};

static const uint f_max = countof(f_table); ///< Maximum F command
