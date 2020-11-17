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
    scan_func *scan;                ///< Scanning function
    exec_func *exec;                ///< Execution function

#if     defined(TECO_TRACE)

    const char *scan_name;          ///< Scanning function name
    const char *exec_name;          ///< Execution function name

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

#define ENTRY(chr, scan, exec) [chr] = { scan, exec, #scan, #exec }

#else

#define ENTRY(chr, scan, exec) [chr] = { scan, exec }

#endif

static const struct cmd_table cmd_table[] =
{
    ENTRY(NUL,     scan_bad,        NULL           ),
    ENTRY(CTRL_A,  scan_fmt_mca1,   exec_ctrl_A    ),
    ENTRY(CTRL_B,  scan_ctrl_B,     NULL           ),
    ENTRY(CTRL_C,  scan_nop,        exec_ctrl_C    ),
    ENTRY(CTRL_D,  scan_nop,        exec_ctrl_D    ),
    ENTRY(CTRL_E,  scan_ctrl_E,     exec_ctrl_E    ),
    ENTRY(CTRL_F,  scan_bad,        NULL           ),
    ENTRY(CTRL_G,  scan_bad,        NULL           ),
    ENTRY(CTRL_H,  scan_ctrl_H,     NULL           ),
    ENTRY(CTRL_I,  scan_fmt_a1,     exec_ctrl_I    ),
    ENTRY(LF,      scan_nop,        NULL           ),
    ENTRY(VT,      scan_nop,        NULL           ),
    ENTRY(FF,      scan_nop,        NULL           ),
    ENTRY(CR,      scan_nop,        NULL           ),
    ENTRY(CTRL_N,  scan_ctrl_N,     NULL           ),
    ENTRY(CTRL_O,  scan_nop,        exec_ctrl_O    ),
    ENTRY(CTRL_P,  scan_ctrl_P,     NULL           ),
    ENTRY(CTRL_Q,  scan_ctrl_Q,     NULL           ),
    ENTRY(CTRL_R,  scan_fmt_n,      exec_ctrl_R    ),
    ENTRY(CTRL_S,  scan_ctrl_S,     NULL           ),
    ENTRY(CTRL_T,  scan_fmt_mc,     exec_ctrl_T    ),
    ENTRY(CTRL_U,  scan_fmt_mcaq1,  exec_ctrl_U    ),
    ENTRY(CTRL_V,  scan_fmt_n,      exec_ctrl_V    ),
    ENTRY(CTRL_W,  scan_fmt_n,      exec_ctrl_W    ),
    ENTRY(CTRL_X,  scan_ctrl_X,     exec_ctrl_X    ),
    ENTRY(CTRL_Y,  scan_ctrl_Y,     NULL           ),
    ENTRY(CTRL_Z,  scan_ctrl_Z,     NULL           ),
    ENTRY(ESC,     scan_nop,        exec_escape    ),
    ENTRY('\x1C',  scan_bad,        NULL           ),
    ENTRY('\x1D',  scan_bad,        NULL           ),
    ENTRY('\x1E',  scan_ctrl_up,    NULL           ),
    ENTRY('\x1F',  scan_ctrl_ubar,  NULL           ),
    ENTRY(SPACE,   scan_nop,        NULL           ),
    ENTRY('!',     scan_bang,       exec_bang      ),
    ENTRY('"',     scan_quote,      exec_quote     ),
    ENTRY('#',     scan_oper,       NULL           ),
    ENTRY('$',     scan_bad,        NULL           ),
    ENTRY('%',     scan_pct,        exec_pct       ),
    ENTRY('&',     scan_oper,       NULL           ),
    ENTRY('\'',    scan_nop,        exec_apos      ),
    ENTRY('(',     scan_lparen,     NULL           ),
    ENTRY(')',     scan_rparen,     NULL           ),
    ENTRY('*',     scan_oper,       NULL           ),
    ENTRY('+',     scan_oper,       NULL           ),
    ENTRY(',',     scan_comma,      NULL           ),
    ENTRY('-',     scan_oper,       NULL           ),
    ENTRY('.',     scan_dot,        NULL           ),
    ENTRY('/',     scan_div,        NULL           ),
    ENTRY('0',     scan_number,     NULL           ),
    ENTRY('1',     scan_number,     NULL           ),
    ENTRY('2',     scan_number,     NULL           ),
    ENTRY('3',     scan_number,     NULL           ),
    ENTRY('4',     scan_number,     NULL           ),
    ENTRY('5',     scan_number,     NULL           ),
    ENTRY('6',     scan_number,     NULL           ),
    ENTRY('7',     scan_number,     NULL           ),
    ENTRY('8',     scan_number,     NULL           ),
    ENTRY('9',     scan_number,     NULL           ),
    ENTRY(':',     scan_colon,      NULL           ),
    ENTRY(';',     scan_fmt_n,      exec_semi      ),
    ENTRY('<',     scan_lt,         exec_lt        ),
    ENTRY('=',     scan_equals,     exec_equals    ),
    ENTRY('>',     scan_gt,         exec_gt        ),
    ENTRY('?',     scan_nop,        exec_trace     ),
    ENTRY('@',     scan_atsign,     NULL           ),
    ENTRY('A',     scan_A,          exec_A         ),
    ENTRY('a',     scan_A,          exec_A         ),
    ENTRY('B',     scan_B,          NULL           ),
    ENTRY('b',     scan_B,          NULL           ),
    ENTRY('C',     scan_fmt_nc,     exec_C         ),
    ENTRY('c',     scan_fmt_nc,     exec_C         ),
    ENTRY('D',     scan_fmt_mc,     exec_D         ),
    ENTRY('d',     scan_fmt_mc,     exec_D         ),
    ENTRY('E',     scan_nop,        NULL           ),
    ENTRY('e',     scan_nop,        NULL           ),
    ENTRY('F',     scan_nop,        NULL           ),
    ENTRY('f',     scan_nop,        NULL           ),
    ENTRY('G',     scan_G,          exec_G         ),
    ENTRY('g',     scan_G,          exec_G         ),
    ENTRY('H',     scan_H,          NULL           ),
    ENTRY('h',     scan_H,          NULL           ),
    ENTRY('I',     scan_fmt_ma1,    exec_I         ),
    ENTRY('i',     scan_fmt_ma1,    exec_I         ),
    ENTRY('J',     scan_fmt_nc,     exec_J         ),
    ENTRY('j',     scan_fmt_nc,     exec_J         ),
    ENTRY('K',     scan_fmt_m,      exec_K         ),
    ENTRY('k',     scan_fmt_m,      exec_K         ),
    ENTRY('L',     scan_fmt_nc,     exec_L         ),
    ENTRY('l',     scan_fmt_nc,     exec_L         ),
    ENTRY('M',     scan_fmt_mcq,    exec_M         ),
    ENTRY('m',     scan_fmt_mcq,    exec_M         ),
    ENTRY('N',     scan_fmt_nca1,   exec_N         ),
    ENTRY('n',     scan_fmt_nca1,   exec_N         ),
    ENTRY('O',     scan_fmt_na1,    exec_O         ),
    ENTRY('o',     scan_fmt_na1,    exec_O         ),
    ENTRY('P',     scan_P,          exec_P         ),
    ENTRY('p',     scan_P,          exec_P         ),
    ENTRY('Q',     scan_Q,          NULL           ),
    ENTRY('q',     scan_Q,          NULL           ),
    ENTRY('R',     scan_fmt_nc,     exec_R         ),
    ENTRY('r',     scan_fmt_nc,     exec_R         ),
    ENTRY('S',     scan_fmt_mda1,   exec_S         ),
    ENTRY('s',     scan_fmt_mda1,   exec_S         ),
    ENTRY('T',     scan_fmt_mc,     exec_T         ),
    ENTRY('t',     scan_fmt_mc,     exec_T         ),
    ENTRY('U',     scan_fmt_mq,     exec_U         ),
    ENTRY('u',     scan_fmt_mq,     exec_U         ),
    ENTRY('V',     scan_fmt_m,      exec_V         ),
    ENTRY('v',     scan_fmt_m,      exec_V         ),
    ENTRY('W',     scan_fmt_mc,     exec_W         ),
    ENTRY('w',     scan_fmt_mc,     exec_W         ),
    ENTRY('X',     scan_fmt_mcq,    exec_X         ),
    ENTRY('x',     scan_fmt_mcq,    exec_X         ),
    ENTRY('Y',     scan_fmt_c,      exec_Y         ),
    ENTRY('y',     scan_fmt_c,      exec_Y         ),
    ENTRY('Z',     scan_Z,          NULL           ),
    ENTRY('z',     scan_Z,          NULL           ),
    ENTRY('[',     scan_fmt_m,      exec_lbracket  ),
    ENTRY('\\',    scan_fmt_n,      exec_bslash    ),
    ENTRY(']',     scan_fmt_mc,     exec_rbracket  ),
    ENTRY('^',     scan_nop,        NULL           ),
    ENTRY('_',     scan_fmt_nca1,   exec_ubar      ),
    ENTRY('`',     scan_bad,        NULL           ),
    ENTRY('{',     scan_bad,        NULL           ),
    ENTRY('|',     scan_nop,        exec_vbar      ),
    ENTRY('}',     scan_bad,        NULL           ),
    ENTRY('~',     scan_tilde,      NULL           ),
    ENTRY(DEL,     scan_bad,        NULL           ),
};

static const uint cmd_max = countof(cmd_table); ///< Maximum command


///  @var    e_table
///  @brief  Table for all commands starting with E.

static const struct cmd_table e_table[] =
{
    ENTRY('%',     scan_fmt_caq1,   exec_E_pct     ),
    ENTRY('1',     scan_E1,         exec_E1        ),
    ENTRY('2',     scan_E2,         exec_E2        ),
    ENTRY('3',     scan_E3,         exec_E3        ),
    ENTRY('4',     scan_E4,         exec_E4        ),
    ENTRY('A',     scan_nop,        exec_EA        ),
    ENTRY('a',     scan_nop,        exec_EA        ),
    ENTRY('B',     scan_fmt_ca1,    exec_EB        ),
    ENTRY('b',     scan_fmt_ca1,    exec_EB        ),
    ENTRY('C',     scan_fmt_n,      exec_EC        ),
    ENTRY('c',     scan_fmt_n,      exec_EC        ),
    ENTRY('D',     scan_ED,         exec_ED        ),
    ENTRY('d',     scan_ED,         exec_ED        ),
    ENTRY('E',     scan_EE,         exec_EE        ),
    ENTRY('e',     scan_EE,         exec_EE        ),
    ENTRY('F',     scan_nop,        exec_EF        ),
    ENTRY('f',     scan_nop,        exec_EF        ),
    ENTRY('G',     scan_fmt_da1,    exec_EG        ),
    ENTRY('g',     scan_fmt_da1,    exec_EG        ),
    ENTRY('H',     scan_EH,         exec_EH        ),
    ENTRY('h',     scan_EH,         exec_EH        ),
    ENTRY('I',     scan_fmt_mca1,   exec_EI        ),
    ENTRY('i',     scan_fmt_mca1,   exec_EI        ),
    ENTRY('J',     scan_EJ,         NULL           ),
    ENTRY('j',     scan_EJ,         NULL           ),
    ENTRY('K',     scan_nop,        exec_EK        ),
    ENTRY('k',     scan_nop,        exec_EK        ),
    ENTRY('L',     scan_fmt_ca1,    exec_EL        ),
    ENTRY('l',     scan_fmt_ca1,    exec_EL        ),
    ENTRY('N',     scan_fmt_ca1,    exec_EN        ),
    ENTRY('n',     scan_fmt_ca1,    exec_EN        ),
    ENTRY('O',     scan_fmt_nc,     exec_EO        ),
    ENTRY('o',     scan_fmt_nc,     exec_EO        ),
    ENTRY('P',     scan_nop,        exec_EP        ),
    ENTRY('p',     scan_nop,        exec_EP        ),
    ENTRY('Q',     scan_fmt_caq1,   exec_EQ        ),
    ENTRY('q',     scan_fmt_caq1,   exec_EQ        ),
    ENTRY('R',     scan_fmt_ca1,    exec_ER        ),
    ENTRY('r',     scan_fmt_ca1,    exec_ER        ),
    ENTRY('S',     scan_ES,         exec_ES        ),
    ENTRY('s',     scan_ES,         exec_ES        ),
    ENTRY('T',     scan_ET,         exec_ET        ),
    ENTRY('t',     scan_ET,         exec_ET        ),
    ENTRY('U',     scan_EU,         exec_EU        ),
    ENTRY('u',     scan_EU,         exec_EU        ),
    ENTRY('V',     scan_EV,         exec_EV        ),
    ENTRY('v',     scan_EV,         exec_EV        ),
    ENTRY('W',     scan_fmt_ca1,    exec_EW        ),
    ENTRY('w',     scan_fmt_ca1,    exec_EW        ),
    ENTRY('X',     scan_nop,        exec_EX        ),
    ENTRY('x',     scan_nop,        exec_EX        ),
    ENTRY('Y',     scan_fmt_c,      exec_EY        ),
    ENTRY('y',     scan_fmt_c,      exec_EY        ),
    ENTRY('_',     scan_fmt_nca1,   exec_E_ubar    ),
};

static const uint e_max = countof(e_table); ///< Maximum E command


///  @var    f_table
///  @brief  Table for all commands starting with F.

static const struct cmd_table f_table[] =
{
    ENTRY('\'',    scan_nop,        exec_F_apos    ),
    ENTRY('0',     scan_F0,         NULL           ),
    ENTRY('1',     scan_fmt_ma2,    exec_F1        ),
    ENTRY('2',     scan_fmt_ma2,    exec_F2        ),
    ENTRY('3',     scan_fmt_ma2,    exec_F3        ),
    ENTRY('<',     scan_nop,        exec_F_lt      ),
    ENTRY('>',     scan_nop,        exec_F_gt      ),
    ENTRY('B',     scan_fmt_mca1,   exec_FB        ),
    ENTRY('b',     scan_fmt_mca1,   exec_FB        ),
    ENTRY('C',     scan_fmt_mca2,   exec_FC        ),
    ENTRY('c',     scan_fmt_mca2,   exec_FC        ),
    ENTRY('D',     scan_fmt_nca1,   exec_FD        ),
    ENTRY('d',     scan_fmt_nca1,   exec_FD        ),
    ENTRY('H',     scan_FH,         NULL           ),
    ENTRY('h',     scan_FH,         NULL           ),
    ENTRY('K',     scan_fmt_nca1,   exec_FK        ),
    ENTRY('k',     scan_fmt_nca1,   exec_FK        ),
    ENTRY('L',     scan_fmt_m,      exec_FL        ),
    ENTRY('l',     scan_fmt_m,      exec_FL        ),
    ENTRY('M',     scan_fmt_a2,     exec_FM        ),
    ENTRY('m',     scan_fmt_a2,     exec_FM        ),
    ENTRY('N',     scan_fmt_mca2,   exec_FN        ),
    ENTRY('n',     scan_fmt_mca2,   exec_FN        ),
    ENTRY('Q',     scan_fmt_aq1,    exec_FQ        ),
    ENTRY('q',     scan_fmt_aq1,    exec_FQ        ),
    ENTRY('R',     scan_fmt_mca1,   exec_FR        ),
    ENTRY('r',     scan_fmt_mca1,   exec_FR        ),
    ENTRY('S',     scan_fmt_mda2,   exec_FS        ),
    ENTRY('s',     scan_fmt_mda2,   exec_FS        ),
    ENTRY('U',     scan_fmt_m,      exec_FU        ),
    ENTRY('u',     scan_fmt_m,      exec_FU        ),
    ENTRY('Z',     scan_FZ,         NULL           ),
    ENTRY('z',     scan_FZ,         NULL           ),
    ENTRY('_',     scan_fmt_mca2,   exec_F_ubar    ),
    ENTRY('|',     scan_nop,        exec_F_vbar    ),
};

static const uint f_max = countof(f_table); ///< Maximum F command
