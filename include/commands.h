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
    ENTRY(CTRL_A,  NULL,            scan_ctrl_A,     exec_ctrl_A,     false),
    ENTRY(CTRL_B,  NULL,            scan_ctrl_B,     NULL,            false),
    ENTRY(CTRL_C,  parse_escape,    NULL,            exec_ctrl_C,     false),
    ENTRY(CTRL_D,  NULL,            scan_x,          exec_ctrl_D,     false),
    ENTRY(CTRL_E,  NULL,            scan_flag1,      exec_ctrl_E,     false),
    ENTRY(CTRL_F,  NULL,            scan_bad,        NULL,            false),
    ENTRY(CTRL_G,  NULL,            scan_bad,        NULL,            false),
    ENTRY(CTRL_H,  NULL,            scan_ctrl_H,     NULL,            false),
    ENTRY(CTRL_I,  NULL,            scan_ctrl_I,     exec_ctrl_I,     false),
    ENTRY(LF,      NULL,            scan_nop,        NULL,            false),
    ENTRY(VT,      NULL,            scan_nop,        NULL,            false),
    ENTRY(FF,      NULL,            scan_nop,        NULL,            false),
    ENTRY(CR,      NULL,            scan_nop,        NULL,            false),
    ENTRY(CTRL_N,  NULL,            scan_flag1,      NULL,            false),
    ENTRY(CTRL_O,  NULL,            scan_x,          exec_ctrl_O,     false),
    ENTRY(CTRL_P,  NULL,            scan_ctrl_P,     NULL,            false),
    ENTRY(CTRL_Q,  NULL,            scan_ctrl_Q,     NULL,            false),
    ENTRY(CTRL_R,  NULL,            NULL,            exec_ctrl_R,     false),
    ENTRY(CTRL_S,  NULL,            scan_ctrl_S,     NULL,            false),
    ENTRY(CTRL_T,  NULL,            scan_ctrl_T,     exec_ctrl_T,     false),
    ENTRY(CTRL_U,  NULL,            scan_ctrl_U,     exec_ctrl_U,     false),
    ENTRY(CTRL_V,  NULL,            NULL,            exec_ctrl_V,     false),
    ENTRY(CTRL_W,  NULL,            NULL,            exec_ctrl_W,     false),
    ENTRY(CTRL_X,  NULL,            scan_flag1,      exec_ctrl_X,     false),
    ENTRY(CTRL_Y,  NULL,            scan_ctrl_Y,     NULL,            false),
    ENTRY(CTRL_Z,  NULL,            scan_ctrl_Z,     NULL,            false),
    ENTRY(ESC,     parse_escape,    NULL,            exec_escape,     true),
    ENTRY('\x1C',  NULL,            scan_bad,        NULL,            false),
    ENTRY('\x1D',  NULL,            scan_bad,        NULL,            false),
    ENTRY('\x1E',  NULL,            scan_ctrl_up,    NULL,            false),
    ENTRY('\x1F',  NULL,            scan_ctrl_ubar,  NULL,            false),
    ENTRY(SPACE,   NULL,            scan_nop,        NULL,            false),
    ENTRY('!',     NULL,            scan_bang,       exec_bang,       true),
    ENTRY('"',     NULL,            scan_quote,      exec_quote,      false),
    ENTRY('#',     NULL,            scan_oper,       NULL,            false),
    ENTRY('$',     NULL,            scan_bad,        NULL,            false),
    ENTRY('%',     NULL,            scan_pct,        exec_pct,        false),
    ENTRY('&',     NULL,            scan_oper,       NULL,            false),
    ENTRY('\'',    parse_escape,    NULL,            exec_apos,       false),
    ENTRY('(',     NULL,            scan_lparen,     NULL,            false),
    ENTRY(')',     NULL,            scan_rparen,     NULL,            false),
    ENTRY('*',     NULL,            scan_oper,       NULL,            false),
    ENTRY('+',     NULL,            scan_oper,       NULL,            false),
    ENTRY(',',     NULL,            scan_comma,      NULL,            false),
    ENTRY('-',     NULL,            scan_oper,       NULL,            false),
    ENTRY('.',     NULL,            scan_dot,        NULL,            false),
    ENTRY('/',     NULL,            scan_div,        NULL,            false),
    ENTRY('0',     NULL,            scan_number,     NULL,            false),
    ENTRY('1',     NULL,            scan_number,     NULL,            false),
    ENTRY('2',     NULL,            scan_number,     NULL,            false),
    ENTRY('3',     NULL,            scan_number,     NULL,            false),
    ENTRY('4',     NULL,            scan_number,     NULL,            false),
    ENTRY('5',     NULL,            scan_number,     NULL,            false),
    ENTRY('6',     NULL,            scan_number,     NULL,            false),
    ENTRY('7',     NULL,            scan_number,     NULL,            false),
    ENTRY('8',     NULL,            scan_number,     NULL,            false),
    ENTRY('9',     NULL,            scan_number,     NULL,            false),
    ENTRY(':',     NULL,            scan_colon,      NULL,            false),
    ENTRY(';',     NULL,            scan_semi,       exec_semi,       false),
    ENTRY('<',     NULL,            scan_lt,         exec_lt,         false),
    ENTRY('=',     NULL,            scan_equals,     exec_equals,     false),
    ENTRY('>',     NULL,            scan_gt,         exec_gt,         false),
    ENTRY('?',     NULL,            scan_x,          exec_trace,      false),
    ENTRY('@',     NULL,            scan_atsign,     NULL,            false),
    ENTRY('A',     NULL,            scan_A,          exec_A,          false),
    ENTRY('a',     NULL,            scan_A,          exec_A,          false),
    ENTRY('B',     NULL,            scan_B,          NULL,            false),
    ENTRY('b',     NULL,            scan_B,          NULL,            false),
    ENTRY('C',     NULL,            scan_C,          exec_C,          false),
    ENTRY('c',     NULL,            scan_C,          exec_C,          false),
    ENTRY('D',     NULL,            scan_D,          exec_D,          false),
    ENTRY('d',     NULL,            scan_D,          exec_D,          false),
    ENTRY('E',     NULL,            NULL,            NULL,            false),
    ENTRY('e',     NULL,            NULL,            NULL,            false),
    ENTRY('F',     NULL,            NULL,            NULL,            false),
    ENTRY('f',     NULL,            NULL,            NULL,            false),
    ENTRY('G',     NULL,            scan_G,          exec_G,          false),
    ENTRY('g',     NULL,            scan_G,          exec_G,          false),
    ENTRY('H',     NULL,            scan_H,          NULL,            false),
    ENTRY('h',     NULL,            scan_H,          NULL,            false),
    ENTRY('I',     NULL,            scan_I,          exec_I,          false),
    ENTRY('i',     NULL,            scan_I,          exec_I,          false),
    ENTRY('J',     NULL,            scan_J,          exec_J,          false),
    ENTRY('j',     NULL,            scan_J,          exec_J,          false),
    ENTRY('K',     NULL,            scan_K,          exec_K,          false),
    ENTRY('k',     NULL,            scan_K,          exec_K,          false),
    ENTRY('L',     NULL,            scan_C,          exec_L,          false),
    ENTRY('l',     NULL,            scan_C,          exec_L,          false),
    ENTRY('M',     NULL,            scan_M,          exec_M,          true),
    ENTRY('m',     NULL,            scan_M,          exec_M,          true),
    ENTRY('N',     NULL,            scan_N,          exec_N,          false),
    ENTRY('n',     NULL,            scan_N,          exec_N,          false),
    ENTRY('O',     NULL,            scan_O,          exec_O,          false),
    ENTRY('o',     NULL,            scan_O,          exec_O,          false),
    ENTRY('P',     NULL,            scan_P,          exec_P,          false),
    ENTRY('p',     NULL,            scan_P,          exec_P,          false),
    ENTRY('Q',     NULL,            scan_Q,          NULL,            false),
    ENTRY('q',     NULL,            scan_Q,          NULL,            false),
    ENTRY('R',     NULL,            scan_C,          exec_R,          false),
    ENTRY('r',     NULL,            scan_C,          exec_R,          false),
    ENTRY('S',     NULL,            scan_S,          exec_S,          false),
    ENTRY('s',     NULL,            scan_S,          exec_S,          false),
    ENTRY('T',     NULL,            scan_T,          exec_T,          false),
    ENTRY('t',     NULL,            scan_T,          exec_T,          false),
    ENTRY('U',     NULL,            scan_U,          exec_U,          false),
    ENTRY('u',     NULL,            scan_U,          exec_U,          false),
    ENTRY('V',     NULL,            scan_V,          exec_V,          false),
    ENTRY('v',     NULL,            scan_V,          exec_V,          false),
    ENTRY('W',     parse_mc,        scan_W,          exec_W,          false),
    ENTRY('w',     parse_mc,        scan_W,          exec_W,          false),
    ENTRY('X',     NULL,            scan_X,          exec_X,          false),
    ENTRY('x',     NULL,            scan_X,          exec_X,          false),
    ENTRY('Y',     NULL,            scan_Y,          exec_Y,          false),
    ENTRY('y',     NULL,            scan_Y,          exec_Y,          false),
    ENTRY('Z',     NULL,            scan_Z,          NULL,            false),
    ENTRY('z',     NULL,            scan_Z,          NULL,            false),
    ENTRY('[',     NULL,            scan_lbracket,   exec_lbracket,   true),
    ENTRY('\\',    NULL,            NULL,            exec_bslash,     false),
    ENTRY(']',     NULL,            scan_rbracket,   exec_rbracket,   true),
    ENTRY('^',     NULL,            NULL,            NULL,            false),
    ENTRY('_',     NULL,            scan_ubar,       exec_ubar,       false),
    ENTRY('`',     NULL,            scan_bad,        NULL,            false),
    ENTRY('{',     NULL,            scan_bad,        NULL,            false),
    ENTRY('|',     parse_escape,    NULL,            exec_vbar,       false),
    ENTRY('}',     NULL,            scan_bad,        NULL,            false),
    ENTRY('~',     NULL,            scan_tilde,      NULL,            false),
    ENTRY(DEL,     NULL,            scan_bad,        NULL,            false),
};

static const uint cmd_max = countof(cmd_table); ///< Maximum command


///  @var    e_table
///  @brief  Table for all commands starting with E.

static const struct cmd_table e_table[] =
{
    ENTRY('%',     NULL,            scan_EQ,         exec_E_pct,      false),
    ENTRY('1',     NULL,            scan_flag2,      exec_E1,         false),
    ENTRY('2',     NULL,            scan_flag2,      exec_E2,         false),
    ENTRY('3',     NULL,            scan_flag2,      exec_E3,         false),
    ENTRY('4',     NULL,            scan_flag2,      exec_E4,         false),
    ENTRY('A',     NULL,            scan_x,          exec_EA,         false),
    ENTRY('a',     NULL,            scan_x,          exec_EA,         false),
    ENTRY('B',     NULL,            scan_ER,         exec_EB,         false),
    ENTRY('b',     NULL,            scan_ER,         exec_EB,         false),
    ENTRY('C',     NULL,            NULL,            exec_EC,         false),
    ENTRY('c',     NULL,            NULL,            exec_EC,         false),
    ENTRY('D',     NULL,            scan_flag2,      exec_ED,         false),
    ENTRY('d',     NULL,            scan_flag2,      exec_ED,         false),
    ENTRY('E',     NULL,            scan_flag1,      exec_EE,         false),
    ENTRY('e',     NULL,            scan_flag1,      exec_EE,         false),
    ENTRY('F',     NULL,            scan_x,          exec_EF,         false),
    ENTRY('f',     NULL,            scan_x,          exec_EF,         false),
    ENTRY('G',     NULL,            scan_ER,         exec_EG,         false),
    ENTRY('g',     NULL,            scan_ER,         exec_EG,         false),
    ENTRY('H',     NULL,            scan_flag2,      exec_EH,         false),
    ENTRY('h',     NULL,            scan_flag2,      exec_EH,         false),
    ENTRY('I',     NULL,            scan_EI,         exec_EI,         false),
    ENTRY('i',     NULL,            scan_EI,         exec_EI,         false),
    ENTRY('J',     parse_mc,        scan_EJ,         exec_nop,        false),
    ENTRY('j',     parse_mc,        scan_EJ,         exec_nop,        false),
    ENTRY('K',     NULL,            scan_x,          exec_EK,         false),
    ENTRY('k',     NULL,            scan_x,          exec_EK,         false),
    ENTRY('L',     NULL,            scan_EL,         exec_EL,         false),
    ENTRY('l',     NULL,            scan_EL,         exec_EL,         false),
    ENTRY('M',     NULL,            scan_EM,         exec_EM,         false),
    ENTRY('m',     NULL,            scan_EM,         exec_EM,         false),
    ENTRY('N',     NULL,            scan_ER,         exec_EN,         false),
    ENTRY('n',     NULL,            scan_ER,         exec_EN,         false),
    ENTRY('O',     NULL,            scan_EO,         exec_EO,         false),
    ENTRY('o',     NULL,            scan_EO,         exec_EO,         false),
    ENTRY('P',     NULL,            scan_x,          exec_EP,         false),
    ENTRY('p',     NULL,            scan_x,          exec_EP,         false),
    ENTRY('Q',     NULL,            scan_EQ,         exec_EQ,         false),
    ENTRY('q',     NULL,            scan_EQ,         exec_EQ,         false),
    ENTRY('R',     NULL,            scan_ER,         exec_ER,         false),
    ENTRY('r',     NULL,            scan_ER,         exec_ER,         false),
    ENTRY('S',     NULL,            scan_flag2,      exec_ES,         false),
    ENTRY('s',     NULL,            scan_flag2,      exec_ES,         false),
    ENTRY('T',     NULL,            scan_flag2,      exec_ET,         false),
    ENTRY('t',     NULL,            scan_flag2,      exec_ET,         false),
    ENTRY('U',     NULL,            scan_flag1,      exec_EU,         false),
    ENTRY('u',     NULL,            scan_flag1,      exec_EU,         false),
    ENTRY('V',     NULL,            scan_flag2,      exec_EV,         false),
    ENTRY('v',     NULL,            scan_flag2,      exec_EV,         false),
    ENTRY('W',     NULL,            scan_ER,         exec_EW,         false),
    ENTRY('w',     NULL,            scan_ER,         exec_EW,         false),
    ENTRY('X',     NULL,            scan_x,          exec_EX,         false),
    ENTRY('x',     NULL,            scan_x,          exec_EX,         false),
    ENTRY('Y',     NULL,            scan_Y,          exec_EY,         false),
    ENTRY('y',     NULL,            scan_Y,          exec_EY,         false),
    ENTRY('Z',     NULL,            scan_ER,         exec_EZ,         false),
    ENTRY('z',     NULL,            scan_ER,         exec_EZ,         false),
    ENTRY('_',     NULL,            scan_E_ubar,     exec_E_ubar,     false),
};

static const uint e_max = countof(e_table); ///< Maximum E command


///  @var    f_table
///  @brief  Table for all commands starting with F.

static const struct cmd_table f_table[] =
{
    ENTRY('\'',    parse_escape,    NULL,            exec_F_apos,     false),
    ENTRY('0',     NULL,            scan_F0,         exec_nop,        false),
    ENTRY('1',     NULL,            scan_F1,         exec_F1,         false),
    ENTRY('2',     NULL,            scan_F1,         exec_F2,         false),
    ENTRY('3',     NULL,            scan_F1,         exec_F3,         false),
    ENTRY('<',     parse_escape,    NULL,            exec_F_lt,       false),
    ENTRY('>',     parse_escape,    NULL,            exec_F_gt,       false),
    ENTRY('B',     NULL,            scan_FB,         exec_FB,         false),
    ENTRY('b',     NULL,            scan_FB,         exec_FB,         false),
    ENTRY('C',     NULL,            scan_FC,         exec_FC,         false),
    ENTRY('c',     NULL,            scan_FC,         exec_FC,         false),
    ENTRY('D',     NULL,            scan_FD,         exec_FD,         false),
    ENTRY('d',     NULL,            scan_FD,         exec_FD,         false),
    ENTRY('F',     NULL,            scan_FF,         exec_FF,         false),
    ENTRY('f',     NULL,            scan_FF,         exec_FF,         false),
    ENTRY('H',     NULL,            scan_FH,         exec_nop,        false),
    ENTRY('h',     NULL,            scan_FH,         exec_nop,        false),
    ENTRY('K',     NULL,            scan_FK,         exec_FK,         false),
    ENTRY('k',     NULL,            scan_FK,         exec_FK,         false),
    ENTRY('L',     NULL,            scan_FL,         exec_FL,         false),
    ENTRY('l',     NULL,            scan_FL,         exec_FL,         false),
    ENTRY('M',     NULL,            scan_FM,         exec_FM,         false),
    ENTRY('m',     NULL,            scan_FM,         exec_FM,         false),
    ENTRY('N',     NULL,            scan_FN,         exec_FN,         false),
    ENTRY('n',     NULL,            scan_FN,         exec_FN,         false),
    ENTRY('Q',     NULL,            scan_EQ,         exec_FQ,         false),
    ENTRY('q',     NULL,            scan_EQ,         exec_FQ,         false),
    ENTRY('R',     NULL,            scan_FR,         exec_FR,         false),
    ENTRY('r',     NULL,            scan_FR,         exec_FR,         false),
    ENTRY('S',     NULL,            scan_FS,         exec_FS,         false),
    ENTRY('s',     NULL,            scan_FS,         exec_FS,         false),
    ENTRY('U',     NULL,            scan_FL,         exec_FU,         false),
    ENTRY('u',     NULL,            scan_FL,         exec_FU,         false),
    ENTRY('Z',     NULL,            scan_FZ,         exec_nop,        false),
    ENTRY('z',     NULL,            scan_FZ,         exec_nop,        false),
    ENTRY('_',     NULL,            scan_F_ubar,     exec_F_ubar,     false),
    ENTRY('|',     parse_escape,    NULL,            exec_F_vbar,     false),
};

static const uint f_max = countof(f_table); ///< Maximum F command
