///
///  @file    env_sys.c
///  @brief   System-specific environment functions for Linux.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "eflags.h"
#include "exec.h"


#define TECO_HW         101             ///< x86 platform

#define TECO_OS           2             ///< Linux O/S


///
///  @brief    Final execution of EG command.
///
///  @returns  Nothing (returns to operating system).
///
////////////////////////////////////////////////////////////////////////////////

void exit_EG(void)
{
    if (eg_command != NULL)
    {
        if (execlp("/bin/sh", "sh", "-c", eg_command, NULL) == -1)
        {
            perror("EG command failed");
        }
    }
}

extern int find_eg(const char *buf, uint len);

///
///  @brief    Find EG function.
///
///  @returns  -1 = success, 0 = unsupported, 1 = failure.
///
////////////////////////////////////////////////////////////////////////////////

int find_eg(const char *buf, uint len)
{
    char *cmd = alloc_mem(len + 1);
    char *arg;
    char *saveptr;

    sprintf(cmd, "%.*s", (int)len, buf);

    cmd = strtok_r(cmd, "\t\n\v\f\r ", &saveptr);
    arg = strtok_r(NULL, "\t\n\v\f\r ", &saveptr);

    if (arg != NULL)
    {
        return 0;
    }

    const char *name;
    const char *result;

    if (!strcasecmp(cmd, "INI"))
    {
        name = "TECO_INIT";
    }
    if (!strcasecmp(cmd, "LIB"))
    {
        name = "TECO_LIBRARY";
    }
    else if (!strcasecmp(cmd, "MEM"))
    {
        name = "TECO_MEMORY";
    }
    else if (!strcasecmp(cmd, "VTE"))
    {
        name = "TECO_VTEDIT";
    }
    else
    {
        return 0;
    }

    if ((result = getenv(name)) == NULL)
    {
        return 1;
    }

    strcpy(filename_buf, result);

    return -1;
}


///
///  @brief    Initialize environment (read environment variables, logical names,
///            etc.)
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_env(int argc, const char * const argv[])
{
    set_config(argc, argv);             // Process command-line options

    const char *env;

    // TODO: add more functionality here

    if ((env = getenv("TECO_PROMPT")) != NULL)
    {
        prompt = env;                   // Change TECO prompt
    }

    if (getenv("TECO_DEBUG") != NULL)
    {
        f.e0.dryrun = true;
    }
}


///
///  @brief    Get information environment about our environment.
///
///  @returns  Result for requested value.
///
////////////////////////////////////////////////////////////////////////////////

int teco_env(int n_arg)
{
    switch (n_arg)
    {
        case -1:
            return (TECO_HW << 8) | TECO_OS;

        case 0:
            return getpid();

        default:
        case 1:
        case 2:
            return 0;
    }
}
