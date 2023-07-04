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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>               // for stat()

#if     defined(__DECC)

    #define noreturn

#else

    #include <stdnoreturn.h>

#endif

#include "teco.h"
#include "ascii.h"
#include "cmdbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "file.h"
#include "term.h"
#include "version.h"

#include "_option_sys.c"            // Include generated option data


#define NOPTIONS        25          ///< Total no. of string options

///
///   @struct  options
///
///   @brief   Structure for holding information on configuration options.
///

struct options
{
    char stack[NOPTIONS];           ///< Option stack 
    const char *args[NOPTIONS];     ///< Stack arguments
    uint next;                      ///< Next option on stack
    const char *mn_args;            ///< Current numeric arguments (from -A)
    int scroll;                     ///< --scroll option
    bool create;                    ///< --create option
    bool display;                   ///< --display option
    bool readonly;                  ///< --read-only option
    bool exit;                      ///< --exit option
    bool execute;                   ///< --execute option
    bool make;                      ///< --make option
    bool mung;                      ///< --mung option
    bool practice;                  ///< --practice option (hidden)
};

///
///   @var     options
///
///   @brief   Current configuration options.
///

static struct options options =
{
    .stack    = { NUL },
    .args     = { NULL },
    .next     = 0,
    .mn_args  = NULL,
    .scroll   = 0,
    .create   = true,
    .display  = true,
    .readonly = false,
    .exit     = false,
    .execute  = false,
    .make     = false,
    .mung     = false,
    .practice = false,
};

///   @var      begin_tag
///
///   @brief    String for marking beginning of practice run.

static const char *begin_tag = "!begin!";

///   @var      create_file
///
///   @brief    String for executing EW command.

static const char *create_file = "%d\"L :^ACreating new file '%s'\1 ' EW%s\e";

///   @var      end_tag
///
///   @brief    String for marking end of practice run.

static const char *end_tag = "!end!";

///   @var      option_limit
///
///   @brief    Error when out of memory for processing command-line options.

static const char *option_limit = "Too many options";


// Local functions

static void opt_arguments(bool optlong, const char *const argv[]);

static void opt_help(void);

static void opt_scroll(bool optlong, const char *const argv[]);

static noreturn void opt_unknown(const char *const argv[]);

static void opt_version(void);

static void parse_files(int argc, const char *const argv[]);

static void parse_options(int argc, const char *const argv[]);

static bool pop_opts(void);

static void push_opt(int option, const char *arg);

static noreturn void quit(const char *format, ...);

static int stat_info(const char *file, dev_t *dev, ino_t *ino);

static void store_cmd(const char *format, ...);


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

    parse_options(argc, argv);

    if (teco_init != NULL)              // Initialization file is always first
    {
        store_cmd("EI%s\e", teco_init);
    }

    if (pop_opts())                     // Pop any options saved on stack
    {
        parse_files(argc, argv);        // Then parse files to edit
    }

    if (optind < argc)
    {
        quit("Too many files");
    }

    // Add display options on the end, unless we're exiting immediately

    if (options.exit)                   // Should we exit at end of commands?
    {
        store_cmd("EX");
    }
    else if (!f.e0.i_redir)             // No display if input redirected
    {
        if (options.display)            // Should we enable display?
        {
            if (teco_vtedit == NULL)    // Yes. Use initialization file?
            {
                store_cmd("-1W");       // No, just start it
            }
            else                        // File takes precedence over -1W
            {
                store_cmd("EI%s\e", teco_vtedit);
            }
        }

        if (options.scroll != 0)        // Add scrolling if requested
        {
            store_cmd("%u,7:W\e", options.scroll);
        }
    }

    if (cbuf->len != 0)                 // Have we stored any commands?
    {
        store_cmd("\e\e");              // Terminate command w/ double ESCape
    }

    if (options.practice)
    {
        fprintf(stderr, "%s ", begin_tag);

        for (uint i = 0; i < cbuf->len; ++i)
        {
            int c = cbuf->data[i];

            if (c == DEL || !isascii(c))
            {
                fprintf(stderr, "[%02x]", c);
            }
            else if (iscntrl(c))
            {
                fprintf(stderr, "^%c", c + 'A' - 1);
            }
            else
            {
                fputc(c, stderr);
            }
        }

        fputs(end_tag, stderr);

        exit(EXIT_SUCCESS);
    }
}


///
///  @brief    Parse -A and --arguments options. These are only used to provide
///            numeric arguments for a subsequent EI command specified by a -E
///            or --execute option.
///
///            Examples:
///
//                -A 123 --execute=foo      => 123@EI/foo/
//                --arguments=456,789 -Ebaz => 456,789@EI/baz/
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_arguments(bool optlong, const char *const argv[])
{
    assert(argv != NULL);
    assert(optarg != NULL);

    const char *p = optarg;
    int nbytes;
    int n;

    if (sscanf(p, "%d%n", &n, &nbytes) == 1)
    {
        p += nbytes;

        if (*p == ',')
        {
            ++p;

            if (sscanf(p, "%d%n", &n, &nbytes) == 1)
            {
                p += nbytes;
            }
        }

        if (*p == NUL)
        {
            push_opt(OPT_arguments, optarg);

            return;
        }
    }

    quit("Invalid argument '%s' for %s option", optarg,
         optlong ? "--arguments" : "-A");
}


///
///  @brief    Parse -H and --help options.
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
///  @brief    Parse -S and --scroll options. These are used to specify the
///            size of the command window in display mode.
///
///            Examples:
///
///               -S 10      => 10,7:W
///               --scroll=5 => 5,7:W
///
///  @brief    Parse --scroll option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_scroll(bool optlong, const char *const argv[])
{
    assert(argv != NULL);

    int nlines;
    int nbytes;

    if (sscanf(optarg, "%d%n", &nlines, &nbytes) == 1)
    {
        if (optarg[nbytes] == NUL && nlines > 0)
        {
            options.scroll = nlines;
            options.display = true;

            return;
        }
    }

    quit("Invalid argument '%s' for %s option", optarg,
         optlong ? "--scroll" : "-S");
}


///
///  @brief    Process unknown command-line option.
///
///  If optopt > 0, then we found an invalid short option and optopt was set to
//   the ASCII code for that option.
///
///  If there is no equals sign in the string pointed to by argv[optind-1], then
///  we have an invalid long option.
///
///  If optopt = 0, then we found an invalid long option and argv[optind-1]
///  points to the option plus the argument.
///
///  If optopt < 0, then we found a valid long option with an invalid argument
//   and argv[optind-1] points to the option.
///
///  @returns  We don't.
///
////////////////////////////////////////////////////////////////////////////////

static noreturn void opt_unknown(
    const char *const argv[])           ///< List of arguments
{
    static const char *badopt  = "Invalid option:";

    if (optopt > 0)
    {
        quit("%s -%c", badopt, optopt);
    }

    assert(argv != NULL);

    const char *p;
    const char *option = argv[optind - 1];

    assert(option != NULL);

    if ((p = strchr(option, '=')) == NULL)
    {
        quit("%s %s", badopt, option);
    }
    else if (optopt == 0)
    {
        quit("%s %.*s", badopt, p - option, option);
    }
    else
    {
        quit("Useless argument for option: %.*s", p - option, option);
    }
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

#if     defined(DEBUG)          // Include debug status for --version

    fputs(" (DEBUG)", stdout);          // Say that it's a debug version

#endif

    fputc('\n', stdout);

    printf("Copyright (C) 2019-2023 Nowwith Treble Software\n");

    exit(EXIT_SUCCESS);
}


///
///  @brief    Parse file options.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_files(
    int argc,                           ///< No. of arguments
    const char *const argv[])           ///< List of arguments
{
    static const char *edit_file = "EB%s\e Y :^AEditing file '%s'\1";
    static const char *read_file = "ER%s\e Y :^AReading file '%s'\1";

    assert(argv != NULL);               // Error if no argument list

    if (argc - optind > 2)              // Can't have more than two files
    {
        return;
    }
    else if (optind >= argc)
    {
        // No files, so find what we used in the last editing session

        char file[PATH_MAX] = { NUL };

        if (!read_memory(file, (uint)sizeof(file)))
        {
            if (options.readonly)       // --read-only requires a file
            {
                quit("How can I inspect nothing?");
            }
        }
        else if (access(file, F_OK) != 0)
        {
            store_cmd(":^ACan't find file '%s'\1", file);
            store_cmd(":^AIgnoring TECO's memory\1");
        }
        else if (options.readonly)
        {
            store_cmd(read_file, file, file);
        }
        else
        { 
            store_cmd(edit_file, file, file);
        }

        return;
    }

    // Here if we have at least one file argument

    const char *arg1 = argv[optind++];

    if (optind < argc)                  // Do we have a second file?
    {
        const char *arg2 = argv[optind++];
        dev_t device1, device2;         // Device IDs
        ino_t inode1, inode2;           // Inode numbers
        int error = stat_info(arg1, &device1, &inode1);

        // We have two names. See if they are pointing to the same file.

        if (error != 0 || stat_info(arg2, &device2, &inode2) != 0 ||
            device1 != device2 || inode1 != inode2)
        {
            // Input file must exist

            if (access(arg1, F_OK) != 0)
            {
                quit("Can't find file '%s'", arg1);
            }

            store_cmd(read_file, arg1, arg1);
            store_cmd(create_file, access(arg2, F_OK), arg2, arg2);

            teco_memory = NULL;         // Don't save file name on exit

            return;
        }

        //  Here if the two files are effectively the same, either because they
        //  have the same path, or the same device IDs and inode numbers.

        store_cmd(":^A'%s' and '%s' are the same file\1", arg1, arg2);
    }
    else if (access(arg1, F_OK) != 0)   // Write new file if it doesn't exist
    {
        if (!options.create)
        {
            quit("Can't find file '%s'", arg1);
        }
        else
        {
            store_cmd(":^ACan't find file '%s'\1", arg1);
            store_cmd(create_file, -1, arg1, arg1);

            return;
        }
    }

    //  Here if we only have one file

    if (options.readonly)               // Should we open for read only?
    {
        store_cmd(read_file, arg1, arg1);
    }
    else                                // No -- open for backup
    {
        store_cmd(edit_file, arg1, arg1);
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
    static const char *no_arg  = "Argument required for option %s";

    assert(argv != NULL);               // Error if no argument list
    assert(argv[0] != NULL);            // Error if no strings in list

    // These two assertions confirm the standard behavior of getopt_long()
    // regarding the ordering of option and non-option arguments.

    assert(optstring[0] != '+' && optstring[0] != '-'); // Verify assumptions
    assert(getenv("POSIXLY_CORRECT") == NULL); // Verify assumptions

    optind = 0;                         // Reset from any previous calls
    opterr = 0;                         // Suppress any error messages

    int c;                              // Current option

    while ((c = getopt_long(argc, (char * const *)argv,
                            optstring, long_options, NULL)) != -1)
    {
        //  The following handles the case where an option is missing a
        //  required argument, but is followed by one or more other options.

        if (optarg != NULL && optarg[0] == '-')
        {
            quit(no_arg, argv[optind - 2]);
        }

        bool optlong = (c < 0);         // true if long option, else false

        switch (abs(c))                 // long option = -(short option)
        {
            case OPT_display:
            case OPT_execute:
            case OPT_formfeed:
            case OPT_keys:
            case OPT_log:
            case OPT_make:
            case OPT_mung:
            case OPT_noformfeed:
            case OPT_text:         push_opt(c, optarg);           break;

            case OPT_arguments:    opt_arguments(optlong, argv);  break;
            case OPT_help:         opt_help();                    break;
            case OPT_scroll:       opt_scroll(optlong, argv);     break;
            case OPT_version:      opt_version();                 break;

            case OPT_create:       options.create   = true;       break;
            case OPT_exit:         options.exit     = true;       break;
            case OPT_initialize:   teco_init        = optarg;     break;
            case OPT_nocreate:     options.create   = false;      break;
            case OPT_noinitialize: teco_init        = NULL;       break;
            case OPT_nomemory:     teco_memory      = false;      break;
            case OPT_noread_only:  options.readonly = false;      break;
            case OPT_practice:     options.practice = true;       break; // (Hidden)
            case OPT_read_only:    options.readonly = true;       break;
            case OPT_nodefaults:   teco_init        = NULL;
                                   teco_memory      = NULL;
                                    //lint -fallthrough
            case OPT_nodisplay:    options.display  = false;
                                   teco_vtedit      = NULL;       break;

            case ':':
                if (optopt == -OPT_make)
                {
                    quit("How can I make nothing?");
                }
                else if (optopt == -OPT_mung)
                {
                    quit("How can I mung nothing?");
                }
                else
                {
                    quit(no_arg, argv[optind - 1]);
                }

            case '?':
            default:
                opt_unknown(argv);
        }
    }
}


///
///  @brief    Pop command-line options from stack.
///
///  @returns  true if it's okay to parse files, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool pop_opts(void)
{
    for (uint i = 0; i < options.next; ++i)
    {
        int c = options.stack[i];
        const char *arg = options.args[i];
        int nbytes;

        switch (c)
        {
            case OPT_arguments:
                options.mn_args = arg;

                break;

            case OPT_execute:
                //  See if the option argument is a literal command string
                //  delimited by a pair of single or double quotes.

                if ((nbytes = (int)strlen(arg)) >= 2)
                {
                    int first = arg[0];
                    int last = arg[nbytes - 1];

                    if (first == last && (first == '"' || first == '\''))
                    {
                        nbytes -= 2;
                        store_cmd("%.*s\e", nbytes, arg + 1);

                        break;
                    }
                }
                
                //  Here if we have a file name. Process the same as --mung.

                //lint -fallthrough

            case OPT_mung:
                if (options.mn_args != NULL)
                {
                    store_cmd("%sEI%s\e", options.mn_args, arg);

                    options.mn_args = NULL;
                }
                else
                {
                    store_cmd("EI%s\e", arg);
                }

                break;

            case OPT_formfeed:
                store_cmd("1,0E3");

                break;

            case OPT_noformfeed:
                store_cmd("0,1E3");

                break;

            case OPT_log:
                store_cmd("EL%s\e", arg);

                break;

            case OPT_make:
                // Can't skip the classic TECO message

                if (!strcasecmp(arg, "love"))
                {
                    store_cmd(":^ANot war?\1");
                }

                store_cmd(create_file, access(arg, F_OK), arg, arg);

                break;

            case OPT_text:
                store_cmd("I%s\e", arg);

                break;

            default:
                break;
        }
    }

    options.next = 0;

    // If --make or --mung were specified, then we shouldn't process any files.

    return !options.make && !options.mung;
}


///
///  @brief    Push option on stack. This makes it possible to ensure that any
///            --initialize option is always stored first in the command
///            string, and that any --display and --scroll options are stored
///            last. All other options are stored in the order in which they
///            occurred on the command line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void push_opt(int option, const char *arg)
{
    static const char *opt_conflict = "Conflicting option: %s";

    switch (option)
    {
        case OPT_display:
        case -OPT_display:
            options.display = true;

            if (arg != NULL)
            {
                teco_vtedit = arg;
            }

            return;

        case OPT_execute:
            if (options.make || options.mung)
            {
                quit(opt_conflict, "-E");
            }

            options.execute = true;
            teco_memory = NULL;         // Don't use memory file

            break;

        case -OPT_execute:
            if (options.make || options.mung)
            {
                quit(opt_conflict, "--execute");
            }

            options.execute = true;
            teco_memory = NULL;         // Don't use memory file

            break;

        case OPT_keys:
        case -OPT_keys:

#if     defined(DEBUG)          // Include --keys option

            key_name = optarg;

            return;

#else

            quit("Option --keys is only available in debug builds");

#endif

        case -OPT_make:
            if (options.execute || options.make || options.mung)
            {
                quit(opt_conflict, "--make");
            }

            options.make = true;

            break;


        case -OPT_mung:
            if (options.execute || options.make || options.mung)
            {
                quit(opt_conflict, "--mung");
            }

            options.mung = true;
            teco_memory = NULL;         // Don't use memory file

            break;

        default:
            break;
    }

    if (options.next == NOPTIONS)
    {
        quit(option_limit);
    }

    options.stack[options.next] = (char)abs(option);
    options.args[options.next] = arg;

    ++options.next;
}


///
///  @brief    Print error message and exit with failure. Note that we add a
///            newline to the output text, so our callers don't need to do so.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static noreturn void quit(const char *format, ...)
{
    assert(format != NULL);

    if (options.practice)
    {
        fprintf(stderr, "%s ", begin_tag);
    }

    va_list args;

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    if (options.practice)
    {
        fprintf(stderr, " %s\n", end_tag);
    }
    else
    {
        fputc('\n', stderr);
        fputs("Try 'teco --help' for more information\n", stderr);
    }

    exit(EXIT_FAILURE);
}


///
///  @brief    Get file information (device ID and inode number).
///
///  @returns  0 if no error, else errno code.
///
////////////////////////////////////////////////////////////////////////////////

static int stat_info(const char *file, dev_t *dev, ino_t *ino)
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
///  @brief    Save next command in command list.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void store_cmd(const char *format, ...)
{
    va_list args;

    va_start(args, format);

    uint size = cbuf->size - cbuf->len;
    int retval = vsnprintf(cbuf->data + cbuf->len, (size_t)size, format, args);

    va_end(args);

    assert(retval >= 0);

    size = (uint)retval;

    if (cbuf->len + size > cbuf->size)
    {
        quit(option_limit);
    }

    cbuf->len += size;

    if (cbuf->size > cbuf->len)         // Add a space to make it more readable
    {
        cbuf->data[cbuf->len++] = ' ';
    }
}
