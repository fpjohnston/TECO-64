///
///  @file    option_sys.c
///  @brief   Process command-line options for TECO editor.
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include <ctype.h>
#include <getopt.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "cbuf.h"
#include "eflags.h"
#include "file.h"
#include "term.h"

#include "options.h"


///
///   @struct  config
///
///   @brief   Structure for holding information on configuration options.
///

struct config
{
    int n_arg;              ///< --argument value
    bool n_set;             ///< --argument flag
    char *buffer;           ///< --buffer
    bool create;            ///< --create
    bool display;           ///< --display
    char *execute;          ///< --execute
    bool exit;              ///< --exit
    bool formfeed;          ///< --formfeed
    bool help;              ///< --help
    const char *initial;    ///< --initial
    char *log;              ///< --log
    const char *memory;     ///< --memory
    char *output;           ///< --output
    const char *scroll;     ///< --scroll
    bool readonly;          ///< --readonly
    const char *vtedit;     ///< --vtedit
    const char *zero;       ///< --zero
};

///
///   @var     config
///
///   @brief   Current configuration options.
///

static struct config config =
{
    .n_arg    = 0,
    .n_set    = false,
    .buffer   = NULL,
    .create   = true,
    .display  = false,
    .execute  = NULL,
    .exit     = false,
    .formfeed = false,
    .help     = false,
    .initial  = NULL,
    .log      = NULL,
    .memory   = NULL,
    .output   = NULL,
    .readonly = false,
    .scroll   = NULL,
    .vtedit   = NULL,
    .zero     = NULL,
};


// Local functions

static void add_cmd(const char *format, ...);

static void exec_config(int argc, const char * const argv[]);

static void print_help(void);


///
///  @brief    Add command to command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void add_cmd(const char *format, ...)
{
    // The following needs to be more than twice as big as the longest
    // possible file name, because it might have to contain two copies
    // of a file name in addition to other text.

    char cmd[PATH_MAX * 2 + 100];
    size_t size = sizeof(cmd);
    va_list args;

    va_start(args, format);

    if (format == NULL)
    {
        const char *p = va_arg(args, const char *);
        int len = (int)strlen(p);

        if (len > 2 &&
            ((p[0] == '"' && p[len - 1] == '"') ||
             (p[0] == '\'' && p[len - 1] == '\'')))
        {
            snprintf(cmd, size, "%.*s", len - 2, p + 1);
        }
        else
        {
            snprintf(cmd, size, "EI%s\e ", p);
        }
    }
    else
    {
        (void)vsnprintf(cmd, size, format, args);
    }

    va_end(args);

    int c;
    const char *p = cmd;

    while ((c = *p++) != NUL)
    {
        store_cbuf(c);
    }
}


///
///  @brief    Process the configuration options we just parsed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_config(int argc, const char * const argv[])
{
    assert(argv != NULL);               // Error if no argument list

    if (config.help)
    {
        print_help();
    }

    // Process commands that don't open a file for editing.

    if (config.initial)  add_cmd(NULL,        config.initial);
    if (config.zero)     add_cmd("%sE2",      config.zero);
    if (config.log)      add_cmd("EL%s\e ",   config.log);
    if (config.buffer)   add_cmd("I%s\e ",    config.buffer);
    if (config.n_set)    add_cmd("%dUA",      config.n_arg);
    if (config.execute)  add_cmd(NULL,        config.execute);
    if (config.display)  add_cmd("-1W ",      NULL);
    if (config.vtedit)   add_cmd(NULL,        config.vtedit);
    if (config.scroll)   add_cmd("%s,7:W \e", config.scroll);
    if (config.formfeed) add_cmd("0,1E3 ",    NULL);

    // file1 may be an input or output file, depending on the options used.
    // file2 is always an output file.

    const char *file1 = NULL;
    const char *file2 = config.output ?: NULL;
    char memory[PATH_MAX];              // File name from memory file

    if (optind < argc - 1)
    {
        tprint("%%Too many file arguments\r\n");

        exit(EXIT_FAILURE);
    }
    else if (optind < argc)
    {
        file1 = argv[optind];
    }
    else if (config.memory != NULL)
    {
        read_memory(memory, (uint)sizeof(memory));

        if (memory[0] != NUL)
        {
            file1 = memory;
        }
    }

    // Here to figure out which file commands to use. Note that if a file
    // open fails, the rest of the command string will be aborted, which
    // means that the subsequent CTRL/A command won't be executed.

    if (file1 != NULL)
    {
        if (file2 != NULL || config.readonly)
        {
            add_cmd("ER%s\e Y ", file1);
            add_cmd(":^A%%Inspecting file: %s\1 ", file1);
        }
        else if (access(file1, F_OK) == 0 || !config.create)
        {
            add_cmd("EB%s\e Y ", file1);
            add_cmd(":^A%%Editing file: %s\1 ", file1);
        }
        else
        {
            file2 = file1;
        }
    }

    if (file2 != NULL)
    {
        add_cmd("EW%s\e ", file2);
        add_cmd(":^A%%Creating file: %s\1 ", file2);
    }

    if (config.exit)                    // Should we exit at end of commands?
    {
        add_cmd("EX ");
    }

    if (cbuf->len != 0)                 // Anything stored?
    {
        add_cmd("\e\e");
    }
}


///
///  @brief    Print help message.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void print_help(void)
{
    const char *p;
    uint i = 0;

    while ((p = help_text[i++]) != NULL)
    {
        tprint("%s\r\n", p);
    }

    exit(EXIT_SUCCESS);
}


///
///  @brief    Process configuration options.
///
///            We can be called to process default options, to process
///            environment variable options, and to process user-specified
///            options.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_config(
    int argc,                           ///< No. of arguments
    const char *const argv[])           ///< List of arguments
{
    assert(argv != NULL);               // Error if no argument list
    assert(argv[0] != NULL);            // Error if no strings in list

    config.initial = teco_init;
    config.memory  = teco_memory;
    config.vtedit  = teco_vtedit;

    // These two assertions confirm the standard behavior of getopt_long()
    // regarding the ordering of option and non-option arguments.

    assert(optstring[0] != '+' && optstring[0] != '-'); // Verify assumptions
    assert(getenv("POSIXLY_CORRECT") == NULL); // Verify assumptions

    int c;
    int idx = 0;

    optind = 0;                         // Reset from any previous calls
    opterr = 0;                         // Suppress any error messages

    while ((c = getopt_long(argc, (char * const *)argv,
                            optstring, long_options, &idx)) != -1)
    {
        switch (c)
        {
            case OPTION_A:
                config.n_set = true;
                config.n_arg = (int)strtol(optarg, NULL, 10);

                break;

            case OPTION_B:
                if (optarg != NULL)
                {
                    config.buffer = optarg;
                }
                else
                {
                    config.buffer = NULL;
                }

                break;

            case OPTION_C:
            case OPTION_c:
                config.create = (c == 'C') ? true : false;

                break;

            case OPTION_D:
                config.display = true;

                break;

            case OPTION_E:
                if (optarg != NULL && optarg[0] != '-')
                {
                    config.execute = optarg;
                }
                else
                {
                    config.execute = NULL;
                }

                teco_memory = NULL;     // TODO: is this necessary?

                break;

            case OPTION_F:
            case OPTION_f:
                config.formfeed = (c == 'F') ? true : false;

                break;

            case OPTION_H:
                config.help = true;

                break;

            case OPTION_I:
                if (optarg != NULL && optarg[0] != '-')
                {
                    config.initial = optarg;
                }
                else
                {
                    config.initial = teco_init;
                }

                break;

            case OPTION_i:
                config.initial = NULL;

                break;

            case OPTION_L:
                if (optarg != NULL && optarg[0] != '-')
                {
                    config.log = optarg;
                }
                else
                {
                    config.log = NULL;
                }

                break;

            case OPTION_M:
                if (optarg != NULL)
                {
                    config.memory = optarg;
                }
                else
                {
                    config.memory = teco_memory;
                }

                break;

            case OPTION_m:
                config.memory = NULL;

                break;

            case OPTION_O:
                if (optarg != NULL && optarg[0] != '-')
                {
                    config.output = optarg;
                }
                else
                {
                    config.output = NULL;
                }

                break;

            case OPTION_o:
                config.output = NULL;

                break;

            case OPTION_R:
            case OPTION_r:
                config.readonly = (c == 'R') ? true : false;

                break;

            case OPTION_S:
                if (optarg != NULL && optarg[0] != '-')
                {
                    config.scroll = optarg;
                }
                else
                {
                    config.scroll = NULL;
                }

                config.display = true;

                break;

            case OPTION_V:
                if (optarg != NULL)
                {
                    config.vtedit = optarg;
                }
                else
                {
                    config.vtedit = teco_vtedit;
                }

                break;

            case OPTION_v:
                config.vtedit = NULL;

                break;

            case OPTION_X:
                config.exit = true;

                break;

            case OPTION_Z:
                if (optarg != NULL)
                {
                    config.zero = optarg;
                }
                else
                {
                    config.zero = "-1";
                }

                break;

            default:
                tprint("%%Unknown option '%s': use --help for list of "
                        "options\r\n", argv[optind - 1]);

                exit(EXIT_FAILURE);
        }
    }

    exec_config(argc, argv);
}
