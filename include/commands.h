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

///  @typedef exec_func
///  @brief   Function to execute command.

typedef void (exec_func)(struct cmd *cmd);

///  @enum   cmd_opts
///  @brief  Options defined for each command.

enum cmd_opts
{
    OPT_M  = 1,                     ///< m argument allowed
    OPT_N  = 1 << 1,                ///< n argument allowed
    OPT_F  = 1 << 2,                ///< Flag argument
    OPT_C  = 1 << 3,                ///< : modifier allowed
    OPT_D  = 1 << 4,                ///< :: modifier allowed
    OPT_A  = 1 << 5,                ///< @ modifier allowed
    OPT_Q  = 1 << 6,                ///< Q-register required
    OPT_T1 = 1 << 7,                ///< 1 text argument allowed
    OPT_T2 = 1 << 8,                ///< 2 text arguments allowed
    OPT_O  = 1 << 9,                ///< Operator or operand
    OPT_E  = 1 << 10                ///< "ESCape"-like command
};

///  @struct cmd_table
///
///  @brief  Format of command tables used to parse and execute commands.

struct cmd_table
{
    exec_func *exec;                ///< Execution function
    enum cmd_opts opts;             ///< Command options

#if     defined(TECO_TRACE)

    const char *name;               ///< Command function name

#endif

};

///
///  @var    cmd_table
///
///  @brief  Dispatch table, defining functions to scan execute input
///          characters, as well as the options for each command.
///

/// @def    ENTRY(chr, func, opts)
/// @brief  Defines entries in command table.

#if     defined(TECO_TRACE)

#define ENTRY(chr, func, opts) [chr] = { func, (opts), #func }

#else

#define ENTRY(chr, func, opts) [chr] = { func, (opts) }

#endif

static const struct cmd_table cmd_table[] =
{
    ENTRY(CTRL_A,  exec_ctrl_A,     OPT_M|OPT_N|OPT_C|OPT_A|OPT_T1),
    ENTRY(CTRL_B,  exec_ctrl_B,     OPT_O),
    ENTRY(CTRL_C,  exec_ctrl_C,     OPT_M|OPT_N|OPT_E),
    ENTRY(CTRL_D,  exec_ctrl_D,     0),
    ENTRY(CTRL_E,  exec_ctrl_E,     OPT_M|OPT_N|OPT_F),
    ENTRY(CTRL_H,  exec_ctrl_H,     OPT_O),
    ENTRY(CTRL_I,  exec_ctrl_I,     OPT_T1),
    ENTRY(CTRL_N,  exec_ctrl_N,     OPT_N),
    ENTRY(CTRL_O,  exec_ctrl_O,     0),
    ENTRY(CTRL_P,  exec_ctrl_P,     OPT_O),
    ENTRY(CTRL_Q,  exec_ctrl_Q,     OPT_N),
    ENTRY(CTRL_R,  exec_ctrl_R,     OPT_N|OPT_F),
    ENTRY(CTRL_S,  exec_ctrl_S,     OPT_O),
    ENTRY(CTRL_T,  exec_ctrl_T,     OPT_M|OPT_N|OPT_C),
    ENTRY(CTRL_U,  exec_ctrl_U,     OPT_M|OPT_N|OPT_C|OPT_A|OPT_Q|OPT_T1),
    ENTRY(CTRL_V,  exec_ctrl_V,     OPT_N),
    ENTRY(CTRL_W,  exec_ctrl_W,     OPT_N),
    ENTRY(CTRL_X,  exec_ctrl_X,     OPT_N|OPT_F),
    ENTRY(CTRL_Y,  exec_ctrl_Y,     OPT_O),
    ENTRY(CTRL_Z,  exec_ctrl_Z,     OPT_O),
    ENTRY(ESC,     exec_escape,     OPT_M|OPT_N|OPT_E),
    ENTRY('\x1F',  exec_ctrl_ubar,  OPT_O),
    ENTRY('!',     exec_bang,       OPT_M|OPT_N|OPT_A|OPT_T1),
    ENTRY('"',     exec_quote,      OPT_N),
    ENTRY('#',     exec_oper,       OPT_O),
    ENTRY('%',     exec_pct,        OPT_N|OPT_C|OPT_Q),
    ENTRY('&',     exec_oper,       OPT_O),
    ENTRY('\'',    exec_apos,       OPT_M|OPT_N|OPT_E),
    ENTRY('(',     exec_lparen,     OPT_O),
    ENTRY(')',     exec_rparen,     OPT_O),
    ENTRY('*',     exec_oper,       OPT_O),
    ENTRY('+',     exec_oper,       OPT_O),
    ENTRY(',',     exec_comma,      OPT_O),
    ENTRY('-',     exec_oper,       OPT_O),
    ENTRY('.',     exec_dot,        OPT_O),
    ENTRY('/',     exec_oper,       OPT_O),
    ENTRY('0',     exec_number,     OPT_O),
    ENTRY('1',     exec_number,     OPT_O),
    ENTRY('2',     exec_number,     OPT_O),
    ENTRY('3',     exec_number,     OPT_O),
    ENTRY('4',     exec_number,     OPT_O),
    ENTRY('5',     exec_number,     OPT_O),
    ENTRY('6',     exec_number,     OPT_O),
    ENTRY('7',     exec_number,     OPT_O),
    ENTRY('8',     exec_number,     OPT_O),
    ENTRY('9',     exec_number,     OPT_O),
    ENTRY(';',     exec_semi,       OPT_N|OPT_C),
    ENTRY('<',     exec_lt,         OPT_N),
    ENTRY('=',     exec_equals,     OPT_N|OPT_C|OPT_A|OPT_T1),
    ENTRY('>',     exec_gt,         OPT_M|OPT_N|OPT_E),
    ENTRY('?',     exec_trace,      0),
    ENTRY('A',     exec_A,          OPT_N|OPT_C),
    ENTRY('a',     exec_A,          OPT_N|OPT_C),
    ENTRY('B',     exec_B,          OPT_O),
    ENTRY('b',     exec_B,          OPT_O),
    ENTRY('C',     exec_C,          OPT_N|OPT_C),
    ENTRY('c',     exec_C,          OPT_N|OPT_C),
    ENTRY('D',     exec_D,          OPT_M|OPT_N|OPT_C),
    ENTRY('d',     exec_D,          OPT_M|OPT_N|OPT_C),
    ENTRY('G',     exec_G,          OPT_N|OPT_C|OPT_Q),
    ENTRY('g',     exec_G,          OPT_N|OPT_C|OPT_Q),
    ENTRY('H',     exec_H,          OPT_O),
    ENTRY('h',     exec_H,          OPT_O),
    ENTRY('I',     exec_I,          OPT_M|OPT_N|OPT_A|OPT_T1),
    ENTRY('i',     exec_I,          OPT_M|OPT_N|OPT_A|OPT_T1),
    ENTRY('J',     exec_J,          OPT_N|OPT_C),
    ENTRY('j',     exec_J,          OPT_N|OPT_C),
    ENTRY('K',     exec_K,          OPT_M|OPT_N),
    ENTRY('k',     exec_K,          OPT_M|OPT_N),
    ENTRY('L',     exec_L,          OPT_N|OPT_C),
    ENTRY('l',     exec_L,          OPT_N|OPT_C),
    ENTRY('M',     exec_M,          OPT_M|OPT_N|OPT_C|OPT_Q),
    ENTRY('m',     exec_M,          OPT_M|OPT_N|OPT_C|OPT_Q),
    ENTRY('N',     exec_N,          OPT_N|OPT_C|OPT_A|OPT_T1),
    ENTRY('n',     exec_N,          OPT_N|OPT_C|OPT_A|OPT_T1),
    ENTRY('O',     exec_O,          OPT_N|OPT_A|OPT_T1),
    ENTRY('o',     exec_O,          OPT_N|OPT_A|OPT_T1),
    ENTRY('P',     exec_P,          OPT_M|OPT_N|OPT_C),
    ENTRY('p',     exec_P,          OPT_M|OPT_N|OPT_C),
    ENTRY('Q',     exec_Q,          OPT_N|OPT_C|OPT_Q),
    ENTRY('q',     exec_Q,          OPT_N|OPT_C|OPT_Q),
    ENTRY('R',     exec_R,          OPT_N|OPT_C),
    ENTRY('r',     exec_R,          OPT_N|OPT_C),
    ENTRY('S',     exec_S,          OPT_M|OPT_N|OPT_C|OPT_D|OPT_A|OPT_T1),
    ENTRY('s',     exec_S,          OPT_M|OPT_N|OPT_C|OPT_D|OPT_A|OPT_T1),
    ENTRY('T',     exec_T,          OPT_M|OPT_N|OPT_C),
    ENTRY('t',     exec_T,          OPT_M|OPT_N|OPT_C),
    ENTRY('U',     exec_U,          OPT_M|OPT_N|OPT_Q),
    ENTRY('u',     exec_U,          OPT_M|OPT_N|OPT_Q),
    ENTRY('V',     exec_V,          OPT_M|OPT_N),
    ENTRY('v',     exec_V,          OPT_M|OPT_N),
    ENTRY('W',     exec_W,          OPT_M|OPT_N|OPT_C),
    ENTRY('w',     exec_W,          OPT_M|OPT_N|OPT_C),
    ENTRY('X',     exec_X,          OPT_M|OPT_N|OPT_C|OPT_Q),
    ENTRY('x',     exec_X,          OPT_M|OPT_N|OPT_C|OPT_Q),
    ENTRY('Y',     exec_Y,          OPT_C),
    ENTRY('y',     exec_Y,          OPT_C),
    ENTRY('Z',     exec_Z,          OPT_O),
    ENTRY('z',     exec_Z,          OPT_O),
    ENTRY('[',     exec_lbracket,   OPT_M|OPT_N),
    ENTRY('\\',    exec_bslash,     OPT_N),
    ENTRY(']',     exec_rbracket,   OPT_M|OPT_N|OPT_C),
    ENTRY('_',     exec_ubar,       OPT_N|OPT_C|OPT_A|OPT_T1),
    ENTRY('|',     exec_vbar,       OPT_M|OPT_N|OPT_E),
};

static const uint cmd_max = countof(cmd_table); ///< Maximum command


///  @var    e_table
///  @brief  Table for all commands starting with E.

static const struct cmd_table e_table[] =
{
    ENTRY('%',     exec_E_pct,      OPT_C|OPT_A|OPT_Q|OPT_T1),
    ENTRY('1',     exec_E1,         OPT_M|OPT_N|OPT_F),
    ENTRY('2',     exec_E2,         OPT_M|OPT_N|OPT_F),
    ENTRY('3',     exec_E3,         OPT_M|OPT_N|OPT_F),
    ENTRY('4',     exec_E4,         OPT_M|OPT_N|OPT_F),
    ENTRY('A',     exec_EA,         0),
    ENTRY('a',     exec_EA,         0),
    ENTRY('B',     exec_EB,         OPT_C|OPT_A|OPT_T1),
    ENTRY('b',     exec_EB,         OPT_C|OPT_A|OPT_T1),
    ENTRY('C',     exec_EC,         OPT_N),
    ENTRY('c',     exec_EC,         OPT_N),
    ENTRY('D',     exec_ED,         OPT_M|OPT_N|OPT_F),
    ENTRY('d',     exec_ED,         OPT_M|OPT_N|OPT_F),
    ENTRY('E',     exec_EE,         OPT_N|OPT_F),
    ENTRY('e',     exec_EE,         OPT_N|OPT_F),
    ENTRY('F',     exec_EF,         0),
    ENTRY('f',     exec_EF,         0),
    ENTRY('G',     exec_EG,         OPT_C|OPT_D|OPT_A|OPT_T1),
    ENTRY('g',     exec_EG,         OPT_C|OPT_D|OPT_A|OPT_T1),
    ENTRY('H',     exec_EH,         OPT_M|OPT_N|OPT_F),
    ENTRY('h',     exec_EH,         OPT_M|OPT_N|OPT_F),
    ENTRY('I',     exec_EI,         OPT_C|OPT_A|OPT_T1),
    ENTRY('i',     exec_EI,         OPT_C|OPT_A|OPT_T1),
    ENTRY('J',     exec_EJ,         OPT_M|OPT_N|OPT_C),
    ENTRY('j',     exec_EJ,         OPT_M|OPT_N|OPT_C),
    ENTRY('K',     exec_EK,         0),
    ENTRY('k',     exec_EK,         0),
    ENTRY('L',     exec_EL,         OPT_C|OPT_A|OPT_T1),
    ENTRY('l',     exec_EL,         OPT_C|OPT_A|OPT_T1),
    ENTRY('N',     exec_EN,         OPT_C|OPT_A|OPT_T1),
    ENTRY('n',     exec_EN,         OPT_C|OPT_A|OPT_T1),
    ENTRY('O',     exec_EO,         OPT_N|OPT_C|OPT_F),
    ENTRY('o',     exec_EO,         OPT_N|OPT_C|OPT_F),
    ENTRY('P',     exec_EP,         0),
    ENTRY('p',     exec_EP,         0),
    ENTRY('Q',     exec_EQ,         OPT_C|OPT_A|OPT_Q|OPT_T1),
    ENTRY('q',     exec_EQ,         OPT_C|OPT_A|OPT_Q|OPT_T1),
    ENTRY('R',     exec_ER,         OPT_C|OPT_A|OPT_T1),
    ENTRY('r',     exec_ER,         OPT_C|OPT_A|OPT_T1),
    ENTRY('S',     exec_ES,         OPT_M|OPT_N|OPT_F),
    ENTRY('s',     exec_ES,         OPT_M|OPT_N|OPT_F),
    ENTRY('T',     exec_ET,         OPT_M|OPT_N|OPT_F),
    ENTRY('t',     exec_ET,         OPT_M|OPT_N|OPT_F),
    ENTRY('U',     exec_EU,         OPT_N|OPT_F),
    ENTRY('u',     exec_EU,         OPT_N|OPT_F),
    ENTRY('V',     exec_EV,         OPT_M|OPT_N|OPT_F),
    ENTRY('v',     exec_EV,         OPT_M|OPT_N|OPT_F),
    ENTRY('W',     exec_EW,         OPT_C|OPT_A|OPT_T1),
    ENTRY('w',     exec_EW,         OPT_C|OPT_A|OPT_T1),
    ENTRY('X',     exec_EX,         0),
    ENTRY('x',     exec_EX,         0),
    ENTRY('Y',     exec_EY,         OPT_C),
    ENTRY('y',     exec_EY,         OPT_C),
    ENTRY('_',     exec_E_ubar,     OPT_N|OPT_C|OPT_A|OPT_T1),
};

static const uint e_max = countof(e_table); ///< Maximum E command


///  @var    f_table
///  @brief  Table for all commands starting with F.

static const struct cmd_table f_table[] =
{
    ENTRY('\'',    exec_F_apos,     OPT_M|OPT_N|OPT_E),
    ENTRY('0',     exec_F0,         OPT_O),
    ENTRY('1',     exec_F1,         OPT_M|OPT_N|OPT_A|OPT_T1|OPT_T2),
    ENTRY('2',     exec_F2,         OPT_M|OPT_N|OPT_A|OPT_T1|OPT_T2),
    ENTRY('3',     exec_F3,         OPT_M|OPT_N|OPT_A|OPT_T1|OPT_T2),
    ENTRY('<',     exec_F_lt,       OPT_M|OPT_N|OPT_E),
    ENTRY('>',     exec_F_gt,       OPT_M|OPT_N|OPT_E),
    ENTRY('B',     exec_FB,         OPT_M|OPT_N|OPT_C|OPT_A|OPT_T1),
    ENTRY('b',     exec_FB,         OPT_M|OPT_N|OPT_C|OPT_A|OPT_T1),
    ENTRY('C',     exec_FC,         OPT_M|OPT_N|OPT_C|OPT_A|OPT_T1|OPT_T2),
    ENTRY('c',     exec_FC,         OPT_M|OPT_N|OPT_C|OPT_A|OPT_T1|OPT_T2),
    ENTRY('D',     exec_FD,         OPT_N|OPT_C|OPT_A|OPT_T1),
    ENTRY('d',     exec_FD,         OPT_N|OPT_C|OPT_A|OPT_T1),
    ENTRY('H',     exec_FH,         OPT_O),
    ENTRY('h',     exec_FH,         OPT_O),
    ENTRY('K',     exec_FK,         OPT_N|OPT_C|OPT_A|OPT_T1),
    ENTRY('k',     exec_FK,         OPT_N|OPT_C|OPT_A|OPT_T1),
    ENTRY('L',     exec_FL,         OPT_M|OPT_N),
    ENTRY('l',     exec_FL,         OPT_M|OPT_N),
    ENTRY('M',     exec_FM,         OPT_A|OPT_T1|OPT_T2),
    ENTRY('m',     exec_FM,         OPT_A|OPT_T1|OPT_T2),
    ENTRY('N',     exec_FN,         OPT_M|OPT_N|OPT_C|OPT_A|OPT_T1|OPT_T2),
    ENTRY('n',     exec_FN,         OPT_M|OPT_N|OPT_C|OPT_A|OPT_T1|OPT_T2),
    ENTRY('Q',     exec_FQ,         OPT_A|OPT_Q|OPT_T1),
    ENTRY('q',     exec_FQ,         OPT_A|OPT_Q|OPT_T1),
    ENTRY('R',     exec_FR,         OPT_M|OPT_N|OPT_C|OPT_A|OPT_T1),
    ENTRY('r',     exec_FR,         OPT_M|OPT_N|OPT_C|OPT_A|OPT_T1),
    ENTRY('S',     exec_FS,         OPT_M|OPT_N|OPT_C|OPT_D|OPT_A|OPT_T1|OPT_T2),
    ENTRY('s',     exec_FS,         OPT_M|OPT_N|OPT_C|OPT_D|OPT_A|OPT_T1|OPT_T2),
    ENTRY('U',     exec_FU,         OPT_M|OPT_N),
    ENTRY('u',     exec_FU,         OPT_M|OPT_N),
    ENTRY('Z',     exec_FZ,         OPT_O),
    ENTRY('z',     exec_FZ,         OPT_O),
    ENTRY('_',     exec_F_ubar,     OPT_M|OPT_N|OPT_C|OPT_A|OPT_T1|OPT_T2),
    ENTRY('|',     exec_F_vbar,     OPT_M|OPT_N|OPT_E),
};

static const uint f_max = countof(f_table); ///< Maximum F command
