///
///  @file    option_sys.c
///  @brief   Process command-line options for TECO editor.
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

// TODO: add --help option

///  @enum     option_t
///  case values for command-line options.

enum option_t
{
    OPTION_C = 'C',
    OPTION_c = 'c',
    OPTION_D = 'D',
    OPTION_E = 'E',
    OPTION_I = 'I',
    OPTION_i = 'i',
    OPTION_L = 'L',
    OPTION_M = 'M',
    OPTION_m = 'm',
    OPTION_R = 'R',
    OPTION_r = 'r',
    OPTION_O = 'O',
    OPTION_o = 'o',
    OPTION_S = 'S',
    OPTION_T = 'T',
    OPTION_W = 'W',
    OPTION_X = 'X'
};

///  @var optstring
///  String of short options parsed by getopt_long().

static const char * const optstring = ":CcDE:I::iL:MmO:oRrS:T:WX";

///  @var    long_options[]
///  @brief  Table of command-line options parsed by getopt_long().

static const struct option long_options[] =
{
    { "create",     no_argument,        NULL,  'C'    },
    { "nocreate",   no_argument,        NULL,  'c'    },
    { "dry-run",    no_argument,        NULL,  'D'    },
    { "execute",    required_argument,  NULL,  'E'    },
    { "initial",    optional_argument,  NULL,  'I'    },
    { "noinitial",  no_argument,        NULL,  'i'    },
    { "log",        required_argument,  NULL,  'L'    },
    { "memory",     no_argument,        NULL,  'M'    },
    { "nomemory",   no_argument,        NULL,  'm'    },
    { "output",     required_argument,  NULL,  'O'    },
    { "nooutput",   no_argument,        NULL,  'o'    },
    { "readonly",   no_argument,        NULL,  'R'    },
    { "noreadonly", no_argument,        NULL,  'r'    },
    { "scroll",     required_argument,  NULL,  'S'    },
    { "text",       required_argument,  NULL,  'T'    },
    { "window",     no_argument,        NULL,  'W'    },
    { "exit",       no_argument,        NULL,  'X'    },
    { NULL,         no_argument,        NULL,  0      },  // Markers for end of list
};

//  TECO options
//  ------------
//  -C, --create
//          Create a new file if the input file does not exist.
//  -c, --nocreate
//          Do not create a new file if no input file.
//  -D, --dry-run
//          Do not execute commands in indirect files.
//  -E, --execute=file
//          Executes TECO macro in file.
//  -I, --initial=file (default file or commands specified by TECO_INIT).
//          Specifies file to be executed at startup.
//  -i, --noinitial
//          Don't use a startup file (ignore TECO_INIT).
//  -L, --log=file
//          Saves input and output in log file.
//  -M, --memory (default)
//          Use TECO_MEMORY to get name of last file edited.
//  -m, --nomemory
//          Don't use TECO_MEMORY.
//  -O, --output=file
//          Specify name of output file.
//  -o, --nooutput (default)
//          Use same name for output file as input file.
//  -R, --read-only
//          Don't create output file.
//  -r, --noread-only (default)
//          Create an output file.
//  -S, --scroll
//          Enable scrolling region (implies --W).
//  -W, --window
//          Enable window mode.
//  -X, --exit
//          Exit TECO after indirect command file executed.

///   @var    config
///
///   @brief  Structure for holding information on configuration options.
///

struct config
{
    struct
    {
        bool create;            ///< --create option seen
        bool dry_run;           ///< --dry-run option seen
        bool execute;           ///< --execute=file option seen
        bool initial;           ///< --initial=file option seen
        bool log;               ///< --log=file option seen
        bool memory;            ///< --memory option seen
        bool output;            ///< --output option seen
        bool readonly;          ///< --readonly option seen
        bool scroll;            ///< --scroll option seen
        bool text;              ///< --text option seen
        bool window;            ///< --window option seen
        bool exit;              ///< --exit option seen
    } flag;
    struct
    {
        const char *execute;    ///< Argument for --execute option
        const char *initial;    ///< Argument for --initial option
        const char *log;        ///< Argument for --log option
        const char *output;     ///< Argument for --output option
        const char *scroll;     ///< Argument for --scroll option
        const char *text;       ///< Argument for --text option
    } arg;
};

struct config config =
{
    .flag =
    {
        .create   = true,
        .dry_run  = false,
        .execute  = false,
        .initial  = true,
        .log      = false,
        .memory   = true,
        .output   = false,
        .readonly = false,
        .scroll   = false,
        .text     = false,
        .window   = false,
        .exit     = false,
    },
    .arg =
    {
        .execute = NULL,
        .initial = NULL,
        .log     = NULL,
        .output  = NULL,
        .scroll  = NULL,
        .text    = NULL,
    },
};
 
// Local functions

static void check_config(void);
    
static void finish_config(int argc, const char * const argv[]);

static void read_memory(char *p, uint len);

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

    if (config.flag.execute && config.arg.execute == NULL)
    {
        option = "--execute";
    }
    else if (config.flag.log && config.arg.log == NULL)
    {
        option = "--log";
    }
    else if (config.flag.output && config.arg.output == NULL)
    {
        option = "--output";
    }
    else if (config.flag.scroll)
    {
        option = "--scroll";

        if (config.arg.scroll != NULL)
        {
            char *endptr;

            (void)strtoul(config.arg.scroll, &endptr, 10);

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
    else if (config.flag.text && config.arg.text == NULL)
    {
        option = "--text";
    }
    else
    {
        return;
    }

    printf("?Missing argument for %s option?\r\n", option);

    exit(EXIT_FAILURE);
}


///
///  @brief    Done reading configuration options; now process everything.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void finish_config(int argc, const char * const argv[])
{
    assert(argv != NULL);

    if ((argc -= optind) > 1)
    {
        printf("?Too many non-option arguments\r\n");

        exit(EXIT_FAILURE);
    }

    char scratch[PATH_MAX];
    char *env;

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
   
    if (config.arg.initial != NULL)
    {
        sprintf(scratch, "@EI|%s| ", config.arg.initial);
        store_cmd(scratch);
    }
    else if (config.flag.initial && (env = getenv("TECO_INIT")) != NULL)
    {
        int len = (int)strlen(env);

        if (len > 2 && env[0] == '"' && env[len - 1] == '"')
        {
            sprintf(scratch, "%.*s", len - 2, env + 1);
        }
        else
        {
            sprintf(scratch, "@EI|%s| ", env);
        }

        store_cmd(scratch);
    }

    if (config.flag.dry_run)
    {
        f.e0.dryrun = true;
    }

    if (config.arg.log != NULL)
    {
        sprintf(scratch, "@EL|%s|", config.arg.log);
        store_cmd(scratch);
    }

    if (config.arg.execute != NULL)
    {
        sprintf(scratch, "@EI|%s|", config.arg.execute);
        store_cmd(scratch);
    }

    if (config.arg.text != NULL)
    {
        sprintf(scratch, "@I|%s|", config.arg.text);
        store_cmd(scratch);
    }

    if (config.flag.exit)
    {
        sprintf(scratch, "EX");
        store_cmd(scratch);
    }

    if (config.flag.window != 0)
    {
        sprintf(scratch, "1W");
        store_cmd(scratch);
    }

    if (config.arg.scroll != NULL)
    {
        sprintf(scratch, "%s,7:W", config.arg.scroll);
        store_cmd(scratch);
    }

    const char *file = NULL;
    char memory[PATH_MAX + 1];

    if (argc == 1)
    {
        file = argv[optind];
    }
    else if (config.flag.memory)
    {
        read_memory(memory, (uint)sizeof(memory));

        if (memory[0] != NUL)
        {
            file = memory;
        }
    }

    if (file != NULL)                   // Do we have something to read?
    {
        if (config.arg.output != NULL)
        {
            sprintf(scratch, "@ER|%s| @EW|%s| Y", file, config.arg.output);
        }
        else
        {
            if (realpath(file, scratch) != NULL)
            {
                if (config.flag.readonly)
                {
                    sprintf(scratch, "@^A|%%Inspecting file '%s'| 13^T 10^T "
                            "@ER|%s| Y ", file, file);
                }
                else
                {
                    sprintf(scratch, "@^A|%%Editing file '%s'| 13^T 10^T "
                            "@EB|%s| Y", file, file);
                }
            }
            else if (config.flag.create && !config.flag.readonly)
            {
                sprintf(scratch, "@^A|%%Can't find file '%s'| 13^T 10^T "
                        "@^A|%%Creating new file| 13^T 10^T "
                        "@EW|%s|", file, file);
            }
            else
            {
                sprintf(scratch, "@^A|?Can't find file '%s'| 13^T 10^T EX",
                        file);
            }
        }

        store_cmd(scratch);
    }
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
    assert(argv != NULL);
    assert(argv[0] != NULL);

    // These two assertions confirm the standard behavior of getopt_long()
    // regarding the ordering of option and non-option arguments.

    assert(optstring[0] != '+' && optstring[0] != '-');
    assert(getenv("POSIXLY_CORRECT") == NULL);

    int c;
    int idx = 0;

    optind = 0;                         // Reset from any previous calls
    opterr = 0;                         // Suppress any error messages

    while ((c = getopt_long(argc, (char * const *)argv,
                            optstring, long_options, &idx)) != -1)
    {
        switch (c)
        {
            case ':':
                if (optopt == 'E')
                {
                    config.flag.execute = true;
                }
                else if (optopt == 'L')
                {
                    config.flag.log = true;
                }
                else if (optopt == 'O')
                {
                    config.flag.output = true;
                }
                else if (optopt == 'S')
                {
                    config.flag.scroll = true;
                }
                else if (optopt == 'T')
                {
                    config.flag.text = true;
                }
                break;

            case OPTION_C:
            case OPTION_c:
                config.flag.create = (c == 'C') ? true : false;

                break;

            case OPTION_D:
                config.flag.dry_run = true;

                break;

            case OPTION_E:
                config.flag.execute = true;

                if (optarg[0] != '-')
                {
                    config.arg.execute = optarg;
                }

                break;

            case OPTION_I:
                config.flag.initial = true;

                if (optarg != NULL && optarg[0] != '-')
                {
                    config.arg.initial = optarg;
                }

                break;
 
            case OPTION_i:
                config.flag.initial = false;

                break;
 
            case OPTION_L:
                config.flag.log = true;

                if (optarg[0] != '-')
                {
                    config.arg.log = optarg;
                }

                break;

            case OPTION_M:
                config.flag.memory = true;

                break;

            case OPTION_m:
                config.flag.memory = false;

                break;

            case OPTION_O:
                config.flag.output = true;

                if (optarg[0] != '-')
                if (optarg != NULL && optarg[0] != '-')
                {
                    config.arg.output = optarg;
                }

                break;
 
            case OPTION_o:
                config.flag.output = false;

                break;
 
            case OPTION_R:
            case OPTION_r:
                config.flag.readonly = (c == 'R') ? true : false;

                break;

            case OPTION_S:
                config.flag.scroll = true;
                config.flag.window = true;                

                if (optarg[0] != '-')
                {
                    config.arg.scroll = optarg;
                }

                break;

            case OPTION_T:
                config.flag.text = true;

                if (optarg[0] != '-')
                {
                    config.arg.text = optarg;
                }

                break;

            case OPTION_W:
                config.flag.window = true;

                break;

            case OPTION_X:
                config.flag.exit = true;

                break;

            default:
                printf("%%Unknown option '%s'\r\n", argv[optind - 1]);

                exit(EXIT_FAILURE);
        }

        check_config();
    }

    finish_config(argc, argv);
}


///
///  @brief    Read file specification from memory file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_memory(char *p, uint len)
{
    assert(p != NULL);

    const char *memory = getenv("TECO_MEMORY");

    if (memory != NULL)
    {
        FILE *fp = fopen(memory, "r");

        if (fp == NULL)
        {
            if (errno != ENOENT && errno != ENODEV)
            {
                printf("%%Can't open memory file '%s'\r\n", memory);
            }
        }
        else
        {
            int c;

            while (--len > 0 && (c = fgetc(fp)) != EOF && isgraph(c))
            {
                *p++ = (char)c;
            }

            fclose(fp);
        }
    }

    *p = NUL;
}


///
///  @brief    Store command-line option in command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void store_cmd(const char *cmd)
{
    assert(cmd != NULL);

    int c;

#define DEBUG_OPTIONS
#if     defined(DEBUG_OPTIONS)
    
    printf("command: %s\r\n", cmd);

#endif

    while ((c = *cmd++) != NUL)
    {
        store_cbuf(c);
    }
}


///
///  @brief    Write EB or EW file to memory file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void write_memory(const char *file)
{
    assert(file != NULL);
    
    const char *memory = getenv("TECO_MEMORY");
    FILE *fp;

    if (memory == NULL)
    {
        return;
    }

    if ((fp = fopen(memory, "w")) == NULL)
    {
        printf("%%Can't open memory file '%s'\r\n", memory);

        return;
    }

    fprintf(fp, "%s\n", file);

    fclose(fp);

}
