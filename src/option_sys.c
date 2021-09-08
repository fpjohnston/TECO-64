///
///  @file    option_sys.c
///  @brief   Process command-line options for TECO editor.
///
///  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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
#include <getopt.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "file.h"
#include "term.h"

#include "cbuf.h"
#include "options.h"


///
///   @struct  options
///
///   @brief   Structure for holding information on configuration options.
///

struct options
{
    const char *args;       ///< --arguments (m,n)
    bool create;            ///< --create
    bool display;           ///< --display
    const char *execute;    ///< --execute
    bool exit;              ///< --exit
    bool formfeed;          ///< --formfeed
    const char *initial;    ///< --initial
    char *log;              ///< --log
    const char *memory;     ///< --memory
    char *output;           ///< --output
    const char *scroll;     ///< --scroll
    bool readonly;          ///< --readonly
    char *text;             ///< --text
    const char *vtedit;     ///< --vtedit
    const char *zero;       ///< --zero
};

///
///   @var     options
///
///   @brief   Current configuration options.
///

static struct options options =
{
    .args     = NULL,
    .create   = true,
    .display  = false,
    .execute  = NULL,
    .exit     = false,
    .formfeed = false,
    .initial  = NULL,
    .log      = NULL,
    .memory   = NULL,
    .output   = NULL,
    .readonly = false,
    .scroll   = NULL,
    .text     = NULL,
    .vtedit   = NULL,
    .zero     = NULL,
};


// Local functions

static void add_cmd(int mnflag, const char *format, ...);


///
///  @brief    Add command to command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void add_cmd(int mnflag, const char *format, ...)
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
        else if (mnflag && options.args != NULL)
        {
            snprintf(cmd, size, "%sEI%s\e ", options.args, p);
        }
        else
        {
            snprintf(cmd, size, "EI%s\e ", p);
        }
    }
    else
    {
        vsnprintf(cmd, size, format, args);
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
///            Note that we use tprint() instead of printf() here because we
///            are called after terminal characteristics have been changed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_options(int argc, const char * const argv[])
{
    assert(argv != NULL);               // Error if no argument list

    // Process commands that don't open a file for editing.

    if (options.initial)  add_cmd(false, NULL,      options.initial);
    if (options.zero)     add_cmd(false, "%sE2",    options.zero);
    if (options.log)      add_cmd(false, "EL%s\e ", options.log);
    if (options.text)     add_cmd(false, "I%s\e ",  options.text);
    if (options.execute)  add_cmd(true,  NULL,      options.execute);
    if (options.formfeed) add_cmd(false, "0,1E3 ",  NULL);

    // Don't enable display mode if we're exiting immediately after execution.

    if (!options.exit)
    {
        if (options.display) add_cmd(false, "-1W ",      NULL);
        if (options.vtedit)  add_cmd(false, NULL,        options.vtedit);
        if (options.scroll)  add_cmd(false, "%s,7:W \e", options.scroll);
    }

    // file1 may be an input or output file, depending on the options used.
    // file2 is always an output file.

    const char *file1 = NULL;
    const char *file2 = options.output ?: NULL;
    char memory[PATH_MAX];              // File name from memory file

    if (optind < argc - 1)
    {
        tprint("Too many file arguments\n");

        exit(EXIT_FAILURE);
    }
    else if (optind < argc)
    {
        file1 = argv[optind];
    }
    else if (options.memory != NULL)
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
        if (file2 != NULL || options.readonly)
        {
            add_cmd(false, "ER%s\e Y ", file1);

            if (!f.e3.noinfo)
            {
                add_cmd(false, ":^A%%Inspecting file: %s\1 ", file1);
            }
        }
        else if (access(file1, F_OK) == 0 || !options.create)
        {
            add_cmd(false, "EB%s\e Y ", file1);

            if (!f.e3.noinfo)
            {
                add_cmd(false, ":^A%%Editing file: %s\1 ", file1);
            }
        }
        else
        {
            file2 = file1;
        }
    }

    if (file2 != NULL)
    {
        add_cmd(false, "EW%s\e ", file2);

        if (!f.e3.noinfo)
        {
            add_cmd(false, ":^A%%Creating file: %s\1 ", file2);
        }
    }

    if (options.exit)                    // Should we exit at end of commands?
    {
        add_cmd(false, "EX ");
    }

    if (cbuf->len != 0)                 // Anything stored?
    {
        add_cmd(false, "\e\e");
    }
}


///
///  @brief    Process configuration options.
///
///            We can be called to process default options, to process
///            environment variable options, and to process user-specified
///            options.
///
///            Note that we use printf() instead of tprint() here because we
///            are called before terminal characteristics have been changed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_options(
    int argc,                           ///< No. of arguments
    const char *const argv[])           ///< List of arguments
{
    assert(argv != NULL);               // Error if no argument list
    assert(argv[0] != NULL);            // Error if no strings in list

    options.initial = teco_init;
    options.memory  = teco_memory;
    options.vtedit  = teco_vtedit;

    // These two assertions confirm the standard behavior of getopt_long()
    // regarding the ordering of option and non-option arguments.

    assert(optstring[0] != '+' && optstring[0] != '-'); // Verify assumptions
    assert(getenv("POSIXLY_CORRECT") == NULL); // Verify assumptions

    int c;
    int idx = 0;
    bool mung = false;

    optind = 0;                         // Reset from any previous calls
    opterr = 0;                         // Suppress any error messages

    while ((c = getopt_long(argc, (char * const *)argv,
                            optstring, long_options, &idx)) != -1)
    {
        switch (c)
        {
            case CTRL_M:
                mung = true;

                break;

            case OPTION_A:
                if (optarg != NULL)
                {
                    const char *p = optarg;
                    int nbytes;

                    if (sscanf(p, "%d%n", &c, &nbytes) == 1)
                    {
                        p += nbytes;

                        if (*p == ',')
                        {
                            ++p;

                            if (sscanf(p, "%d%n", &c, &nbytes) == 1)
                            {
                                p += nbytes;
                            }
                        }
                    }

                    if (*p != NUL)
                    {
                        printf("Invalid value '%s' for --argument "
                               "option\n", optarg);

                        exit(EXIT_FAILURE);
                    }

                    options.args = optarg;
                }
                else
                {
                    options.args = NULL;
                }

                break;

            case OPTION_T:
                if (optarg != NULL)
                {
                    options.text = optarg;
                }
                else
                {
                    options.text = NULL;
                }

                break;

            case OPTION_C:
            case OPTION_c:
                options.create = (c == 'C') ? true : false;

                break;

            case OPTION_D:
                options.display = true;

                break;

            case OPTION_E:
                if (optarg != NULL)
                {
                    if (optarg[0] == '-')
                    {
                        printf("Invalid file name for %s option\n",
                               argv[optind - 2]);

                        exit(EXIT_FAILURE);
                    }

                    options.execute = optarg;
                }
                else
                {
                    options.execute = NULL;
                }

                teco_memory = NULL;

                break;

            case OPTION_F:
            case OPTION_f:
                options.formfeed = (c == 'F') ? true : false;

                break;

            case OPTION_H:
            {
                const char *p;
                uint i = 0;

                while ((p = help_text[i++]) != NULL)
                {
                    printf("%s\n", p);
                }

                exit(EXIT_SUCCESS);
            }

            case OPTION_I:
                if (optarg != NULL && optarg[0] != '-')
                {
                    options.initial = optarg;
                }
                else
                {
                    options.initial = teco_init;
                }

                break;

            case OPTION_i:
                options.initial = NULL;

                break;

            case OPTION_K:
                if (optarg != NULL && optarg[0] != '-')
                {
                    key_name = optarg;
                }
                else
                {
                    key_name = NULL;
                }

                break;

            case OPTION_L:
                if (optarg != NULL && optarg[0] != '-')
                {
                    options.log = optarg;
                }
                else
                {
                    options.log = NULL;
                }

                break;

            case OPTION_M:
                if (optarg != NULL)
                {
                    options.memory = optarg;
                }
                else
                {
                    options.memory = teco_memory;
                }

                break;

            case OPTION_m:
                options.memory = NULL;

                break;

            case OPTION_n:
                options.create  = false;
                options.initial = NULL;
                options.memory  = NULL;
                options.vtedit  = NULL;

                break;

            case OPTION_O:
                if (optarg != NULL && optarg[0] != '-')
                {
                    options.output = optarg;
                }
                else
                {
                    options.output = NULL;
                }

                break;

            case OPTION_o:
                options.output = NULL;

                break;

            case OPTION_R:
            case OPTION_r:
                options.readonly = (c == 'R') ? true : false;

                break;

            case OPTION_S:
                if (optarg != NULL && optarg[0] != '-')
                {
                    options.scroll = optarg;
                }
                else
                {
                    options.scroll = NULL;
                }

                options.display = true;

                break;

            case OPTION_V:
                if (optarg != NULL)
                {
                    options.vtedit = optarg;
                }
                else
                {
                    options.vtedit = teco_vtedit;
                }

                break;

            case OPTION_v:
                options.vtedit = NULL;

                break;

            case OPTION_X:
                options.exit = true;

                break;

            case OPTION_Z:
                if (optarg != NULL)
                {
                    options.zero = optarg;
                }
                else
                {
                    options.zero = "-1";
                }

                break;

            case ':':
                printf("%s option requires file option\n", argv[optind - 1]);

                exit(EXIT_FAILURE);

            default:
                printf("Unknown option '%s': use --help for list of "
                       "options\n", argv[optind - 1]);

                exit(EXIT_FAILURE);
        }
    }

    // Disable display mode if stdin has been redirected

    if (f.e0.i_redir)
    {
        options.vtedit = NULL;
    }

    if (mung && argv[optind] != NULL)
    {
        options.execute = argv[optind++];

        teco_memory = NULL;
    }
}
