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

///  @typedef scan_func
///  @brief   Function to scan command.

typedef bool (scan_func)(struct cmd *cmd);

///  @typedef exec_func
///  @brief   Function to execute command.

typedef void (exec_func)(struct cmd *cmd);

///  @struct cmd_table
///
///  @brief  Format of command tables used to scan and execute commands.

struct cmd_table
{
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

#define ENTRY(chr, scan, exec, mn) [chr] = { scan, exec, mn, #scan, #exec }

#else

#define ENTRY(chr, scan, exec, mn) [chr] = { scan, exec, mn }

#endif

static const struct cmd_table cmd_table[] =
{
    ENTRY(NUL,     scan_nop,        NULL,            false),
    ENTRY(CTRL_A,  scan_ctrl_A,     exec_ctrl_A,     false),
    ENTRY(CTRL_B,  scan_ctrl_B,     NULL,            false),
    ENTRY(CTRL_C,  NULL,            exec_ctrl_C,     false),
    ENTRY(CTRL_D,  scan_x,          exec_ctrl_D,     false),
    ENTRY(CTRL_E,  scan_flag1,      exec_ctrl_E,     false),
    ENTRY(CTRL_F,  scan_bad,        NULL,            false),
    ENTRY(CTRL_G,  scan_bad,        NULL,            false),
    ENTRY(CTRL_H,  scan_ctrl_H,     NULL,            false),
    ENTRY(CTRL_I,  scan_ctrl_I,     exec_ctrl_I,     false),
    ENTRY(LF,      scan_nop,        NULL,            false),
    ENTRY(VT,      scan_nop,        NULL,            false),
    ENTRY(FF,      scan_nop,        NULL,            false),
    ENTRY(CR,      scan_nop,        NULL,            false),
    ENTRY(CTRL_N,  scan_flag1,      NULL,            false),
    ENTRY(CTRL_O,  scan_x,          exec_ctrl_O,     false),
    ENTRY(CTRL_P,  scan_ctrl_P,     NULL,            false),
    ENTRY(CTRL_Q,  scan_ctrl_Q,     NULL,            false),
    ENTRY(CTRL_R,  NULL,            exec_ctrl_R,     false),
    ENTRY(CTRL_S,  scan_ctrl_S,     NULL,            false),
    ENTRY(CTRL_T,  scan_ctrl_T,     exec_ctrl_T,     false),
    ENTRY(CTRL_U,  scan_ctrl_U,     exec_ctrl_U,     false),
    ENTRY(CTRL_V,  NULL,            exec_ctrl_V,     false),
    ENTRY(CTRL_W,  NULL,            exec_ctrl_W,     false),
    ENTRY(CTRL_X,  scan_flag1,      exec_ctrl_X,     false),
    ENTRY(CTRL_Y,  scan_ctrl_Y,     NULL,            false),
    ENTRY(CTRL_Z,  scan_ctrl_Z,     NULL,            false),
    ENTRY(ESC,     NULL,            exec_escape,     true),
    ENTRY('\x1C',  scan_bad,        NULL,            false),
    ENTRY('\x1D',  scan_bad,        NULL,            false),
    ENTRY('\x1E',  scan_ctrl_up,    NULL,            false),
    ENTRY('\x1F',  scan_ctrl_ubar,  NULL,            false),
    ENTRY(SPACE,   scan_nop,        NULL,            false),
    ENTRY('!',     scan_bang,       exec_bang,       true),
    ENTRY('"',     scan_quote,      exec_quote,      false),
    ENTRY('#',     scan_oper,       NULL,            false),
    ENTRY('$',     scan_bad,        NULL,            false),
    ENTRY('%',     scan_pct,        exec_pct,        false),
    ENTRY('&',     scan_oper,       NULL,            false),
    ENTRY('\'',    NULL,            exec_apos,       false),
    ENTRY('(',     scan_lparen,     NULL,            false),
    ENTRY(')',     scan_rparen,     NULL,            false),
    ENTRY('*',     scan_oper,       NULL,            false),
    ENTRY('+',     scan_oper,       NULL,            false),
    ENTRY(',',     scan_comma,      NULL,            false),
    ENTRY('-',     scan_oper,       NULL,            false),
    ENTRY('.',     scan_dot,        NULL,            false),
    ENTRY('/',     scan_div,        NULL,            false),
    ENTRY('0',     scan_number,     NULL,            false),
    ENTRY('1',     scan_number,     NULL,            false),
    ENTRY('2',     scan_number,     NULL,            false),
    ENTRY('3',     scan_number,     NULL,            false),
    ENTRY('4',     scan_number,     NULL,            false),
    ENTRY('5',     scan_number,     NULL,            false),
    ENTRY('6',     scan_number,     NULL,            false),
    ENTRY('7',     scan_number,     NULL,            false),
    ENTRY('8',     scan_number,     NULL,            false),
    ENTRY('9',     scan_number,     NULL,            false),
    ENTRY(':',     scan_colon,      NULL,            false),
    ENTRY(';',     scan_semi,       exec_semi,       false),
    ENTRY('<',     scan_lt,         exec_lt,         false),
    ENTRY('=',     scan_equals,     exec_equals,     false),
    ENTRY('>',     scan_gt,         exec_gt,         false),
    ENTRY('?',     scan_x,          exec_trace,      false),
    ENTRY('@',     scan_atsign,     NULL,            false),
    ENTRY('A',     scan_A,          exec_A,          false),
    ENTRY('a',     scan_A,          exec_A,          false),
    ENTRY('B',     scan_B,          NULL,            false),
    ENTRY('b',     scan_B,          NULL,            false),
    ENTRY('C',     scan_C,          exec_C,          false),
    ENTRY('c',     scan_C,          exec_C,          false),
    ENTRY('D',     scan_D,          exec_D,          false),
    ENTRY('d',     scan_D,          exec_D,          false),
    ENTRY('E',     NULL,            NULL,            false),
    ENTRY('e',     NULL,            NULL,            false),
    ENTRY('F',     NULL,            NULL,            false),
    ENTRY('f',     NULL,            NULL,            false),
    ENTRY('G',     scan_G,          exec_G,          false),
    ENTRY('g',     scan_G,          exec_G,          false),
    ENTRY('H',     scan_H,          NULL,            false),
    ENTRY('h',     scan_H,          NULL,            false),
    ENTRY('I',     scan_I,          exec_I,          false),
    ENTRY('i',     scan_I,          exec_I,          false),
    ENTRY('J',     scan_J,          exec_J,          false),
    ENTRY('j',     scan_J,          exec_J,          false),
    ENTRY('K',     scan_K,          exec_K,          false),
    ENTRY('k',     scan_K,          exec_K,          false),
    ENTRY('L',     scan_C,          exec_L,          false),
    ENTRY('l',     scan_C,          exec_L,          false),
    ENTRY('M',     scan_M,          exec_M,          true),
    ENTRY('m',     scan_M,          exec_M,          true),
    ENTRY('N',     scan_N,          exec_N,          false),
    ENTRY('n',     scan_N,          exec_N,          false),
    ENTRY('O',     scan_O,          exec_O,          false),
    ENTRY('o',     scan_O,          exec_O,          false),
    ENTRY('P',     scan_P,          exec_P,          false),
    ENTRY('p',     scan_P,          exec_P,          false),
    ENTRY('Q',     scan_Q,          NULL,            false),
    ENTRY('q',     scan_Q,          NULL,            false),
    ENTRY('R',     scan_C,          exec_R,          false),
    ENTRY('r',     scan_C,          exec_R,          false),
    ENTRY('S',     scan_S,          exec_S,          false),
    ENTRY('s',     scan_S,          exec_S,          false),
    ENTRY('T',     scan_T,          exec_T,          false),
    ENTRY('t',     scan_T,          exec_T,          false),
    ENTRY('U',     scan_U,          exec_U,          false),
    ENTRY('u',     scan_U,          exec_U,          false),
    ENTRY('V',     scan_V,          exec_V,          false),
    ENTRY('v',     scan_V,          exec_V,          false),
    ENTRY('W',     scan_W,          exec_W,          false),
    ENTRY('w',     scan_W,          exec_W,          false),
    ENTRY('X',     scan_X,          exec_X,          false),
    ENTRY('x',     scan_X,          exec_X,          false),
    ENTRY('Y',     scan_Y,          exec_Y,          false),
    ENTRY('y',     scan_Y,          exec_Y,          false),
    ENTRY('Z',     scan_Z,          NULL,            false),
    ENTRY('z',     scan_Z,          NULL,            false),
    ENTRY('[',     scan_lbracket,   exec_lbracket,   true),
    ENTRY('\\',    NULL,            exec_bslash,     false),
    ENTRY(']',     scan_rbracket,   exec_rbracket,   true),
    ENTRY('^',     NULL,            NULL,            false),
    ENTRY('_',     scan_ubar,       exec_ubar,       false),
    ENTRY('`',     scan_bad,        NULL,            false),
    ENTRY('{',     scan_bad,        NULL,            false),
    ENTRY('|',     NULL,            exec_vbar,       false),
    ENTRY('}',     scan_bad,        NULL,            false),
    ENTRY('~',     scan_tilde,      NULL,            false),
    ENTRY(DEL,     scan_bad,        NULL,            false),
};

static const uint cmd_max = countof(cmd_table); ///< Maximum command


///  @var    e_table
///  @brief  Table for all commands starting with E.

static const struct cmd_table e_table[] =
{
    ENTRY('%',     scan_EQ,         exec_E_pct,      false),
    ENTRY('1',     scan_flag2,      exec_E1,         false),
    ENTRY('2',     scan_flag2,      exec_E2,         false),
    ENTRY('3',     scan_flag2,      exec_E3,         false),
    ENTRY('4',     scan_flag2,      exec_E4,         false),
    ENTRY('A',     scan_x,          exec_EA,         false),
    ENTRY('a',     scan_x,          exec_EA,         false),
    ENTRY('B',     scan_ER,         exec_EB,         false),
    ENTRY('b',     scan_ER,         exec_EB,         false),
    ENTRY('C',     NULL,            exec_EC,         false),
    ENTRY('c',     NULL,            exec_EC,         false),
    ENTRY('D',     scan_flag2,      exec_ED,         false),
    ENTRY('d',     scan_flag2,      exec_ED,         false),
    ENTRY('E',     scan_flag1,      exec_EE,         false),
    ENTRY('e',     scan_flag1,      exec_EE,         false),
    ENTRY('F',     scan_x,          exec_EF,         false),
    ENTRY('f',     scan_x,          exec_EF,         false),
    ENTRY('G',     scan_ER,         exec_EG,         false),
    ENTRY('g',     scan_ER,         exec_EG,         false),
    ENTRY('H',     scan_flag2,      exec_EH,         false),
    ENTRY('h',     scan_flag2,      exec_EH,         false),
    ENTRY('I',     scan_EI,         exec_EI,         false),
    ENTRY('i',     scan_EI,         exec_EI,         false),
    ENTRY('J',     scan_EJ,         exec_nop,        false),
    ENTRY('j',     scan_EJ,         exec_nop,        false),
    ENTRY('K',     scan_x,          exec_EK,         false),
    ENTRY('k',     scan_x,          exec_EK,         false),
    ENTRY('L',     scan_EL,         exec_EL,         false),
    ENTRY('l',     scan_EL,         exec_EL,         false),
    ENTRY('M',     scan_EM,         exec_EM,         false),
    ENTRY('m',     scan_EM,         exec_EM,         false),
    ENTRY('N',     scan_ER,         exec_EN,         false),
    ENTRY('n',     scan_ER,         exec_EN,         false),
    ENTRY('O',     scan_EO,         exec_EO,         false),
    ENTRY('o',     scan_EO,         exec_EO,         false),
    ENTRY('P',     scan_x,          exec_EP,         false),
    ENTRY('p',     scan_x,          exec_EP,         false),
    ENTRY('Q',     scan_EQ,         exec_EQ,         false),
    ENTRY('q',     scan_EQ,         exec_EQ,         false),
    ENTRY('R',     scan_ER,         exec_ER,         false),
    ENTRY('r',     scan_ER,         exec_ER,         false),
    ENTRY('S',     scan_flag2,      exec_ES,         false),
    ENTRY('s',     scan_flag2,      exec_ES,         false),
    ENTRY('T',     scan_flag2,      exec_ET,         false),
    ENTRY('t',     scan_flag2,      exec_ET,         false),
    ENTRY('U',     scan_flag1,      exec_EU,         false),
    ENTRY('u',     scan_flag1,      exec_EU,         false),
    ENTRY('V',     scan_flag2,      exec_EV,         false),
    ENTRY('v',     scan_flag2,      exec_EV,         false),
    ENTRY('W',     scan_ER,         exec_EW,         false),
    ENTRY('w',     scan_ER,         exec_EW,         false),
    ENTRY('X',     scan_x,          exec_EX,         false),
    ENTRY('x',     scan_x,          exec_EX,         false),
    ENTRY('Y',     scan_Y,          exec_EY,         false),
    ENTRY('y',     scan_Y,          exec_EY,         false),
    ENTRY('Z',     scan_ER,         exec_EZ,         false),
    ENTRY('z',     scan_ER,         exec_EZ,         false),
    ENTRY('_',     scan_E_ubar,     exec_E_ubar,     false),
};

static const uint e_max = countof(e_table); ///< Maximum E command


///  @var    f_table
///  @brief  Table for all commands starting with F.

static const struct cmd_table f_table[] =
{
    ENTRY('\'',    NULL,            exec_F_apos,     false),
    ENTRY('0',     scan_F0,         exec_nop,        false),
    ENTRY('1',     scan_F1,         exec_F1,         false),
    ENTRY('2',     scan_F1,         exec_F2,         false),
    ENTRY('3',     scan_F1,         exec_F3,         false),
    ENTRY('<',     NULL,            exec_F_lt,       false),
    ENTRY('>',     NULL,            exec_F_gt,       false),
    ENTRY('B',     scan_FB,         exec_FB,         false),
    ENTRY('b',     scan_FB,         exec_FB,         false),
    ENTRY('C',     scan_FC,         exec_FC,         false),
    ENTRY('c',     scan_FC,         exec_FC,         false),
    ENTRY('D',     scan_FD,         exec_FD,         false),
    ENTRY('d',     scan_FD,         exec_FD,         false),
    ENTRY('F',     scan_FF,         exec_FF,         false),
    ENTRY('f',     scan_FF,         exec_FF,         false),
    ENTRY('H',     scan_FH,         exec_nop,        false),
    ENTRY('h',     scan_FH,         exec_nop,        false),
    ENTRY('K',     scan_FK,         exec_FK,         false),
    ENTRY('k',     scan_FK,         exec_FK,         false),
    ENTRY('L',     scan_case,       exec_FL,         false),
    ENTRY('l',     scan_case,       exec_FL,         false),
    ENTRY('M',     scan_FM,         exec_FM,         false),
    ENTRY('m',     scan_FM,         exec_FM,         false),
    ENTRY('N',     scan_FN,         exec_FN,         false),
    ENTRY('n',     scan_FN,         exec_FN,         false),
    ENTRY('Q',     scan_EQ,         exec_FQ,         false),
    ENTRY('q',     scan_EQ,         exec_FQ,         false),
    ENTRY('R',     scan_FR,         exec_FR,         false),
    ENTRY('r',     scan_FR,         exec_FR,         false),
    ENTRY('S',     scan_FS,         exec_FS,         false),
    ENTRY('s',     scan_FS,         exec_FS,         false),
    ENTRY('U',     scan_case,       exec_FU,         false),
    ENTRY('u',     scan_case,       exec_FU,         false),
    ENTRY('Z',     scan_FZ,         exec_nop,        false),
    ENTRY('z',     scan_FZ,         exec_nop,        false),
    ENTRY('_',     scan_F_ubar,     exec_F_ubar,     false),
    ENTRY('|',     NULL,            exec_F_vbar,     false),
};

static const uint f_max = countof(f_table); ///< Maximum F command
