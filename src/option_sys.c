///
///  @file    option_sys.c
///  @brief   Process command-line options for TECO editor.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "file.h"

#include "_options.h"


///
///   @struct  config
///
///   @brief   Structure for holding information on configuration options.
///

struct config
{
    struct
    {
        bool argument;          ///< --argument option seen
        bool buffer;            ///< --buffer option seen
        bool create;            ///< --create option seen
        bool execute;           ///< --execute=file option seen
        bool help;              ///< --help option seen
        bool initial;           ///< --initial=file option seen
        bool log;               ///< --log=file option seen
        bool memory;            ///< --memory option seen
        bool output;            ///< --output option seen
        bool readonly;          ///< --readonly option seen
        bool scroll;            ///< --scroll option seen
        bool window;            ///< --window option seen
    } f;                        ///< true/false flags

    int n;                      ///< Numeric value for --argument option

    struct
    {
        char *buffer;           ///< String argument for --buffer option
        char *execute;          ///< String argument for --execute option
        char *initial;          ///< String argument for --initial option
        char *log;              ///< String argument for --log option
        char *output;           ///< String argument for --output option
        char *scroll;           ///< String argument for --scroll option
    } s;                        ///< String arguments
};

///
///   @var     config
///
///   @brief   Current configuration options.
///

static struct config config =
{
    .f =
    {
        .argument = false,
        .buffer   = false,
        .create   = true,
        .execute  = false,
        .help     = false,
        .initial  = true,
        .log      = false,
        .memory   = true,
        .output   = false,
        .readonly = false,
        .scroll   = false,
        .window   = false,
    },

    .n = 0,

    .s =
    {
        .buffer  = NULL,
        .execute = NULL,
        .initial = NULL,
        .log     = NULL,
        .output  = NULL,
        .scroll  = NULL,
    },
};


// Local functions

static void check_config(void);

static void copy_arg(char *buf, ulong size, char *p);

static void finish_config(int argc, const char * const argv[]);

static void print_help(void);

static void store_cmd(const char *str);


///
///  @brief    Check configuration options requiring arguments. We do the check
///            here rather than in set_config() in order to minimize duplication
///            of effort for errors that can occur in multiple places.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void check_config(void)
{
    const char *option;

    if (config.f.execute && config.s.execute == NULL)
    {
        option = "--execute";
    }
    else if (config.f.log && config.s.log == NULL)
    {
        option = "--log";
    }
    else if (config.f.output && config.s.output == NULL)
    {
        option = "--output";
    }
    else if (config.f.scroll)
    {
        option = "--scroll";

        if (config.s.scroll != NULL)
        {
            char *endptr;

            (void)strtoul(config.s.scroll, &endptr, 10);

            if (errno == 0)
            {
                if (*endptr == NUL)
                {
                    return;
                }

                errno = EINVAL;
            }

            printf("?%s for %s option\r\n", strerror(errno), option);

            exit(EXIT_FAILURE);
        }
    }
    else if (config.f.buffer && config.s.buffer == NULL)
    {
        option = "--buffer";
    }
    else
    {
        return;
    }

    printf("?Missing argument for %s option?\r\n", option);

    exit(EXIT_FAILURE);
}


///
///  @brief    Copy argument, which is either a file name which we will open
///            with an EI command, or a command string in single or double
///            quotes.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void copy_arg(char *cmd, ulong size, char *file)
{
    assert(cmd != NULL);                // Error if no command block
    assert(file != NULL);               // Error if no file name

    int len = (int)strlen(file);

    if (len > 2 &&
        ((file[0] == '"' && file[len - 1] == '"') ||
         (file[0] == '"' && file[len - 1] == '"')))
    {
        snprintf(cmd, size, "%.*s", len - 2, file + 1);
    }
    else
    {
        snprintf(cmd, size, "EI%s\e ", file);
    }
}


///
///  @brief    Process the configuration options we just parsed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void finish_config(int argc, const char * const argv[])
{
    assert(argv != NULL);               // Error if no argument list

    if ((argc -= optind) > 1)
    {
        printf("?Too many non-option arguments\r\n");

        exit(EXIT_FAILURE);
    }

    // The following needs to be more than twice as big as the longest possible
    // file name, because it might have to contain two copies of a file name in
    // addition to other text.

    char cmdstring[PATH_MAX * 2 + 100];
    char *env;

    if (config.f.help)
    {
        print_help();
    }

    //  Process --initial and --noinitial options.
    //
    //  --initial is the default if neither is specified.
    //
    //  If --initial=file, open specified initialization file.
    //  If --initial, open initialization file specified by TECO_INIT.
    //  If --noinitial, don't open an initialization file.
    //
    //  Note that if the environment variable value is enclosed in double
    //  quotes, it is treated as a string of commands rather than a file name.

    if (config.s.initial != NULL)
    {
        copy_arg(cmdstring, sizeof(cmdstring), config.s.initial);
        store_cmd(cmdstring);
    }
    else if (config.f.initial && (env = getenv("TECO_INIT")) != NULL)
    {
        copy_arg(cmdstring, sizeof(cmdstring), env);
        store_cmd(cmdstring);
    }

    if (config.s.log != NULL)
    {
        snprintf(cmdstring, sizeof(cmdstring), "EL%s\e ", config.s.log);
        store_cmd(cmdstring);
    }

    if (config.s.buffer != NULL)
    {
        snprintf(cmdstring, sizeof(cmdstring), "I%s\e ", config.s.buffer);
        store_cmd(cmdstring);
    }

    if (config.f.argument)
    {
        snprintf(cmdstring, sizeof(cmdstring), "%dUA ", config.n);
        store_cmd(cmdstring);
    }

    if (config.s.execute != NULL)
    {
        copy_arg(cmdstring, sizeof(cmdstring), config.s.execute);
        store_cmd(cmdstring);
    }

    if (config.f.window != 0)
    {
        store_cmd("1W ");
    }

    if (config.s.scroll != NULL)
    {
        snprintf(cmdstring, sizeof(cmdstring), "%s,7:W \e", config.s.scroll);
        store_cmd(cmdstring);
    }

    const char *file = NULL;
    char memory[PATH_MAX];              // File name from memory file

    if (argc == 1)
    {
        file = argv[optind];
    }
    else if (config.f.memory)
    {
        read_memory(memory, (uint)sizeof(memory));

        if (memory[0] != NUL)
        {
            file = memory;
        }
    }

    //  If a file was specified, then there are the following possibilities:
    //
    //  1. File exists and --output was seen, so we do separate ER and EW commands.
    //  2. File exists and --readonly was seen, so we do an ER command.
    //  3. File exists and --readonly was not seen, so we do an EB command.
    //  4. File does not exist and --create was seen, so we do an EW command.
    //  5. File does not exist and either --create was not seen, or --readonly
    //     was seen, or --output was seen, in which case we print an error and exit.
    //

    if (file != NULL)                   // Do we have something to read?
    {
        if (access(file, F_OK) == 0)    // Does file exist?
        {
            if (config.s.output != NULL)
            {
                snprintf(cmdstring, sizeof(cmdstring), "ER%s\e EW%s\e Y ",
                         file, config.s.output);
            }
            else if (config.f.readonly)
            {
                snprintf(cmdstring, sizeof(cmdstring),
                         ":^A%%Inspecting file '%s'%c ER%s\e Y ",
                        file, CTRL_A, file);
            }
            else
            {
                snprintf(cmdstring, sizeof(cmdstring),
                         ":^A%%Editing file '%s'%c EB%s\e Y ",
                        file, CTRL_A, file);
            }
        }
        else if (config.f.create && !config.f.readonly &&
                 !config.f.output)
        {
            snprintf(cmdstring, sizeof(cmdstring),
                    ":^A%%Can't find file '%s'%c :^A%%Creating "
                    "new file%c EW%s\e ", file, CTRL_A, CTRL_A, file);
        }
        else
        {
            snprintf(cmdstring, sizeof(cmdstring),
                     ":^A?Can't find file '%s'%c EX ", file, CTRL_A);
        }

        store_cmd(cmdstring);
    }

    if (command->len != 0)              // Anything stored?
    {
        store_cmd("\e\e");              // If so, properly terminate command
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
    int c;

    while ((p = help_text[i++]) != NULL)
    {
        while ((c = *p++) != NUL)
        {
            fputc(c, stdout);
        }

        fputc(CR, stdout);
        fputc(LF, stdout);
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
                config.f.argument = true;
                config.n = (int)strtol(optarg, NULL, 10);

                break;

            case OPTION_B:
                config.f.buffer = true;
                config.s.buffer = optarg;

                break;

            case OPTION_C:
            case OPTION_c:
                config.f.create = (c == 'C') ? true : false;

                break;

            case OPTION_E:
                config.f.execute = true;
                config.s.execute = optarg;

                // TECO_MEMORY is used to determine the name of a file to edit
                // if none is specified on the command line. Ensure that we
                // don't use it if we are executing an indirect command file.

                (void)unsetenv("TECO_MEMORY");

                break;

            case OPTION_H:
                config.f.help = true;

                break;

            case OPTION_I:
                config.f.initial = true;

                if (optarg != NULL && optarg[0] != '-')
                {
                    config.s.initial = optarg;
                }

                break;

            case OPTION_i:
                config.f.initial = false;

                break;

            case OPTION_L:
                config.f.log = true;
                config.s.log = optarg;

                break;

            case OPTION_M:
                config.f.memory = true;

                break;

            case OPTION_m:
                config.f.memory = false;

                break;

            case OPTION_O:
                config.f.output = true;
                config.s.output = optarg;

                break;

            case OPTION_o:
                config.f.output = false;

                break;

            case OPTION_R:
            case OPTION_r:
                config.f.readonly = (c == 'R') ? true : false;

                break;

            case OPTION_S:
                config.f.scroll = true;
                config.s.scroll = optarg;
                //lint -fallthrough

            case OPTION_W:
                config.f.window = true;

                break;

            case OPTION_X:
                f.e0.exit = true;

                break;

            case OPTION_Z:
                if (optarg != NULL)
                {
                    f.e2.flag = (int)strtol(optarg, NULL, 10);
                }
                else
                {
                    f.e2.flag = -1;
                }

                break;

            default:
                printf("%%Unknown option '%s': use --help for list of "
                       "options\r\n", argv[optind - 1]);

                exit(EXIT_FAILURE);
        }

        check_config();
    }

    finish_config(argc, argv);
}


///
///  @brief    Store command-line option in command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void store_cmd(const char *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    int c;

    while ((c = *cmd++) != NUL)
    {
        store_cbuf(c);
    }
}
