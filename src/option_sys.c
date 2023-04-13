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
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
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

#define MAX_CMDS        100             ///< Maximum no. of command-line arguments

static char **cmd_list = NULL;          ///< List of command-line arguments
static int next_cmd;                    ///< No. of command-line arguments found

///
///   @struct  options
///
///   @brief   Structure for holding information on configuration options.
///

struct options
{
    const char *file1;      ///< Input file
    const char *file2;      ///< Output file
    const char *args;       ///< -A or --arguments
    bool create;            ///< -C or --create
    bool display;           ///< -D or --display
    bool readonly;          ///< -R or --readonly
    bool exit;              ///< -X or --exit
    bool mung;              ///< --mung (hidden)
    bool print;             ///< --print (hidden)
    bool quit;              ///< --quit (hidden)
};

///
///   @var     options
///
///   @brief   Current configuration options.
///

static struct options options =
{
    .file1    = NULL,
    .file2    = NULL,
    .args     = NULL,
    .create   = true,
    .display  = false,
    .readonly = false,
    .exit     = false,
    .mung     = false,
    .print    = false,
    .quit     = false,
};

// Local functions

static void check_arg(const char *str, const char *option);

static void check_file(const char *option);

static void exec_options(void);

static int get_info(const char *file, dev_t *dev, ino_t *ino);

static void open_files(const char *infile, const char *outfile);

static void parse_args(const char *args);

static void parse_A(void);

static void parse_D(void);

#if     defined(DEBUG)

static void parse_eflag(const char *cmd);

#endif

static void parse_E(void);

static void parse_H(void);

static void parse_I(void);

#if     defined(DEBUG)

static void parse_keys(void);

#endif

static void parse_L(void);

static void parse_mung(const char *file);

static void parse_M(void);

static void parse_n(void);

static void parse_S(void);

static void parse_T(void);

static void parse_V(void);

static void parse_version(void);

static void print_initial(void);

static void read_options(int argc, const char *const argv[]);

static void save_next(const char *format, ...);

static void store_cmd(const char *format, ...);

static void store_EB(const char *file);

static void store_ER(const char *file);

static void store_EW(const char *file);


///
///  @brief    Verify that argument for command-line option is valid.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void check_arg(const char *str, const char *option)
{
    if (optarg == NULL)
    {
        printf("Missing %s for %s option\n", str, option);

        exit(EXIT_FAILURE);
    }        
}


///
///  @brief    Verify that file argument for command-line option is valid.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void check_file(const char *option)
{
    check_arg("file name", option);

    if (optarg[0] == '-')
    {
        printf("Invalid file name for %s option\n", option);

        exit(EXIT_FAILURE);
    }
}


///
///  @brief    Store all command-line options in command buffer for execution
///            when TECO starts.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_options(void)
{
    if (teco_init != NULL)              // Initializaton file is always first
    {
        store_cmd("EI%s\e", teco_init);
    }

    if (teco_vtedit != NULL)            // Initialize display next
    {
        store_cmd("EI%s\e", teco_vtedit);
    }

    // Now process all options we saw

    for (int i = 0; i < next_cmd; ++i)
    {
        store_cmd(cmd_list[i]);
    }

    // Now process input file (if any) and output file (if any)

    if (options.file1 == NULL)          // We have no file names
    {
        open_files(NULL, NULL);
    }
    else if (options.file2 == NULL)     // We only have one file name
    {
        open_files(options.file1, NULL);
    }
    else                                // We have two file names
    {
        dev_t dev1, dev2;
        ino_t ino1, ino2;
        int err1 = get_info(options.file1, &dev1, &ino1);
        int err2 = get_info(options.file2, &dev2, &ino2);

        // See if the two files are actually the same. There are basically two
        // possibilities: either both files exist and have the same device IDs
        // and inode numbers, or neither exist, but their file names match. In
        // either case, we skip using the second file name, and just use an EB
        // command to open the file.

        if (err1 == 0 && err2 == 0)
        {
            if (dev1 == dev2 && ino1 == ino2)
            {
                options.file2 = NULL;
            }
        }
        else if (err1 == ENOENT && err2 == ENOENT)
        {
            if (!strcmp(options.file1, options.file2))
            {
                options.file2 = NULL;
            }
        }

        open_files(options.file1, options.file2);
    }

    if (options.exit)                   // Should we exit at end of commands?
    {
        store_cmd("EX");
    }

    if (cbuf->len != 0)                 // Anything stored?
    {
        store_cmd("\e\e");
    }
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
///  @brief    Process command-line options.
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

    // Disable display mode if stdin has been redirected

    if (f.e0.i_redir)
    {
        options.display = false;

        teco_vtedit = NULL;
    }

    // Allocate list of command-line options we've processed

    cmd_list = alloc_mem((uint)sizeof(const char *) * MAX_CMDS);
    next_cmd = 0;

    for (int i = 0; i < MAX_CMDS; ++i)
    {
        cmd_list[i] = NULL;
    }

    read_options(argc, argv);

    // See if we have any file arguments

    if (optind < argc)
    {
        if (options.mung)
        {
            parse_mung(argv[optind++]);
        }
        else
        {
            options.file1 = argv[optind++];

            // Check for an output file name

            if (!options.readonly && optind < argc)
            {
                options.file2 = argv[optind++];
            }
        }

        if (optind < argc)
        {
            printf("?Too many file arguments\n");

            exit(EXIT_FAILURE);
        }
    }

    exec_options();

    // Free up memory for command-line options

    for (int i = 0; i < MAX_CMDS; ++i)
    {
        if (cmd_list[i] != NULL)
        {
            free_mem(&cmd_list[i]);
        }
    }

    free_mem(&cmd_list);

    print_initial();                    // Maybe print initial command string
}


///
///  @brief    Open input and/or output files. If we have neither, we'll check
///            TECO's memory file; if we only have one, we'll open it for
///            backup; if we have both, we'll open the first for reading and
///            the second for writing. Note that in that last case, our caller
///            will have first verified that we have two different files.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void open_files(const char *infile, const char *outfile)
{
    // Here to figure out which file commands to use for an input file.

    if (infile == NULL)
    {
        char memory[PATH_MAX] = { '\0'}; // File name from memory file
        
        if (teco_memory != NULL)        // Try to read memory file
        {
            read_memory(memory, (uint)sizeof(memory));

            if (memory[0] != NUL)
            {
                store_EB(memory);

                return;
            }
        }

        // Here if we have no file to open, so just start TECO, unless the
        // --read-only option was used, in which case we print an error.

        if (options.readonly)
        {
            store_cmd(":^A?How can I inspect nothing?\1");

            options.exit = true;
        }

        return;
    }

    // Here if we have an input file

    if (outfile == NULL)                // Any output file?
    {
        store_EB(infile);               // No, try to open for backup
    }
    else                                // Open separate input and output files
    {
        store_ER(infile);
        store_EW(outfile);

        teco_memory = NULL;             // Don't save file name on exit
    }
}


///
///  @brief    Parse m,n arguments for flag option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_args(const char *option)
{
    // If argument exists, confirm that it's of the "ddd" or "ddd,ddd" where
    // "ddd" is a decimal integer.

    const char *p = optarg;
    int nbytes;
    int c;

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
        printf("Invalid value '%s' for %s option\n", optarg, option);

        exit(EXIT_FAILURE);
    }
}


///
///  @brief    Parse -A or --arguments option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_A(void)
{
    const char *option = "-A or --arguments";

    check_arg("value", option);
    parse_args(option);
    options.args = optarg;
}


///
///  @brief    Parse -D or --display option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_D(void)
{
    // No display mode if input is redirected or if we're exiting immediately

    if (!f.e0.i_redir || options.exit)
    {
        save_next("-1W");

        options.display = true;
    }
}


///
///  @brief    Parse --E1, --E2, --E3, and --E4 options.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DEBUG)

static void parse_eflag(const char *option)
{
    const char *args = optarg;

    if (args == NULL)
    {
        args = "-1";
    }

    parse_args(option);
    save_next("%s%s\e", args, option);
}

#endif


///
///  @brief    Parse -E or --execute option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_E(void)
{
    check_file("-E or --execute");
    parse_mung(optarg);
}


///
///  @brief    Parse -H or --help option.
///
///  @returns  Exits from TECO.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_H(void)
{
    const char *p;
    uint i = 0;

    while ((p = help_text[i++]) != NULL)
    {
        printf("%s\n", p);
    }

    exit(EXIT_SUCCESS);
}


///
///  @brief    Parse -I or --initialize option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_I(void)
{
    check_file("-I or --initialize");

    teco_init = optarg;
}


///
///  @brief    Parse --keys option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DEBUG)

static void parse_keys(void)
{
    if (optarg != NULL && optarg[0] != '-')
    {
        key_name = optarg;
    }
    else
    {
        key_name = NULL;
    }
}

#endif


///
///  @brief    Parse -L or --log option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_L(void)
{
    check_file("-L or --log option");
    save_next("EL%s\e", optarg);
}


///
///  @brief    Parse --mung option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_mung(const char *file)
{
    assert(file != NULL);

    // If file name starts and ends with single or double quotes, then it's
    // really a TECO command string.

    size_t nbytes = strlen(file);
    const char *args = options.args;

    options.args = NULL;
    teco_memory  = NULL;
    
    if (nbytes > 2)
    {
        int first = file[0];
        int last = file[nbytes - 1];

        if (first == last && (first == '"' || first == '\''))
        {
            nbytes -= 2;
            save_next("%.*s\e", (int)nbytes, file + 1);

            return;
        }
    }

    if (args == NULL)
    {
        args = "";
    }

    save_next("%sEI%s\e", args, file);
}


///
///  @brief    Parse -M or --memory option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_M(void)
{
    check_file("-M or --memory option");

    teco_memory = optarg;
}


///
///  @brief    Parse -n option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_n(void)
{
    teco_init   = NULL;
    teco_memory = NULL;
    teco_vtedit = NULL;

    options.create  = false;
}


///
///  @brief    Parse -S or --scroll option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_S(void)
{
    // If argument exists, confirm that it's of the "ddd", where
    // "ddd" is a decimal integer.

    check_arg("argument", "-S or --scroll option");

    const char *p = optarg;
    int count;
    int c;

    if (sscanf(p, "%d%n", &c, &count) == 1)
    {
        p += count;
    }

    if (*p != NUL)
    {
        printf("Invalid value '%s' for -S or --scroll option\n", optarg);

        exit(EXIT_FAILURE);
    }

    // No display mode if input is redirected or if we're exiting immediately

    if (!f.e0.i_redir || options.exit)
    {
        if (!options.display)
        {
            parse_D();
        }

        save_next("%s,7:W\e", optarg);
    }
}


///
///  @brief    Parse -T or --text option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_T(void)
{
    check_arg("argument", "-T or --text option");
    save_next("I%s\e", optarg);
}


///
///  @brief    Parse -V or --vtedit option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_V(void)
{
    check_file("-V or --vtedit option");

    // No display mode if input is redirected or if we're exiting immediately

    if (!f.e0.i_redir || options.exit)
    {
        teco_vtedit = optarg;
    }
}


///
///  @brief    Parse --version option.
///
///  @returns  Exits from TECO.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_version(void)
{
    fputs("teco (TECO text editor) version ", stdout);

#if     defined(DEBUG)

    fputc('X', stdout);     // Say that it's a debug version

#endif

    printf("%d.%d.%d\n", major_version, minor_version, patch_version);

    printf("Copyright (C) 2019-2023 Nowwith Treble Software\n");

    exit(EXIT_SUCCESS);
}


///
///  @brief    If --print option was specified, print initial command string
///            that we created from the command-line options. If the -X option
///            was also specified, then we will also exit.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

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

    (void)fflush(stdout);

    if (options.quit)
    {
        exit(EXIT_SUCCESS);
    }
}


///
///  @brief    Read command-line options.
///
///            Note that we use printf() instead of tprint() here because we
///            are called before terminal characteristics have been changed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_options(
    int argc,                           ///< No. of arguments
    const char *const argv[])           ///< List of arguments
{
    assert(argv != NULL);               // Error if no argument list
    assert(argv[0] != NULL);            // Error if no strings in list

    // These two assertions confirm the standard behavior of getopt_long()
    // regarding the ordering of option and non-option arguments.

    assert(optstring[0] != '+' && optstring[0] != '-'); // Verify assumptions
    assert(getenv("POSIXLY_CORRECT") == NULL); // Verify assumptions

    optind = 0;                         // Reset from any previous calls
    opterr = 0;                         // Suppress any error messages

    int c;
    int idx = 0;

    while ((c = getopt_long(argc, (char * const *)argv,
                            optstring, long_options, &idx)) != -1)
    {
        switch (c)
        {
            case OPTION_A: parse_A();                   break;
            case OPTION_C: options.create = true;       break;
            case OPTION_c: options.create = false;      break;
            case OPTION_D: parse_D();                   break;
            case OPTION_E: parse_E();                   break;
            case OPTION_F: save_next("1,0E3");          break;
            case OPTION_f: save_next("0,1E3");          break;
            case OPTION_H: parse_H();                   break;
            case OPTION_I: parse_I();                   break;
            case OPTION_i: teco_init = NULL;            break;
            case OPTION_L: parse_L();                   break;
            case OPTION_M: parse_M();                   break;
            case OPTION_m: teco_memory = NULL;          break;
            case OPTION_n: parse_n();                   break;
            case OPTION_R: options.readonly = true;     break;
            case OPTION_r: options.readonly = false;    break;
            case OPTION_S: parse_S();                   break;
            case OPTION_T: parse_T();                   break;
            case OPTION_V: parse_V();                   break;
            case OPTION_X: options.exit = true;         //lint -fallthrough
            case OPTION_v: teco_vtedit = NULL;          break;

            // Hidden options start here

            case 1: parse_version();                    break;
            case 2: options.mung = true;                break;
            case 3: options.quit  = true;               //lint -fallthrough
            case 4: options.print = true;               break;

#if     defined(DEBUG)

            // Debug options start here

            case 200: parse_keys();                     break;
            case 201: parse_eflag("E1");                break;
            case 202: parse_eflag("E2");                break;
            case 203: parse_eflag("E3");                break;
            case 204: parse_eflag("E4");                break;

#endif

            case ':':
                printf("Argument required for %s option\n", argv[optind - 1]);

                exit(EXIT_FAILURE);

            default:
                printf("Unknown option '%s': use --help for list of "
                       "options\n", argv[optind - 1]);

                exit(EXIT_FAILURE);
        }
    }
}


///
///  @brief    Save next command in command list.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void save_next(const char *format, ...)
{
    if (next_cmd == MAX_CMDS)
    {
        printf("Too many command-line options\n");

        exit(EXIT_FAILURE);
    }

    char cmd[PATH_MAX];
    va_list args;

    va_start(args, format);

    vsnprintf(cmd, sizeof(cmd), format, args);

    va_end(args);

    size_t nbytes = strlen(cmd);

    cmd_list[next_cmd] = alloc_mem((uint)nbytes + 1);
    sprintf(cmd_list[next_cmd], "%s", cmd);

    ++next_cmd;
}


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


///
///  @brief    Store EB command. Note that if the file open fails, the rest of
//             the command string will be aborted, which means that the CTRL/A
//             command won't be executed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void store_EB(const char *file)
{
    if (options.readonly)               // Should we open for read only?
    {
        store_ER(file);
    }
    else                                // No -- open for backup
    {
        store_cmd("EB%s\e Y :^AEditing file: %s\1", file, file);
    }
}


///
///  @brief    Store ER command. Note that if the file open fails, the rest of
//             the command string will be aborted, which means that the CTRL/A
//             command won't be executed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void store_ER(const char *file)
{
    store_cmd("ER%s\e Y :^AReading file: %s\1", file, file);
}


///
///  @brief    Store EW command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void store_EW(const char *file)
{
    store_cmd(":^AWriting file: %s\1 EW%s\e", file, file);
}
