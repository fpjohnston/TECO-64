///
///  @file    option_sys.c
///  @brief   Process command-line options for TECO editor.
///
///  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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
#include <getopt.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>                   // for stat()

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "file.h"
#include "term.h"
#include "version.h"

#include "cbuf.h"
#include "options.h"

#if     defined(DEBUG)

#include <ctype.h>
#include <stdbool.h>

#endif


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
    const char *formfeed;   ///< --formfeed
    const char *initial;    ///< --initial
    char *log;              ///< --log
    const char *memory;     ///< --memory
    const char *scroll;     ///< --scroll
    bool readonly;          ///< --readonly
    char *text;             ///< --text
    const char *vtedit;     ///< --vtedit

#if     defined(DEBUG)

    const char *e1;         ///< --e1 (debug only)
    const char *e2;         ///< --e2 (debug only)
    const char *e3;         ///< --e3 (debug only)
    const char *e4;         ///< --e4 (debug only)
    bool print;             ///< --print (debug only)
    bool quit;              ///< --quit (debug only)

#endif

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
    .formfeed = NULL,
    .initial  = NULL,
    .log      = NULL,
    .memory   = NULL,
    .readonly = false,
    .scroll   = NULL,
    .text     = NULL,
    .vtedit   = NULL,

#if     defined(DEBUG)

    .e1       = NULL,
    .e2       = NULL,
    .e3       = NULL,
    .e4       = NULL,
    .print    = false,
    .quit     = false,

#endif

};

static const char *file1 = NULL;        ///< Input file
static const char *file2 = NULL;        ///< Output file

// Local functions

static bool check_infile(const char *infile, const char *outfile);

static void check_option(const char *format, const char *option);

static void format_EI(const char *ei_args, const char *file);

static int get_info(const char *file, dev_t *dev, ino_t *ino);

#if     defined(DEBUG)

static void print_initial(void);

#endif

static void store_cmd(const char *format, ...);


///
///  @brief    Check which commands we should use for an input file, and whether
///            to read the memory file if no input file provided.
///
///  @returns  true if we found an existing input file, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool check_infile(const char *infile, const char *outfile)
{
    // Here to figure out which file commands to use for an input file. If a
    // file open fails, the rest of the command string will be aborted, which
    // means that the subsequent CTRL/A command won't be executed.

    if (options.readonly && infile == NULL)
    {
        store_cmd(":^A?How can I inspect nothing?\1");

        options.exit = true;
    }
    else if (infile != NULL)
    {
        if (outfile != NULL || options.readonly)
        {
            store_cmd("ER%s\e Y :^AReading file: %s\1", infile, infile);

            if (outfile != NULL)
            {
                teco_memory = NULL;     // Don't save file name on exit
            }
        }
        else if (access(infile, F_OK) == 0 || !options.create)
        {
            store_cmd("EB%s\e Y :^AEditing file: %s\1", infile, infile);
        }
        else                            // File doesn't exist
        {
            return false;               // So it's really an output file
        }
    }

    return true;
}


///
///  @brief    Format a command string and store it in the command buffer. When
///            initialization has completed, the contents of the command buffer
///            will be the first set of commands that TECO will execute.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void check_option(const char *format, const char *option)
{
    assert(format != NULL);

    if (option == NULL)                 // Did user specify this option?
    {
        return;                         // No, so there's nothing to do
    }

    store_cmd(format, option);
}


///
///  @brief    Format an EI command string and store it in the command buffer.
///            This is used to implement the --init, --execute, and --vtedit
///            command options, the arguments for which are usually a file
///            name, but which could be a string of TECO commands, delimited
///            by single or double quotes.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void format_EI(const char *ei_args, const char *file)
{
    if (file == NULL)                   // Did user specify this option?
    {
        return;                         // No, so there's nothing to do
    }

    // If file name starts and ends with single or double quotes, then it's
    // really a TECO command string.

    int len = (int)strlen(file);

    if (len > 2)
    {
        int first = file[0];
        int last = file[len - 1];

        if (first == last && (first == '"' || first == '\''))
        {
            store_cmd("%.*s\e",len - 2, file + 1);

            return;
        }
    }

    // Here if user specified a file and not a command string

    if (ei_args != NULL)
    {
        store_cmd("%sEI%s\e", ei_args, file);
    }
    else
    {
        store_cmd("EI%s\e", file);
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

void exec_options(void) 
{
    format_EI(NULL, options.initial);
    check_option("EL%s\e", options.log);
    check_option("I%s\e",  options.text);
    format_EI(options.args, options.execute);
    check_option("%sE3", options.formfeed);

#if     defined(DEBUG)

    check_option("%sE1", options.e1);
    check_option("%sE2", options.e2);
    check_option("%sE3", options.e3);
    check_option("%sE4", options.e4);

#endif

    // Don't enable display mode if we're exiting immediately after execution.

    if (!options.exit)
    {
        if (options.display)
        {
            store_cmd("-1W");
        }

        format_EI(NULL, options.vtedit);
        check_option("%s,7:W\e", options.scroll);
    }

    // Now process input file (if any) and output file (if any)

    if (file1 == NULL)                  // We have no file names
    {
        if (options.memory != NULL)     // Try to read memory file
        {
            char memory[PATH_MAX];      // File name from memory file

            read_memory(memory, (uint)sizeof(memory));

            if (memory[0] != NUL)
            {
                if (!check_infile(memory, NULL))
                {
                    file2 = file1;
                }
            }
        }
    }
    else if (file2 == NULL)             // We only have one file name
    {
        if (!check_infile(file1, NULL))
        {
            file2 = file1;
        }
    }
    else                                // We have two file names
    {
        dev_t dev1, dev2;
        ino_t ino1, ino2;
        int err1 = get_info(file1, &dev1, &ino1);
        int err2 = get_info(file2, &dev2, &ino2);

        // See if the two files are actually the same. There are basically two
        // possibilities: either both files exist and have the same device IDs
        // and inode numbers, or neither exist, but their file names match. In
        // either case, we skip using the second file name, and just use an EB
        // command to open the file.

        if (err1 == 0 && err2 == 0)
        {
            if (dev1 == dev2 && ino1 == ino2)
            {
                file2 = NULL;
            }
        }
        else if (err1 == ENOENT && err2 == ENOENT)
        {
            if (!strcmp(file1, file2))
            {
                file2 = NULL;
            }
        }

        if (file2 != NULL)
        {
            if (!check_infile(file1, file2))
            {
                tprint("?Input file does not exist\n");

                exit(EXIT_FAILURE);
            }
        }
        else
        {
            if (!check_infile(file1, NULL))
            {
                file2 = file1;
            }
        }
    }

    if (file2 != NULL)
    {
        store_cmd(":^AWriting file: %s\1 EW%s\e", file2, file2);
    }

    if (options.exit)                   // Should we exit at end of commands?
    {
        store_cmd("EX");
    }

    if (cbuf->len != 0)                 // Anything stored?
    {
        store_cmd("\e\e");
    }

#if     defined(DEBUG)

    print_initial();                    // Maybe print initial command string

#endif

}


///
///  @brief    Get file information (device ID and inode number).
///
///  @returns  0 if no error, else errno code.
///
////////////////////////////////////////////////////////////////////////////////

static int get_info(const char *file, dev_t *dev, ino_t *ino)
{
    assert(file != NULL);
    assert(dev != NULL);
    assert(ino != NULL);

    struct stat statbuf;

    errno = 0;

    if (stat(file, &statbuf) == -1)
    {
        *dev = *ino = 0;

        return errno;
    }

    *dev = statbuf.st_dev;
    *ino = statbuf.st_ino;

    return 0;
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

                options.memory  = NULL;

                break;

            case OPTION_F:
                options.formfeed = "1,0";

                break;

            case OPTION_f:
                options.formfeed = "0,1";

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
                if (optarg != NULL)
                {
                    if (optarg[0] == '-')
                    {
                        printf("Invalid file name for %s option\n",
                               argv[optind - 2]);

                        exit(EXIT_FAILURE);
                    }

                    options.initial = optarg;
                }
                else
                {
                    options.initial = NULL;
                }

                break;

            case OPTION_i:
                options.initial = NULL;

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
                teco_init   = NULL;
                teco_memory = NULL;
                teco_vtedit = NULL;

                options.create  = false;
                options.initial = NULL;
                options.memory  = NULL;
                options.vtedit  = NULL;

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

            case OPTION_V:
                if (optarg != NULL)
                {
                    if (optarg[0] == '-')
                    {
                        printf("Invalid file name for %s option\n",
                               argv[optind - 2]);

                        exit(EXIT_FAILURE);
                    }

                    options.vtedit = optarg;
                }
                else
                {
                    options.vtedit = NULL;
                }

                break;

            case OPTION_v:
                options.vtedit = NULL;

                break;

            case OPTION_X:
                options.exit = true;

                break;

            case ':':
                printf("Argument required for %s option\n", argv[optind - 1]);

                exit(EXIT_FAILURE);

            // Hidden options start here

            case SPACE:                 // --version option
                fputs("teco (TECO text editor) version ", stdout);

#if     defined(DEBUG)

                fputc('X', stdout);     // Say that it's a debug version

#endif

                printf("%d.%d.%d\n", major_version, minor_version, patch_version);

                printf("Copyright (C) 2019-2023 Nowwith Treble Software\n");

                exit(EXIT_SUCCESS);

            case DEL:                   // --mung option
                mung = true;

                break;

#if     defined(DEBUG)

            // Debug options start here

            case '\001':
                options.e1 = (optarg != NULL) ? optarg : "-1";

                break;

            case '\002':
                options.e2 = (optarg != NULL) ? optarg : "-1";

                break;

            case '\003':
                options.e3 = (optarg != NULL) ? optarg : "-1";

                break;

            case '\004':
                options.e4 = (optarg != NULL) ? optarg : "-1";

                break;

            case '\005':
                if (optarg != NULL && optarg[0] != '-')
                {
                    key_name = optarg;
                }
                else
                {
                    key_name = NULL;
                }

                break;

            case '\006':
                options.print   = true; // Print initial command string

                break;

            case '\007':
                options.quit = true;    // Quit after printing command string

                break;

#endif

            default:
                printf("Unknown option '%s': use --help for list of "
                       "options\n", argv[optind - 1]);

                exit(EXIT_FAILURE);
        }
    }

    // See if we have any file arguments

    if (optind < argc)
    {
        if (mung)
        {
            options.memory  = NULL;
            options.execute = argv[optind++];
        }
        else
        {
            file1 = argv[optind++];

            // Check for an output file name

            if (optind < argc)
            {
                file2 = argv[optind++];
            }
        }

        if (optind < argc)
        {
            tprint("?Too many file arguments\n");

            exit(EXIT_FAILURE);
        }
    }

    // Disable display mode if stdin has been redirected

    if (f.e0.i_redir)
    {
        options.display = false;
        options.scroll = NULL;
        options.vtedit = NULL;
    }
}


///
///  @brief    If --print option was specified, print initial command string
///            that we created from the command-line options. If the -X option
///            was also specified, then we will also exit.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DEBUG)

static void print_initial(void)
{
    if (!options.print)                 // Should we print the command string?
    {
        return;
    }

    tprint("Commands: ");

    for (uint i = 0; i < cbuf->len; ++i)
    {
        int c = cbuf->data[i];

        switch (c)
        {
            case BS:
                fputs("\\b", stdout);
                break;

            case HT:
                fputs("\\t", stdout);
                break;

            case LF:
                fputs("\\n", stdout);
                break;

            case VT:
                fputs("\\v", stdout);
                break;

            case FF:
                fputs("\\f", stdout);
                break;

            case CR:
                fputs("\\r", stdout);
                break;

            case ESC:
                fputs("\\e", stdout);
                break;

            default:
                if (iscntrl(c))
                {
                    printf("\\%d", c);
                }
                else
                {
                    fputc(c, stdout);
                }
                break;
        }
    }

    tprint("\n");
    fflush(stdout);

    if (options.quit)
    {
        exit(EXIT_SUCCESS);
    }
}

#endif


///
///  @brief    Store a formatted command string in the command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void store_cmd(const char *format, ...)
{
    assert(format != NULL);

    char cmd[PATH_MAX];
    va_list args;

    va_start(args, format);

    vsnprintf(cmd, sizeof(cmd), format, args);

    va_end(args);

    int c;
    const char *p = cmd;

    while ((c = *p++) != NUL)
    {
        store_cbuf(c);
    }

    store_cbuf(' ');
}
