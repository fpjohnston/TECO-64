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

//#if     defined(TRACE)

//    const char *scan_name;          ///< Name of scan function
//    const char *exec_name;          ///< Name of execute function

//#endif

};

#define NO_ARGS     (bool)false     ///< m & n args are not preserved by command

#define MN_ARGS     (bool)true      ///< m & n args are preserved by command

/// @def    ENTRY(chr, scan, exec)
/// @brief  Defines entries in command table.

//#if     defined(TRACE)

//#define ENTRY(chr, scan, exec) [chr] = { scan, exec, #scan, #exec }

//#else

#define ENTRY(chr, scan, exec) [chr] = { scan, exec }

//#endif

///
///  @var    cmd_table
///
///  @brief  Dispatch table, defining functions to scan and execute commands.
///

static const struct cmd_table cmd_table[] =
{
/* (INSERT: GENERAL COMMANDS) */
};


///  @var    e_table
///  @brief  Table for all commands starting with E.

static const struct cmd_table e_table[] =
{
/* (INSERT: E COMMANDS) */
};


///  @var    f_table
///  @brief  Table for all commands starting with F.

static const struct cmd_table f_table[] =
{
/* (INSERT: F COMMANDS) */
};
