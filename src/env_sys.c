///
///  @file    env_sys.c
///  @brief   System-specific environment functions for Linux.
///
///  @copyright 2019-2022 Franklin P. Johnston / Nowwith Treble Software
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

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "exec.h"
#include "file.h"
#include "term.h"


const char *teco_init = NULL;           ///< Name of initialization macro

const char *teco_memory = NULL;         ///< Name of memory file

const char *teco_library = NULL;        ///< Location of macro library

const char *teco_prompt = "*";          ///< TECO's prompt

const char *teco_vtedit = NULL;         ///< Name of VTEDIT macro

// Define operating system

#if     defined(__linux)

#define TECO_OS          10             ///< Linux
#define TECO_HW          10             ///< x86

#elif   defined(__win64)

#define TECO_OS          20             ///< Windows
#define TECO_HW          10             ///< x86

#elif   defined(__APPLE__)

#define TECO_OS          30             ///< MacOS
#define TECO_HW          20             ///< x86

#elif   defined(__vms)

#define TECO_OS          40             ///< VMS
#define TECO_HW          30             ///< Alpha

#else

#define TECO_OS          -1             ///< Unknown
#define TECO_HW          -1             ///< Unknown

#endif


///
///  @brief    Final execution of EG command.
///
///  @returns  Nothing (returns to operating system).
///
////////////////////////////////////////////////////////////////////////////////

void exit_EG(void)
{
    if (eg_command[0] != NUL)
    {
        if (execlp("/bin/sh", "sh", "-c", eg_command, NULL) == -1)
        {
            perror("EG command failed");
        }
    }
}


///
///  @brief    Find EG function.
///
///  @returns  -1 = success, 0 = unsupported, 1 = failure.
///
////////////////////////////////////////////////////////////////////////////////

int find_eg(char *cmd)
{
    assert(cmd != NULL);

    //  Get environment variable and load Q-register *.

    const char *result;

    if (!strcasecmp(cmd, "INI"))
    {
        result = teco_init = getenv("TECO_INIT");
    }
    else if (!strcasecmp(cmd, "LIB"))
    {
        result = teco_library = getenv("TECO_LIBRARY");
    }
    else if (!strcasecmp(cmd, "MEM"))
    {
        result = teco_memory = getenv("TECO_MEMORY");
    }
    else if (!strcasecmp(cmd, "VTE"))
    {
        result = teco_vtedit = getenv("TECO_VTEDIT");
    }
    else
    {
        return 0;                       // Invalid environment var.
    }

    if (result == NULL)
    {
        return 1;                       // Valid env. var., but undefined
    }

    set_last(result);

    return -1;                          // Environment variable is defined
}


///
///  @brief    Initialize environment (read environment variables,
///            logical names, etc.)
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_env(void)
{
    teco_init    = getenv("TECO_INIT");
    teco_memory  = getenv("TECO_MEMORY");
    teco_library = getenv("TECO_LIBRARY");
    teco_vtedit  = getenv("TECO_VTEDIT");

    const char *env;

    if ((env = getenv("TECO_PROMPT")) != NULL)
    {
        teco_prompt = env;              // Change TECO prompt
    }
}


///
///  @brief    Get information environment about our environment.
///
///            -1EJ - The operating system upon which TECO is running.
///                   This is currently 10 for Linux.
///
///            -2EJ - The processor upon which TECO is running. This is
///                   currently 10 for x86.
///
///            -3EJ - The processor word size in bits.
///
///            -4EJ - The size of numeric arguments in bits.
///
///            -5EJ - The process status, as follows:
///
///                   > 0 - Foreground process, attached to a terminal.
///                   = 0 - Background process, attached to a terminal.
///                   < 0 - Child or detached process.
///
///             0EJ - Process ID
///            0:EJ - Parent process ID
///
///  @returns  Result for requested value.
///
////////////////////////////////////////////////////////////////////////////////

int teco_env(int n_arg, bool colon)
{
    switch (n_arg)
    {
        case 2:
        case 1:
            return 0;

        case 0:
            return colon ? getppid() : getpid();

        case -1:
            return TECO_OS;

        case -2:
            return TECO_HW;

        case -3:
            return sizeof(size_t) * CHAR_BIT;

        case -4:
            return sizeof(int_t) * CHAR_BIT;

        case -5:
            pid_t pid = tcgetpgrp(STDIN_FILENO);

            if (pid == -1)
            {
                return -1;              // Process is child or detached
            }
            else if (pid == getpgrp())
            {
                return (int)pid;        // Foreground process
            }
            else
            {
                return 0;               // Background process
            }

        default:
            throw(E_NYI);               // No such EJ command
    }
}
