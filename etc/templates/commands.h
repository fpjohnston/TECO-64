///
///  @file    commands.h
///  @brief   Data tables used in executing command strings.
///
/* (INSERT: WARNING NOTICE) */
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

#include "ascii.h"

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
/* (INSERT: GENERAL COMMANDS) */
};

static const uint cmd_max = countof(cmd_table); ///< Maximum command


///  @var    e_table
///  @brief  Table for all commands starting with E.

static const struct cmd_table e_table[] =
{
/* (INSERT: E COMMANDS) */
};

static const uint e_max = countof(e_table); ///< Maximum E command


///  @var    f_table
///  @brief  Table for all commands starting with F.

static const struct cmd_table f_table[] =
{
/* (INSERT: F COMMANDS) */
};

static const uint f_max = countof(f_table); ///< Maximum F command
