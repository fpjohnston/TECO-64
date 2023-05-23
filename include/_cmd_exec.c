///
///  @file    _cmd_exec.c
///
///  @brief   Auxiliary file for cmd_exec.c, containing command tables.
///
///           This file defines data that is statically stored, rather than
///           just being declared, and is intended to be used by only one
///           source file, through use of the #include directive. This is why
///           it is created in the include/ directory, with a file type of
///           .c, and with the same name (prefaced by a leading underscore)
///           as the file which will use it.
///
///  *** This file is automatically generated. DO NOT MODIFY. ***
///
///  @copyright 2020-2023 Franklin P. Johnston / Nowwith Treble Software
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

// Command types

enum
{
    c_none,                     ///< General command
    c_LF,                       ///< Line feed
    c_WHITE,                    ///< Whitespace other than LF
    c_UP,                       ///< Uparrow
    c_E,                        ///< E commands
    c_F,                        ///< F commands
    c_M                         ///< M, !, [, ], and ESCape
};

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
    int type;                       ///< Command type

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

#define ENTRY(chr, scan, exec, type) [chr] = { scan, exec, type, #scan, #exec }

#else

#define ENTRY(chr, scan, exec, type) [chr] = { scan, exec, type }

#endif

///
///  @var    cmd_table
///
///  @brief  Dispatch table, defining functions to scan and execute commands.
///

static const struct cmd_table cmd_table[] =
{
    ENTRY(NUL,         NULL,            NULL,            c_WHITE ),
    ENTRY(CTRL_A,      scan_ctrl_A,     exec_ctrl_A,     c_none  ),
    ENTRY(CTRL_B,      scan_ctrl_B,     NULL,            c_none  ),
    ENTRY(CTRL_C,      NULL,            exec_ctrl_C,     c_none  ),
    ENTRY(CTRL_D,      scan_simple,     exec_ctrl_D,     c_none  ),
    ENTRY(CTRL_E,      scan_flag1,      exec_ctrl_E,     c_none  ),
    ENTRY(CTRL_F,      NULL,            NULL,            c_none  ),
    ENTRY(CTRL_G,      NULL,            NULL,            c_none  ),
    ENTRY(CTRL_H,      scan_ctrl_H,     NULL,            c_none  ),
    ENTRY(CTRL_I,      scan_ctrl_I,     exec_ctrl_I,     c_none  ),
    ENTRY(CTRL_J,      NULL,            NULL,            c_LF    ),
    ENTRY(CTRL_K,      NULL,            NULL,            c_none  ),
    ENTRY(CTRL_L,      NULL,            NULL,            c_WHITE ),
    ENTRY(CTRL_M,      NULL,            NULL,            c_WHITE ),
    ENTRY(CTRL_N,      scan_flag1,      NULL,            c_none  ),
    ENTRY(CTRL_O,      scan_simple,     exec_ctrl_O,     c_none  ),
    ENTRY(CTRL_P,      scan_ctrl_P,     NULL,            c_none  ),
    ENTRY(CTRL_Q,      scan_ctrl_Q,     exec_ctrl_Q,     c_none  ),
    ENTRY(CTRL_R,      NULL,            exec_ctrl_R,     c_none  ),
    ENTRY(CTRL_S,      scan_ctrl_S,     NULL,            c_none  ),
    ENTRY(CTRL_T,      scan_ctrl_T,     exec_ctrl_T,     c_none  ),
    ENTRY(CTRL_U,      scan_ctrl_U,     exec_ctrl_U,     c_none  ),
    ENTRY(CTRL_V,      NULL,            exec_ctrl_V,     c_none  ),
    ENTRY(CTRL_W,      NULL,            exec_ctrl_W,     c_none  ),
    ENTRY(CTRL_X,      scan_flag1,      exec_ctrl_X,     c_none  ),
    ENTRY(CTRL_Y,      scan_ctrl_Y,     NULL,            c_none  ),
    ENTRY(CTRL_Z,      scan_ctrl_Z,     NULL,            c_none  ),
    ENTRY(CTRL_LEFT,   NULL,            exec_escape,     c_M     ),
    ENTRY(CTRL_BACK,   NULL,            NULL,            c_none  ),
    ENTRY(CTRL_RIGHT,  NULL,            NULL,            c_none  ),
    ENTRY(CTRL_UP,     scan_ctrl_up,    NULL,            c_none  ),
    ENTRY(CTRL_UNDER,  scan_ctrl_under,  NULL,            c_none  ),
    ENTRY(SPACE,       NULL,            NULL,            c_WHITE ),
    ENTRY('!',         scan_not,        exec_bang,       c_M     ),
    ENTRY('"',         scan_quote,      exec_quote,      c_none  ),
    ENTRY('#',         scan_or,         NULL,            c_none  ),
    ENTRY('$',         NULL,            NULL,            c_none  ),
    ENTRY('%',         scan_pct,        exec_pct,        c_none  ),
    ENTRY('&',         scan_and,        NULL,            c_none  ),
    ENTRY('\'',        NULL,            exec_apos,       c_none  ),
    ENTRY('(',         scan_open,       NULL,            c_none  ),
    ENTRY(')',         scan_close,      NULL,            c_none  ),
    ENTRY('*',         scan_mul,        NULL,            c_none  ),
    ENTRY('+',         scan_add,        NULL,            c_none  ),
    ENTRY(',',         scan_comma,      NULL,            c_none  ),
    ENTRY('-',         scan_sub,        NULL,            c_none  ),
    ENTRY('.',         scan_dot,        NULL,            c_none  ),
    ENTRY('/',         scan_div,        NULL,            c_none  ),
    ENTRY('0',         scan_number,     NULL,            c_none  ),
    ENTRY('1',         scan_number,     NULL,            c_none  ),
    ENTRY('2',         scan_number,     NULL,            c_none  ),
    ENTRY('3',         scan_number,     NULL,            c_none  ),
    ENTRY('4',         scan_number,     NULL,            c_none  ),
    ENTRY('5',         scan_number,     NULL,            c_none  ),
    ENTRY('6',         scan_number,     NULL,            c_none  ),
    ENTRY('7',         scan_number,     NULL,            c_none  ),
    ENTRY('8',         scan_number,     NULL,            c_none  ),
    ENTRY('9',         scan_number,     NULL,            c_none  ),
    ENTRY(':',         scan_colon,      NULL,            c_none  ),
    ENTRY(';',         scan_semi,       exec_semi,       c_none  ),
    ENTRY('<',         scan_less,       exec_less,       c_none  ),
    ENTRY('=',         scan_equals,     exec_equals,     c_none  ),
    ENTRY('>',         scan_greater,    exec_greater,    c_none  ),
    ENTRY('?',         scan_simple,     exec_trace,      c_none  ),
    ENTRY('@',         scan_atsign,     NULL,            c_none  ),
    ENTRY('A',         scan_A,          exec_A,          c_none  ),
    ENTRY('a',         scan_A,          exec_A,          c_none  ),
    ENTRY('B',         scan_B,          NULL,            c_none  ),
    ENTRY('b',         scan_B,          NULL,            c_none  ),
    ENTRY('C',         scan_C,          exec_C,          c_none  ),
    ENTRY('c',         scan_C,          exec_C,          c_none  ),
    ENTRY('D',         scan_D,          exec_D,          c_none  ),
    ENTRY('d',         scan_D,          exec_D,          c_none  ),
    ENTRY('E',         NULL,            NULL,            c_E     ),
    ENTRY('e',         NULL,            NULL,            c_E     ),
    ENTRY('F',         NULL,            NULL,            c_F     ),
    ENTRY('f',         NULL,            NULL,            c_F     ),
    ENTRY('G',         scan_G,          exec_G,          c_none  ),
    ENTRY('g',         scan_G,          exec_G,          c_none  ),
    ENTRY('H',         scan_H,          NULL,            c_none  ),
    ENTRY('h',         scan_H,          NULL,            c_none  ),
    ENTRY('I',         scan_I,          exec_I,          c_none  ),
    ENTRY('i',         scan_I,          exec_I,          c_none  ),
    ENTRY('J',         scan_J,          exec_J,          c_none  ),
    ENTRY('j',         scan_J,          exec_J,          c_none  ),
    ENTRY('K',         scan_K,          exec_K,          c_none  ),
    ENTRY('k',         scan_K,          exec_K,          c_none  ),
    ENTRY('L',         scan_C,          exec_L,          c_none  ),
    ENTRY('l',         scan_C,          exec_L,          c_none  ),
    ENTRY('M',         scan_M,          exec_M,          c_M     ),
    ENTRY('m',         scan_M,          exec_M,          c_M     ),
    ENTRY('N',         scan_N,          exec_N,          c_none  ),
    ENTRY('n',         scan_N,          exec_N,          c_none  ),
    ENTRY('O',         scan_O,          exec_O,          c_none  ),
    ENTRY('o',         scan_O,          exec_O,          c_none  ),
    ENTRY('P',         scan_P,          exec_P,          c_none  ),
    ENTRY('p',         scan_P,          exec_P,          c_none  ),
    ENTRY('Q',         scan_Q,          NULL,            c_none  ),
    ENTRY('q',         scan_Q,          NULL,            c_none  ),
    ENTRY('R',         scan_C,          exec_R,          c_none  ),
    ENTRY('r',         scan_C,          exec_R,          c_none  ),
    ENTRY('S',         scan_S,          exec_S,          c_none  ),
    ENTRY('s',         scan_S,          exec_S,          c_none  ),
    ENTRY('T',         scan_T,          exec_T,          c_none  ),
    ENTRY('t',         scan_T,          exec_T,          c_none  ),
    ENTRY('U',         scan_U,          exec_U,          c_none  ),
    ENTRY('u',         scan_U,          exec_U,          c_none  ),
    ENTRY('V',         scan_V,          exec_V,          c_none  ),
    ENTRY('v',         scan_V,          exec_V,          c_none  ),
    ENTRY('W',         scan_W,          exec_W,          c_none  ),
    ENTRY('w',         scan_W,          exec_W,          c_none  ),
    ENTRY('X',         scan_X,          exec_X,          c_none  ),
    ENTRY('x',         scan_X,          exec_X,          c_none  ),
    ENTRY('Y',         scan_Y,          exec_Y,          c_none  ),
    ENTRY('y',         scan_Y,          exec_Y,          c_none  ),
    ENTRY('Z',         scan_Z,          NULL,            c_none  ),
    ENTRY('z',         scan_Z,          NULL,            c_none  ),
    ENTRY('[',         scan_push,       exec_push,       c_M     ),
    ENTRY('\\',        NULL,            exec_back,       c_none  ),
    ENTRY(']',         scan_pop,        exec_pop,        c_M     ),
    ENTRY('^',         NULL,            NULL,            c_UP    ),
    ENTRY('_',         scan_under,      exec_under,      c_none  ),
    ENTRY('`',         NULL,            NULL,            c_none  ),
    ENTRY('{',         NULL,            NULL,            c_none  ),
    ENTRY('|',         NULL,            exec_pipe,       c_none  ),
    ENTRY('}',         NULL,            NULL,            c_none  ),
    ENTRY('~',         scan_xor,        NULL,            c_none  ),
    ENTRY(DEL,         NULL,            NULL,            c_none  ),
};


///  @var    e_table
///  @brief  Table for all commands starting with E.

static const struct cmd_table e_table[] =
{
    ENTRY('%',         scan_EQ,         exec_E_pct,      c_none  ),
    ENTRY('1',         scan_flag2,      exec_E1,         c_none  ),
    ENTRY('2',         scan_flag2,      exec_E2,         c_none  ),
    ENTRY('3',         scan_flag2,      exec_E3,         c_none  ),
    ENTRY('4',         scan_flag2,      exec_E4,         c_none  ),
    ENTRY('A',         scan_simple,     exec_EA,         c_none  ),
    ENTRY('a',         scan_simple,     exec_EA,         c_none  ),
    ENTRY('B',         scan_ER,         exec_EB,         c_none  ),
    ENTRY('b',         scan_ER,         exec_EB,         c_none  ),
    ENTRY('C',         NULL,            exec_EC,         c_none  ),
    ENTRY('c',         NULL,            exec_EC,         c_none  ),
    ENTRY('D',         scan_flag2,      exec_ED,         c_none  ),
    ENTRY('d',         scan_flag2,      exec_ED,         c_none  ),
    ENTRY('E',         scan_flag1,      exec_EE,         c_none  ),
    ENTRY('e',         scan_flag1,      exec_EE,         c_none  ),
    ENTRY('F',         scan_simple,     exec_EF,         c_none  ),
    ENTRY('f',         scan_simple,     exec_EF,         c_none  ),
    ENTRY('G',         scan_ER,         exec_EG,         c_none  ),
    ENTRY('g',         scan_ER,         exec_EG,         c_none  ),
    ENTRY('H',         scan_flag2,      exec_EH,         c_none  ),
    ENTRY('h',         scan_flag2,      exec_EH,         c_none  ),
    ENTRY('I',         scan_EI,         exec_EI,         c_none  ),
    ENTRY('i',         scan_EI,         exec_EI,         c_none  ),
    ENTRY('J',         scan_EJ,         exec_nop,        c_none  ),
    ENTRY('j',         scan_EJ,         exec_nop,        c_none  ),
    ENTRY('K',         scan_simple,     exec_EK,         c_none  ),
    ENTRY('k',         scan_simple,     exec_EK,         c_none  ),
    ENTRY('L',         scan_EL,         exec_EL,         c_none  ),
    ENTRY('l',         scan_EL,         exec_EL,         c_none  ),
    ENTRY('M',         scan_EM,         exec_EM,         c_none  ),
    ENTRY('m',         scan_EM,         exec_EM,         c_none  ),
    ENTRY('N',         scan_ER,         exec_EN,         c_none  ),
    ENTRY('n',         scan_ER,         exec_EN,         c_none  ),
    ENTRY('O',         scan_EO,         exec_EO,         c_none  ),
    ENTRY('o',         scan_EO,         exec_EO,         c_none  ),
    ENTRY('P',         scan_simple,     exec_EP,         c_none  ),
    ENTRY('p',         scan_simple,     exec_EP,         c_none  ),
    ENTRY('Q',         scan_EQ,         exec_EQ,         c_none  ),
    ENTRY('q',         scan_EQ,         exec_EQ,         c_none  ),
    ENTRY('R',         scan_ER,         exec_ER,         c_none  ),
    ENTRY('r',         scan_ER,         exec_ER,         c_none  ),
    ENTRY('S',         scan_flag2,      exec_ES,         c_none  ),
    ENTRY('s',         scan_flag2,      exec_ES,         c_none  ),
    ENTRY('T',         scan_flag2,      exec_ET,         c_none  ),
    ENTRY('t',         scan_flag2,      exec_ET,         c_none  ),
    ENTRY('U',         scan_flag1,      exec_EU,         c_none  ),
    ENTRY('u',         scan_flag1,      exec_EU,         c_none  ),
    ENTRY('V',         scan_flag2,      exec_EV,         c_none  ),
    ENTRY('v',         scan_flag2,      exec_EV,         c_none  ),
    ENTRY('W',         scan_ER,         exec_EW,         c_none  ),
    ENTRY('w',         scan_ER,         exec_EW,         c_none  ),
    ENTRY('X',         scan_simple,     exec_EX,         c_none  ),
    ENTRY('x',         scan_simple,     exec_EX,         c_none  ),
    ENTRY('Y',         scan_Y,          exec_EY,         c_none  ),
    ENTRY('y',         scan_Y,          exec_EY,         c_none  ),
    ENTRY('Z',         scan_ER,         exec_EZ,         c_none  ),
    ENTRY('z',         scan_ER,         exec_EZ,         c_none  ),
    ENTRY('_',         scan_E_under,    exec_E_under,    c_none  ),
};


///  @var    f_table
///  @brief  Table for all commands starting with F.

static const struct cmd_table f_table[] =
{
    ENTRY('\'',        NULL,            exec_F_apos,     c_none  ),
    ENTRY('0',         scan_F0,         exec_nop,        c_none  ),
    ENTRY('1',         scan_F1,         exec_F1,         c_none  ),
    ENTRY('2',         scan_F1,         exec_F2,         c_none  ),
    ENTRY('3',         scan_F1,         exec_F3,         c_none  ),
    ENTRY('4',         scan_F1,         exec_F4,         c_none  ),
    ENTRY('<',         NULL,            exec_F_less,     c_none  ),
    ENTRY('>',         NULL,            exec_F_greater,  c_none  ),
    ENTRY('B',         scan_FB,         exec_FB,         c_none  ),
    ENTRY('b',         scan_FB,         exec_FB,         c_none  ),
    ENTRY('C',         scan_FC,         exec_FC,         c_none  ),
    ENTRY('c',         scan_FC,         exec_FC,         c_none  ),
    ENTRY('D',         scan_FD,         exec_FD,         c_none  ),
    ENTRY('d',         scan_FD,         exec_FD,         c_none  ),
    ENTRY('F',         scan_FF,         exec_FF,         c_none  ),
    ENTRY('f',         scan_FF,         exec_FF,         c_none  ),
    ENTRY('H',         scan_FH,         exec_nop,        c_none  ),
    ENTRY('h',         scan_FH,         exec_nop,        c_none  ),
    ENTRY('K',         scan_FK,         exec_FK,         c_none  ),
    ENTRY('k',         scan_FK,         exec_FK,         c_none  ),
    ENTRY('L',         scan_case,       exec_FL,         c_none  ),
    ENTRY('l',         scan_case,       exec_FL,         c_none  ),
    ENTRY('M',         scan_FM,         exec_FM,         c_none  ),
    ENTRY('m',         scan_FM,         exec_FM,         c_none  ),
    ENTRY('N',         scan_FN,         exec_FN,         c_none  ),
    ENTRY('n',         scan_FN,         exec_FN,         c_none  ),
    ENTRY('Q',         scan_EQ,         exec_FQ,         c_none  ),
    ENTRY('q',         scan_EQ,         exec_FQ,         c_none  ),
    ENTRY('R',         scan_FR,         exec_FR,         c_none  ),
    ENTRY('r',         scan_FR,         exec_FR,         c_none  ),
    ENTRY('S',         scan_FS,         exec_FS,         c_none  ),
    ENTRY('s',         scan_FS,         exec_FS,         c_none  ),
    ENTRY('U',         scan_case,       exec_FU,         c_none  ),
    ENTRY('u',         scan_case,       exec_FU,         c_none  ),
    ENTRY('Z',         scan_FZ,         exec_nop,        c_none  ),
    ENTRY('z',         scan_FZ,         exec_nop,        c_none  ),
    ENTRY('_',         scan_F_under,    exec_F_under,    c_none  ),
    ENTRY('|',         NULL,            exec_F_vbar,     c_none  ),
};
