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
#include <limits.h>                 //lint !e451
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>               // for stat()

#include "teco.h"
#include "ascii.h"
#include "cbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "file.h"
#include "term.h"
#include "version.h"

#include "_option_sys.c"            // Include text for --help option


///
///   @struct  options
///
///   @brief   Structure for holding information on configuration options.
///

struct options
{
    const char *file1;      ///< Input file, or file to mung
    const char *file2;      ///< Output file
    const char *args;       ///< --arguments option
    int scroll;             ///< --scroll option
    bool create;            ///< --create option
    bool display;           ///< --display and --nodisplay options
    bool init;              ///< --initialize and --noinitialize option s
    bool memory;            ///< --nomemory option 
    bool readonly;          ///< --read-only option
    bool exit;              ///< --exit option
    bool mung;              ///< --mung option (hidden)
    bool practice;          ///< --practice option (hidden)
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
    .scroll   = 0,
    .create   = true,
    .display  = true,
    .init     = true,
    .memory   = true,
    .readonly = false,
    .exit     = false,
    .mung     = false,
    .practice = false,
};


// Local functions

static void check_arg(const char *str, const char *option);

static void check_file(const char *option);

static int get_info(const char *file, dev_t *dev, ino_t *ino);

static void open_files(const char *infile, const char *outfile);

static void opt_arguments(void);

static void opt_display(void);

static void opt_execute(void);

static void opt_help(void);

static void opt_initialize(void);

static void opt_keys(void);

static void opt_log(void);

static void opt_mung(const char *file);

static void opt_nodefaults(void);

static void opt_nodisplay(void);

static void opt_practice(void);

static void opt_scroll(void);

static void opt_text(void);

static void opt_version(void);

static void parse_args(const char *args);

static void parse_options(int argc, const char *const argv[]);

static void print_cmd(const char *cmd);

static void store_cmd(const char *format, ...);

static void store_EB(const char *file);

static void store_ER(const char *file);

static void store_EW(const char *file);

static uint verify_size(int nbytes);


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
///  @brief    Check obsolete options -V, -v, --vtedit, and --novtedit.
///
///            THIS IS A TEMPORARY FUNCTION, AND WILL BE REMOVED IN A FUTURE RELEASE.
///
///  @returns  Exits if obsolete option found
///
////////////////////////////////////////////////////////////////////////////////

static void check_vtedit(const char *const argv[], int lastopt);

static void check_vtedit(const char *const argv[], int lastopt)
{
    assert(argv != NULL);

    const char *option = argv[optind - 1];
    const char *error = "-V";           // Assume a default error
    const char *alt = "-D or --display"; // And a default alternate command

    if (optopt != 0 && optind == lastopt)
    {
        if (optopt == 'v')
        {
            error = "-v";
            alt = "-d or --nodisplay";
        }
        else if (optopt != 'V')
        {
            return;
        }
    }
    else if (strcmp(option, "-v") == 0)
    {
        error = "-v";
        alt = "-d or --nodisplay";
    }
    else if (strstr(option, "--nov") == option)
    {
        error = "--novtedit";
        alt = "-d or --nodisplay";
    }
    else if (strstr(option, "--vt") == option)
    {
        error = "--vtedit";
    }
    else if (strcmp(option, "-V") != 0)
    {
        return;
    }

    // Here if we know we have an obsolete error

    printf("Obsolete option '%s' -- use %s instead\n", error, alt);

    exit(EXIT_FAILURE);
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

    options.display = (teco_vtedit != NULL);
    options.init    = (teco_init   != NULL);
    options.memory  = (teco_memory != NULL);

    parse_options(argc, argv);

    // If we have an initialization file, then it will be processed by TECO
    // first. This means we have to shift over any other commands in the buffer
    // to make room for this command.

    if (options.init)                   // Initialization file is always first
    {
        assert(teco_init != NULL);

        char initbuf[KB];
        int retval = snprintf(initbuf, sizeof(initbuf), "EI%s\e ", teco_init);
        uint size = verify_size(retval);

        memmove(cbuf->data + size, cbuf->data, (size_t)cbuf->len);
        memcpy(cbuf->data, initbuf, (size_t)size);

        cbuf->len += size;
        cbuf->data[cbuf->len] = NUL;
    }

    // Process any input file and any output file

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

    // Add display options on the end, unless we're exiting immediately

    if (options.exit)                   // Should we exit at end of commands?
    {
        store_cmd("EX ");
    }
    else if (!f.e0.i_redir)             // No display if input redirected
    {
        if (options.display)            // Do we have a display initialization file?
        {
            if (teco_vtedit == NULL)
            {
                store_cmd("-1W ");
            }
            else
            {
                store_cmd("EI%s\e ", teco_vtedit);
            }
        }

        if (options.scroll != 0)        // Add scrolling if requested
        {
            store_cmd("%u,7:W\e ", options.scroll);
        }
    }

    if (cbuf->len != 0)                 // Have we stored any commands?
    {
        store_cmd("\e\e");              // Terminate command w/ double ESCape
    }

    print_cmd(cbuf->data);

    if (options.practice)
    {
        exit(EXIT_SUCCESS);
    }
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

        if (options.memory)             // Try to read memory file
        {
            assert(teco_memory != NULL);

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
            store_cmd(":^A?How can I inspect nothing?\1 ");

            options.exit = true;
        }

        return;
    }

    // Here if we have an input file

    if (outfile == NULL)                // If no output file, use EW or EB
    {
        if (access(infile, F_OK) != 0 && options.create)
        {
            store_EW(infile);            // EW if no file and okay to create one
        }
        else
        {
            store_EB(infile);            // Else try EB
        }
    }
    else                                // Open separate input and output files
    {
        store_ER(infile);
        store_EW(outfile);

        teco_memory = NULL;             // Don't save file name on exit
    }
}


///
///  @brief    Parse -A or --arguments option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_arguments(void)
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

static void opt_display(void)
{
    if (optarg != NULL)
    {
        teco_vtedit = optarg;
    }

    options.display = true;
}


///
///  @brief    Parse -E or --execute option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_execute(void)
{
    check_file("-E or --execute");
    opt_mung(optarg);
}


///
///  @brief    Parse -H or --help option.
///
///  @returns  Exits from TECO.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_help(void)
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

static void opt_initialize(void)
{
    check_file("-I or --initialize");

    teco_init = optarg;
    options.init = true;
}


///
///  @brief    Parse --keys option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_keys(void)
{
#if     defined(DEBUG)
    if (optarg != NULL && optarg[0] != '-')
    {
        key_name = optarg;
    }
    else
    {
        key_name = NULL;
    }
#else
    printf("--keys option is only available in debug builds\n");

    exit(EXIT_SUCCESS);
#endif
}


///
///  @brief    Parse -L or --log option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_log(void)
{
    check_file("-L or --log");
    store_cmd("EL%s\e ", optarg);
}


///
///  @brief    Parse --mung option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_mung(const char *file)
{
    assert(file != NULL);

    // If file name starts and ends with single or double quotes, then it's
    // really a TECO command string.

    size_t nbytes = strlen(file);
    const char *args = options.args;

    options.args = NULL;
    options.memory = false;             // Don't read memory file if munging

    if (nbytes > 2)
    {
        int first = file[0];
        int last = file[nbytes - 1];

        if (first == last && (first == '"' || first == '\''))
        {
            nbytes -= 2;
            store_cmd("%.*s\e ", (int)nbytes, file + 1);

            return;
        }
    }

    if (args == NULL)
    {
        args = "";
    }

    store_cmd("%sEI%s\e ", args, file);
}


///
///  @brief    Parse -n option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_nodefaults(void)
{
    options.display = false;
    options.init    = false;
    options.memory  = false;
}


///
///  @brief    Parse -d or --nodisplay option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_nodisplay(void)
{
    teco_vtedit = NULL;
    options.display = false;
}


///
///  @brief    Parse --practice option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_practice(void)
{
    options.practice = true;
}


///
///  @brief    Parse -S or --scroll option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_scroll(void)
{
    // If argument exists, confirm that it's of the "ddd", where
    // "ddd" is a decimal integer.

    check_arg("argument", "-S or --scroll");

    const char *p = optarg;
    int count;
    int c;

    if (sscanf(p, "%d%n", &c, &count) == 1)
    {
        p += count;
    }

    if (*p != NUL || c <= 0)
    {
        printf("Invalid value '%s' for -S or --scroll option\n", optarg);

        exit(EXIT_FAILURE);
    }

    options.scroll = c;
    options.display = true;
}


///
///  @brief    Parse -T or --text option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_text(void)
{
    check_arg("argument", "-T or --text");
    store_cmd("I%s\e ", optarg);
}


///
///  @brief    Parse --version option.
///
///  @returns  Exits from TECO.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_version(void)
{
    printf("teco (TECO text editor) version %d.%d.%d", major_version,
           minor_version, patch_version);

#if     defined(DEBUG)

    fputs(" (DEBUG)", stdout);          // Say that it's a debug version

#endif

    fputc('\n', stdout);

    printf("Copyright (C) 2019-2023 Nowwith Treble Software\n");

    exit(EXIT_SUCCESS);
}


///
///  @brief    Parse m,n arguments for -A, --E1, --E2, --E3, and --E4 options.
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
///  @brief    Parse command-line options.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_options(
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
    int lastopt = 1;

    while ((c = getopt_long(argc, (char * const *)argv,
                            optstring, long_options, NULL)) != -1)
    {
        switch (c)
        {
            // Options that just set values

            case OPT_create:       options.create = true;     break;
            case OPT_nocreate:     options.create = false;    break;
            case OPT_noinitialize: options.init = false;      break;
            case OPT_nomemory:     options.memory = false;    break;
            case OPT_read_only:    options.readonly = true;   break;
            case OPT_noread_only:  options.readonly = false;  break;
            case OPT_exit:         options.exit = true;       break;
            case OPT_mung:         options.mung = true;       break; // Hidden

            // Options that call functions

            case OPT_arguments:    opt_arguments();           break;
            case OPT_display:      opt_display();             break;
            case OPT_execute:      opt_execute();             break;
            case OPT_formfeed:     store_cmd("1,0E3 ");       break;
            case OPT_noformfeed:   store_cmd("0,1E3 ");       break;
            case OPT_help:         opt_help();                break;
            case OPT_initialize:   opt_initialize();          break;
            case OPT_keys:         opt_keys();                break;
            case OPT_log:          opt_log();                 break;
            case OPT_nodefaults:   opt_nodefaults();          break;
            case OPT_nodisplay:    opt_nodisplay();           break;
            case OPT_practice:     opt_practice();            break;
            case OPT_scroll:       opt_scroll();              break;
            case OPT_text:         opt_text();                break;
            case OPT_version:      opt_version();             break; // Hidden

            case ':':
                printf("Argument required for %s option\n", argv[optind - 1]);

                exit(EXIT_FAILURE);

            case '?':
                check_vtedit(argv, lastopt); // TODO: REMOVE IN FUTURE RELEASE

                //lint -fallthrough

            default:
                printf("Unknown option ");

                //  The following fixes the problem that arises if an invalid
                //  short option is combined with another short option (either
                //  valid or invalid), which causes an error to be returned
                //  without updating optind. This means that argv[optind - 1]
                //  will point to the previous argument instead of the current
                //  one.
                //
                //  Consider the following cases with invalid short options:
                //
                //      Command          Result
                //      -----------      ---------------------
                //      teco -n -x       Unknown option '-x'
                //      teco -n -xc      Unknown option '-n'
                //      teco -xc         Unknown option 'teco'
                //
                //  To ensure correct output, if optopt is non-zero, meaning
                //  that it contains the ASCII for a invalid short option, and
                //  if optind has not been incremented since the last option,
                //  then that short option was combined with another option,
                //  and we have to use optopt to print the error. Otherwise we
                //  either have an invalid long option, or an invalid short
                //  option that was not combined with another option, and we
                //  should use optind to print the error.
                //
                //      Command          Action
                //      -----------      ---------------------
                //      teco -xc         optopt contains bad option.
                //      teco -x          argv[optind-1] points to option.
                //      teco --x         argv[optind-1] points to option.

                if (optopt != 0 && optind == lastopt)
                {
                    printf("'-%c'", optopt);
                }
                else
                {
                    printf("\"%s\"", argv[optind - 1]);
                }

                printf(": use --help for list of options\n");

                exit(EXIT_FAILURE);
        }

        lastopt = optind;
    }

    // Check for file arguments

    if (optind < argc)
    {
        if (options.mung)
        {
            opt_mung(argv[optind++]);
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
}


///
///  @brief    Print command in queue if --print option seen.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void print_cmd(const char *cmd)
{
    assert(cmd != NULL);

    if (!options.practice)
    {
        return;
    }

    int c;

    printf("Commands: ");

    while ((c = *cmd++) != NUL)
    {
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

    fputc('\n', stdout);
}


///
///  @brief    Save next command in command list.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void store_cmd(const char *format, ...)
{
    va_list args;

    va_start(args, format);

    size_t size = cbuf->size - cbuf->len;
    int retval = vsnprintf(cbuf->data + cbuf->len, size, format, args);

    va_end(args);

    cbuf->len += verify_size(retval);

    cbuf->data[cbuf->len] = NUL;
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
        store_cmd("EB%s\e Y :^AEditing file: %s\1 ", file, file);
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
    store_cmd("ER%s\e Y :^AReading file: %s\1 ", file, file);
}


///
///  @brief    Store EW command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void store_EW(const char *file)
{
    store_cmd(":^AWriting file: %s\1 EW%s\e ", file, file);
}


///
///  @brief    Verify that there's room in the option buffer for the next
///            command.
///
///  @returns  No. of bytes to store.
///
////////////////////////////////////////////////////////////////////////////////

static uint verify_size(int retval)
{
    assert(retval > 0);

    uint size = (uint)retval;

    if (cbuf->len + size >= cbuf->size)
    {
        printf("No memory for command-line options\n");

        exit(EXIT_FAILURE);
    }

    return size;
}
