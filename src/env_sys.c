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

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "eflags.h"
#include "exec.h"
#include "file.h"
#include "term.h"


#if    0 // TODO: what to do with the following?

enum
{
    HW_X86 = 1,                         ///< x86 processor
    HW_PPC                              ///< PowerPC processor
};

enum
{
    BITS_32 = 32,                       ///< Word size is 32 bits
    BITS_64 = 64                        ///< Word size is 64 bits
};

enum
{
    OS_LINUX = 1,                       ///< Linux
    OS_WINDOWS,                         ///< Windows
    OS_MAC                              ///< MacOS
};

enum
{
    ENV_NATIVE = 1,                     ///< Native environment
    ENV_LINUX,                          ///< Linux emulator
    ENV_WINDOWS                         ///< Windows emulator
};
f
#endif

#define TECO_HW          101            ///< x86 hardware

#define TECO_OS          2              ///< Linux operating system

// Local functions

static void read_value(const char *var, uint *value);


///
///  @brief    Execute FI command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_FI(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len == 0)
    {
        print_str("FI: no keyword found\r\n");

        return;
    }    

    if (cmd->text2.len == 0)
    {
        print_str("FI: no value found for keyword '%.*s'\r\n", cmd->text1.len,
            cmd->text1.buf);

        return;
    }    

    print_str("FI: %.*s = %.*s\r\n", cmd->text1.len, cmd->text1.buf,
              cmd->text2.len, cmd->text2.buf);
}


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

    if ((env = getenv("TECO_PROMPT")) != NULL)
    {
        prompt = env;                   // Change TECO prompt
    }

    if (getenv("TECO_DEBUG") != NULL)
    {
        f.e0.dryrun = true;
    }

    read_value("TECO_MACRO_MAX", &macro_max);
    read_value("TECO_LOOP_MAX",  &loop_max);
    read_value("TECO_QREG_MAX",  &qreg_max);
}


///
///  @brief    Read environment variable and set value if valid.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_value(const char *var, uint *value)
{
    assert(var != NULL);
    assert(value != NULL);

    const char *env;
    char *endptr;
    long n;

    if ((env = getenv(var)) != NULL)
    {
        errno = 0;
        n = strtol(env, &endptr, 10);

        if (errno == 0 && n >= 0 && n != LONG_MAX)
        {
            *value = (uint)n;
        }        
    }
}


///
///  @brief    Get information environment about our environment.
///
///  @returns  Result for requested value.
///
////////////////////////////////////////////////////////////////////////////////

int teco_env(int n_arg, bool colon)
{
    if (n_arg == -1)
    {
        if (colon)
        {
            print_str("Linux,x86,64-bit\r\n");
        }

        return (TECO_HW << 8) | TECO_OS;
    }
    else if (n_arg == 0)
    {
        if (colon)
        {
            print_str("%u\r\n", (uint)getpid());
        }

        return getpid();
    }
    else
    {
        if (colon)
        {
            print_str("0\r\n");
        }

        return 0;
    }
}
