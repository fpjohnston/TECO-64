///
///  @file    commands.h
///  @brief   Data tables used in executing command strings.
///
///  *** Automatically generated from template file. DO NOT MODIFY. ***
///
///  @copyright 2020-2022 Franklin P. Johnston / Nowwith Treble Software
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

#define NO_ARGS     (bool)false     ///< m & n args are not preserved by command

#define MN_ARGS     (bool)true      ///< m & n args are preserved by command

/// @def    ENTRY(chr, scan, exec)
/// @brief  Defines entries in command table.

#if     defined(TECO_TRACE)

#define ENTRY(chr, scan, exec, mn) [chr] = { scan, exec, mn, #scan, #exec }

#else

#define ENTRY(chr, scan, exec, mn) [chr] = { scan, exec, mn }

#endif

///
///  @var    cmd_table
///
///  @brief  Dispatch table, defining functions to scan and execute commands.
///

static const struct cmd_table cmd_table[] =
{
    ENTRY(NUL,     scan_nop,        NULL,            NO_ARGS),
    ENTRY(CTRL_A,  scan_ctrl_A,     exec_ctrl_A,     NO_ARGS),
    ENTRY(CTRL_B,  scan_ctrl_B,     NULL,            NO_ARGS),
    ENTRY(CTRL_C,  NULL,            exec_ctrl_C,     NO_ARGS),
    ENTRY(CTRL_D,  scan_x,          exec_ctrl_D,     NO_ARGS),
    ENTRY(CTRL_E,  scan_flag1,      exec_ctrl_E,     NO_ARGS),
    ENTRY(CTRL_F,  scan_bad,        NULL,            NO_ARGS),
    ENTRY(CTRL_G,  scan_bad,        NULL,            NO_ARGS),
    ENTRY(CTRL_H,  scan_ctrl_H,     NULL,            NO_ARGS),
    ENTRY(CTRL_I,  scan_ctrl_I,     exec_ctrl_I,     NO_ARGS),
    ENTRY(LF,      scan_nop,        NULL,            NO_ARGS),
    ENTRY(VT,      scan_bad,        NULL,            NO_ARGS),
    ENTRY(FF,      scan_nop,        NULL,            NO_ARGS),
    ENTRY(CR,      scan_nop,        NULL,            NO_ARGS),
    ENTRY(CTRL_N,  scan_flag1,      NULL,            NO_ARGS),
    ENTRY(CTRL_O,  scan_x,          exec_ctrl_O,     NO_ARGS),
    ENTRY(CTRL_P,  scan_ctrl_P,     NULL,            NO_ARGS),
    ENTRY(CTRL_Q,  scan_ctrl_Q,     NULL,            NO_ARGS),
    ENTRY(CTRL_R,  NULL,            exec_ctrl_R,     NO_ARGS),
    ENTRY(CTRL_S,  scan_ctrl_S,     NULL,            NO_ARGS),
    ENTRY(CTRL_T,  scan_ctrl_T,     exec_ctrl_T,     NO_ARGS),
    ENTRY(CTRL_U,  scan_ctrl_U,     exec_ctrl_U,     NO_ARGS),
    ENTRY(CTRL_V,  NULL,            exec_ctrl_V,     NO_ARGS),
    ENTRY(CTRL_W,  NULL,            exec_ctrl_W,     NO_ARGS),
    ENTRY(CTRL_X,  scan_flag1,      exec_ctrl_X,     NO_ARGS),
    ENTRY(CTRL_Y,  scan_ctrl_Y,     NULL,            NO_ARGS),
    ENTRY(CTRL_Z,  scan_ctrl_Z,     NULL,            NO_ARGS),
    ENTRY(ESC,     NULL,            exec_escape,     MN_ARGS),
    ENTRY('\x1C',  scan_bad,        NULL,            NO_ARGS),
    ENTRY('\x1D',  scan_bad,        NULL,            NO_ARGS),
    ENTRY('\x1E',  scan_ctrl_up,    NULL,            NO_ARGS),
    ENTRY('\x1F',  scan_ctrl_ubar,  NULL,            NO_ARGS),
    ENTRY(SPACE,   scan_nop,        NULL,            NO_ARGS),
    ENTRY('!',     scan_bang,       exec_bang,       MN_ARGS),
    ENTRY('"',     scan_quote,      exec_quote,      NO_ARGS),
    ENTRY('#',     scan_oper,       NULL,            NO_ARGS),
    ENTRY('$',     scan_bad,        NULL,            NO_ARGS),
    ENTRY('%',     scan_pct,        exec_pct,        NO_ARGS),
    ENTRY('&',     scan_oper,       NULL,            NO_ARGS),
    ENTRY('\'',    NULL,            exec_apos,       NO_ARGS),
    ENTRY('(',     scan_lparen,     NULL,            NO_ARGS),
    ENTRY(')',     scan_rparen,     NULL,            NO_ARGS),
    ENTRY('*',     scan_oper,       NULL,            NO_ARGS),
    ENTRY('+',     scan_oper,       NULL,            NO_ARGS),
    ENTRY(',',     scan_comma,      NULL,            NO_ARGS),
    ENTRY('-',     scan_oper,       NULL,            NO_ARGS),
    ENTRY('.',     scan_dot,        NULL,            NO_ARGS),
    ENTRY('/',     scan_div,        NULL,            NO_ARGS),
    ENTRY('0',     scan_number,     NULL,            NO_ARGS),
    ENTRY('1',     scan_number,     NULL,            NO_ARGS),
    ENTRY('2',     scan_number,     NULL,            NO_ARGS),
    ENTRY('3',     scan_number,     NULL,            NO_ARGS),
    ENTRY('4',     scan_number,     NULL,            NO_ARGS),
    ENTRY('5',     scan_number,     NULL,            NO_ARGS),
    ENTRY('6',     scan_number,     NULL,            NO_ARGS),
    ENTRY('7',     scan_number,     NULL,            NO_ARGS),
    ENTRY('8',     scan_number,     NULL,            NO_ARGS),
    ENTRY('9',     scan_number,     NULL,            NO_ARGS),
    ENTRY(':',     scan_colon,      NULL,            NO_ARGS),
    ENTRY(';',     scan_semi,       exec_semi,       NO_ARGS),
    ENTRY('<',     scan_lt,         exec_lt,         NO_ARGS),
    ENTRY('=',     scan_equals,     exec_equals,     NO_ARGS),
    ENTRY('>',     scan_gt,         exec_gt,         NO_ARGS),
    ENTRY('?',     scan_x,          exec_trace,      NO_ARGS),
    ENTRY('@',     scan_atsign,     NULL,            NO_ARGS),
    ENTRY('A',     scan_A,          exec_A,          NO_ARGS),
    ENTRY('a',     scan_A,          exec_A,          NO_ARGS),
    ENTRY('B',     scan_B,          NULL,            NO_ARGS),
    ENTRY('b',     scan_B,          NULL,            NO_ARGS),
    ENTRY('C',     scan_C,          exec_C,          NO_ARGS),
    ENTRY('c',     scan_C,          exec_C,          NO_ARGS),
    ENTRY('D',     scan_D,          exec_D,          NO_ARGS),
    ENTRY('d',     scan_D,          exec_D,          NO_ARGS),
    ENTRY('E',     NULL,            NULL,            NO_ARGS),
    ENTRY('e',     NULL,            NULL,            NO_ARGS),
    ENTRY('F',     NULL,            NULL,            NO_ARGS),
    ENTRY('f',     NULL,            NULL,            NO_ARGS),
    ENTRY('G',     scan_G,          exec_G,          NO_ARGS),
    ENTRY('g',     scan_G,          exec_G,          NO_ARGS),
    ENTRY('H',     scan_H,          NULL,            NO_ARGS),
    ENTRY('h',     scan_H,          NULL,            NO_ARGS),
    ENTRY('I',     scan_I,          exec_I,          NO_ARGS),
    ENTRY('i',     scan_I,          exec_I,          NO_ARGS),
    ENTRY('J',     scan_J,          exec_J,          NO_ARGS),
    ENTRY('j',     scan_J,          exec_J,          NO_ARGS),
    ENTRY('K',     scan_K,          exec_K,          NO_ARGS),
    ENTRY('k',     scan_K,          exec_K,          NO_ARGS),
    ENTRY('L',     scan_C,          exec_L,          NO_ARGS),
    ENTRY('l',     scan_C,          exec_L,          NO_ARGS),
    ENTRY('M',     scan_M,          exec_M,          MN_ARGS),
    ENTRY('m',     scan_M,          exec_M,          MN_ARGS),
    ENTRY('N',     scan_N,          exec_N,          NO_ARGS),
    ENTRY('n',     scan_N,          exec_N,          NO_ARGS),
    ENTRY('O',     scan_O,          exec_O,          NO_ARGS),
    ENTRY('o',     scan_O,          exec_O,          NO_ARGS),
    ENTRY('P',     scan_P,          exec_P,          NO_ARGS),
    ENTRY('p',     scan_P,          exec_P,          NO_ARGS),
    ENTRY('Q',     scan_Q,          NULL,            NO_ARGS),
    ENTRY('q',     scan_Q,          NULL,            NO_ARGS),
    ENTRY('R',     scan_C,          exec_R,          NO_ARGS),
    ENTRY('r',     scan_C,          exec_R,          NO_ARGS),
    ENTRY('S',     scan_S,          exec_S,          NO_ARGS),
    ENTRY('s',     scan_S,          exec_S,          NO_ARGS),
    ENTRY('T',     scan_T,          exec_T,          NO_ARGS),
    ENTRY('t',     scan_T,          exec_T,          NO_ARGS),
    ENTRY('U',     scan_U,          exec_U,          NO_ARGS),
    ENTRY('u',     scan_U,          exec_U,          NO_ARGS),
    ENTRY('V',     scan_V,          exec_V,          NO_ARGS),
    ENTRY('v',     scan_V,          exec_V,          NO_ARGS),
    ENTRY('W',     scan_W,          exec_W,          NO_ARGS),
    ENTRY('w',     scan_W,          exec_W,          NO_ARGS),
    ENTRY('X',     scan_X,          exec_X,          NO_ARGS),
    ENTRY('x',     scan_X,          exec_X,          NO_ARGS),
    ENTRY('Y',     scan_Y,          exec_Y,          NO_ARGS),
    ENTRY('y',     scan_Y,          exec_Y,          NO_ARGS),
    ENTRY('Z',     scan_Z,          NULL,            NO_ARGS),
    ENTRY('z',     scan_Z,          NULL,            NO_ARGS),
    ENTRY('[',     scan_lbracket,   exec_lbracket,   MN_ARGS),
    ENTRY('\\',    NULL,            exec_bslash,     NO_ARGS),
    ENTRY(']',     scan_rbracket,   exec_rbracket,   MN_ARGS),
    ENTRY('^',     NULL,            NULL,            NO_ARGS),
    ENTRY('_',     scan_ubar,       exec_ubar,       NO_ARGS),
    ENTRY('`',     scan_bad,        NULL,            NO_ARGS),
    ENTRY('{',     scan_bad,        NULL,            NO_ARGS),
    ENTRY('|',     NULL,            exec_vbar,       NO_ARGS),
    ENTRY('}',     scan_bad,        NULL,            NO_ARGS),
    ENTRY('~',     scan_tilde,      NULL,            NO_ARGS),
    ENTRY(DEL,     scan_bad,        NULL,            NO_ARGS),
};

static const uint cmd_max = countof(cmd_table); ///< Maximum command


///  @var    e_table
///  @brief  Table for all commands starting with E.

static const struct cmd_table e_table[] =
{
    ENTRY('%',     scan_EQ,         exec_E_pct,      NO_ARGS),
    ENTRY('1',     scan_flag2,      exec_E1,         NO_ARGS),
    ENTRY('2',     scan_flag2,      exec_E2,         NO_ARGS),
    ENTRY('3',     scan_flag2,      exec_E3,         NO_ARGS),
    ENTRY('4',     scan_flag2,      exec_E4,         NO_ARGS),
    ENTRY('A',     scan_x,          exec_EA,         NO_ARGS),
    ENTRY('a',     scan_x,          exec_EA,         NO_ARGS),
    ENTRY('B',     scan_ER,         exec_EB,         NO_ARGS),
    ENTRY('b',     scan_ER,         exec_EB,         NO_ARGS),
    ENTRY('C',     NULL,            exec_EC,         NO_ARGS),
    ENTRY('c',     NULL,            exec_EC,         NO_ARGS),
    ENTRY('D',     scan_flag2,      exec_ED,         NO_ARGS),
    ENTRY('d',     scan_flag2,      exec_ED,         NO_ARGS),
    ENTRY('E',     scan_flag1,      exec_EE,         NO_ARGS),
    ENTRY('e',     scan_flag1,      exec_EE,         NO_ARGS),
    ENTRY('F',     scan_x,          exec_EF,         NO_ARGS),
    ENTRY('f',     scan_x,          exec_EF,         NO_ARGS),
    ENTRY('G',     scan_ER,         exec_EG,         NO_ARGS),
    ENTRY('g',     scan_ER,         exec_EG,         NO_ARGS),
    ENTRY('H',     scan_flag2,      exec_EH,         NO_ARGS),
    ENTRY('h',     scan_flag2,      exec_EH,         NO_ARGS),
    ENTRY('I',     scan_EI,         exec_EI,         NO_ARGS),
    ENTRY('i',     scan_EI,         exec_EI,         NO_ARGS),
    ENTRY('J',     scan_EJ,         exec_nop,        NO_ARGS),
    ENTRY('j',     scan_EJ,         exec_nop,        NO_ARGS),
    ENTRY('K',     scan_x,          exec_EK,         NO_ARGS),
    ENTRY('k',     scan_x,          exec_EK,         NO_ARGS),
    ENTRY('L',     scan_EL,         exec_EL,         NO_ARGS),
    ENTRY('l',     scan_EL,         exec_EL,         NO_ARGS),
    ENTRY('M',     scan_EM,         exec_EM,         NO_ARGS),
    ENTRY('m',     scan_EM,         exec_EM,         NO_ARGS),
    ENTRY('N',     scan_ER,         exec_EN,         NO_ARGS),
    ENTRY('n',     scan_ER,         exec_EN,         NO_ARGS),
    ENTRY('O',     scan_EO,         exec_EO,         NO_ARGS),
    ENTRY('o',     scan_EO,         exec_EO,         NO_ARGS),
    ENTRY('P',     scan_x,          exec_EP,         NO_ARGS),
    ENTRY('p',     scan_x,          exec_EP,         NO_ARGS),
    ENTRY('Q',     scan_EQ,         exec_EQ,         NO_ARGS),
    ENTRY('q',     scan_EQ,         exec_EQ,         NO_ARGS),
    ENTRY('R',     scan_ER,         exec_ER,         NO_ARGS),
    ENTRY('r',     scan_ER,         exec_ER,         NO_ARGS),
    ENTRY('S',     scan_flag2,      exec_ES,         NO_ARGS),
    ENTRY('s',     scan_flag2,      exec_ES,         NO_ARGS),
    ENTRY('T',     scan_flag2,      exec_ET,         NO_ARGS),
    ENTRY('t',     scan_flag2,      exec_ET,         NO_ARGS),
    ENTRY('U',     scan_flag1,      exec_EU,         NO_ARGS),
    ENTRY('u',     scan_flag1,      exec_EU,         NO_ARGS),
    ENTRY('V',     scan_flag2,      exec_EV,         NO_ARGS),
    ENTRY('v',     scan_flag2,      exec_EV,         NO_ARGS),
    ENTRY('W',     scan_ER,         exec_EW,         NO_ARGS),
    ENTRY('w',     scan_ER,         exec_EW,         NO_ARGS),
    ENTRY('X',     scan_x,          exec_EX,         NO_ARGS),
    ENTRY('x',     scan_x,          exec_EX,         NO_ARGS),
    ENTRY('Y',     scan_Y,          exec_EY,         NO_ARGS),
    ENTRY('y',     scan_Y,          exec_EY,         NO_ARGS),
    ENTRY('Z',     scan_ER,         exec_EZ,         NO_ARGS),
    ENTRY('z',     scan_ER,         exec_EZ,         NO_ARGS),
    ENTRY('_',     scan_E_ubar,     exec_E_ubar,     NO_ARGS),
};

static const uint e_max = countof(e_table); ///< Maximum E command


///  @var    f_table
///  @brief  Table for all commands starting with F.

static const struct cmd_table f_table[] =
{
    ENTRY('\'',    NULL,            exec_F_apos,     NO_ARGS),
    ENTRY('0',     scan_F0,         exec_nop,        NO_ARGS),
    ENTRY('1',     scan_F1,         exec_F1,         NO_ARGS),
    ENTRY('2',     scan_F1,         exec_F2,         NO_ARGS),
    ENTRY('3',     scan_F1,         exec_F3,         NO_ARGS),
    ENTRY('<',     NULL,            exec_F_lt,       NO_ARGS),
    ENTRY('>',     NULL,            exec_F_gt,       NO_ARGS),
    ENTRY('B',     scan_FB,         exec_FB,         NO_ARGS),
    ENTRY('b',     scan_FB,         exec_FB,         NO_ARGS),
    ENTRY('C',     scan_FC,         exec_FC,         NO_ARGS),
    ENTRY('c',     scan_FC,         exec_FC,         NO_ARGS),
    ENTRY('D',     scan_FD,         exec_FD,         NO_ARGS),
    ENTRY('d',     scan_FD,         exec_FD,         NO_ARGS),
    ENTRY('F',     scan_FF,         exec_FF,         NO_ARGS),
    ENTRY('f',     scan_FF,         exec_FF,         NO_ARGS),
    ENTRY('H',     scan_FH,         exec_nop,        NO_ARGS),
    ENTRY('h',     scan_FH,         exec_nop,        NO_ARGS),
    ENTRY('K',     scan_FK,         exec_FK,         NO_ARGS),
    ENTRY('k',     scan_FK,         exec_FK,         NO_ARGS),
    ENTRY('L',     scan_case,       exec_FL,         NO_ARGS),
    ENTRY('l',     scan_case,       exec_FL,         NO_ARGS),
    ENTRY('M',     scan_FM,         exec_FM,         NO_ARGS),
    ENTRY('m',     scan_FM,         exec_FM,         NO_ARGS),
    ENTRY('N',     scan_FN,         exec_FN,         NO_ARGS),
    ENTRY('n',     scan_FN,         exec_FN,         NO_ARGS),
    ENTRY('Q',     scan_EQ,         exec_FQ,         NO_ARGS),
    ENTRY('q',     scan_EQ,         exec_FQ,         NO_ARGS),
    ENTRY('R',     scan_FR,         exec_FR,         NO_ARGS),
    ENTRY('r',     scan_FR,         exec_FR,         NO_ARGS),
    ENTRY('S',     scan_FS,         exec_FS,         NO_ARGS),
    ENTRY('s',     scan_FS,         exec_FS,         NO_ARGS),
    ENTRY('U',     scan_case,       exec_FU,         NO_ARGS),
    ENTRY('u',     scan_case,       exec_FU,         NO_ARGS),
    ENTRY('Z',     scan_FZ,         exec_nop,        NO_ARGS),
    ENTRY('z',     scan_FZ,         exec_nop,        NO_ARGS),
    ENTRY('_',     scan_F_ubar,     exec_F_ubar,     NO_ARGS),
    ENTRY('|',     NULL,            exec_F_vbar,     NO_ARGS),
};

static const uint f_max = countof(f_table); ///< Maximum F command
