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


#define NOPTIONS        64          ///< Total no. of string options

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
    int create;                     ///< --create option
    bool display;                   ///< --display option
    bool readonly;                  ///< --read-only option
    bool exit;                      ///< --exit option
    bool mung;                      ///< --mung option
    bool make;                      ///< --make option
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
    .create   = 0,
    .display  = true,
    .readonly = false,
    .exit     = false,
    .mung     = false,
    .make     = false,
    .practice = false,
};


// Local functions

static void opt_arguments(const char *const argv[]);

static void opt_display(void);

static void opt_execute(const char *const argv[]);

static void opt_help(void);

static void opt_initialize(const char *const argv[]);

static void opt_keys(void);

static void opt_log(const char *const argv[]);

static void opt_scroll(const char *const argv[]);

static void opt_version(void);

static void parse_files(int argc, const char *const argv[]);

static void parse_mung(const char *p);

static void parse_options(int argc, const char *const argv[]);

static void pop_opts(void);

static void push_opt(char option, const char *arg);

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

    pop_opts();                         // Pop any options saved on stack

    parse_files(argc, argv);

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
        fputs("!begin! ", stdout);

        for (uint i = 0; i < cbuf->len; ++i)
        {
            int c = cbuf->data[i];

            if (c == DEL || !isascii(c))
            {
                printf("[%02x]", c);
            }
            else if (iscntrl(c))
            {
                printf("^%c", c + 'A' - 1);
            }
            else
            {
                fputc(c, stdout);
            }
        }

        fputs("!end!\n", stdout);

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

static void opt_arguments(const char *const argv[])
{
    assert(argv != NULL);

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
            push_opt('A', optarg);

            return;
        }
    }

    assert(optind >= 2);

    quit("Invalid value '%s' for %s option", optarg, argv[optind - 2]);
}


///
///  @brief    Parse -D and --display options.
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
///  @brief    Parse -E and --execute options.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_execute(const char *const argv[])
{
    assert(argv != NULL);

    if (optarg[0] == '-')
    {
        assert(optind >= 2);

        quit("Invalid file name for %s option", argv[optind - 2]);
    }

    push_opt('E', optarg);
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
///  @brief    Parse -I and --initialize options.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_initialize(const char *const argv[])
{
    assert(argv != NULL);

    if (optarg[0] == '-')
    {
        assert(optind >= 2);

        quit("Invalid file name for %s option", argv[optind - 2]);
    }

    teco_init = optarg;
}


///
///  @brief    Parse --keys option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_keys(void)
{

#if     defined(DEBUG)          // Include --key option

    if (optarg != NULL && optarg[0] != '-')
    {
        key_name = optarg;
    }
    else
    {
        key_name = NULL;
    }

#else

    quit("--keys option is only available in debug builds");

#endif

}


///
///  @brief    Parse -L and --log options.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void opt_log(const char *const argv[])
{
    assert(argv != NULL);

    if (optarg[0] == '-')
    {
        assert(optind >= 2);

        quit("Invalid file name for %s option", argv[optind - 2]);
    }

    push_opt('L', optarg);
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

static void opt_scroll(const char *const argv[])
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

    assert(optind >= 2);

    quit("Invalid value '%s' for %s option", optarg, argv[optind - 2]);
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
    static const char *EB_cmd = "EB%s\e Y :^AEditing file '%s'\1";
    static const char *ER_cmd = "ER%s\e Y :^AReading file '%s'\1";
    static const char *EW_cmd = ":^ACreating new file '%s'\1 EW%s\e";

    assert(argv != NULL);               // Error if no argument list

    if (argc - optind > 2)              // Can't have more than two files
    {
        return;
    }
    else if (optind >= argc)            // Do have have any files?
    {
        if (options.mung)               //--mung requires a file
        {
            quit("How can I mung nothing?");
        }
        else if (options.make)          // --make requires a file
        {
            quit("How can I make nothing?");
        }

        // See if there is a memory file we can use

        if (teco_memory != NULL)
        {
            char file[PATH_MAX] = { NUL };

            read_memory(file, (uint)sizeof(file));

            if (file[0] != NUL)         // Does memory file contain anything?
            {
                if (access(file, F_OK) != 0)
                {
                    store_cmd(":^ACan't find file '%s'\1", file);
                    store_cmd(":^AIgnoring TECO's memory\1");
                }
                else if (options.readonly)
                {
                    store_cmd(ER_cmd, file, file);
                }
                else
                {
                    store_cmd(EB_cmd, file, file);
                }

                return;
            }
        }

        if (options.readonly)           // --read-only requires a file
        {
            quit("How can I inspect nothing?");
        }

        return;
    }

    // Here if we have at least one file argument

    const char *arg1 = argv[optind++];

    if (options.mung)
    {
        parse_mung(arg1);

        return;
    }
    else if (options.make)
    {
        if (!strcasecmp(arg1, "love"))
        {
            store_cmd(":^ANot war?\1"); // Classic TECO message
        }

        store_cmd(EW_cmd, arg1, arg1);

        return;
    }
    else if (optind < argc)             // Do we have a second file?
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

            store_cmd(ER_cmd, arg1, arg1);
            store_cmd(EW_cmd, arg2, arg2);

            teco_memory = NULL;         // Don't save file name on exit

            return;
        }

        //  Here if the two files are effectively the same, either because they
        //  have the same path, or the same device IDs and inode numbers.

        store_cmd(":^A'%s' and '%s' are the same file\1", arg1, arg2);
    }
    else if (access(arg1, F_OK) != 0)   // Write new file if it doesn't exist
    {
        store_cmd(":^ACan't find file '%s'\1", arg1);
        store_cmd(EW_cmd, arg1, arg1);

        return;
    }

    //  Here if we only have one file

    if (options.readonly)               // Should we open for read only?
    {
        store_cmd(ER_cmd, arg1, arg1);
    }
    else                                // No -- open for backup
    {
        store_cmd(EB_cmd, arg1, arg1);
    }
}


///
///  @brief    Parse argument for --execute and --mung options. There are
///            basically two options: a file name, and a command string. The
///            latter is denoted by an argument more than two characters in
///            length that starts and ends with single or double quotes.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_mung(const char *p)
{
    assert(p != NULL);

    uint nbytes = (uint)strlen(p);

    teco_memory = NULL;     // Don't read memory file if munging

    if (nbytes > 2)
    {
        int first = p[0];
        int last = p[nbytes - 1];

        if (first == last && (first == '"' || first == '\''))
        {
            nbytes -= 2;
            store_cmd("%.*s\e", (int)nbytes, p + 1);

            return;
        }
    }

    // Here when we know we don't have a command string.

    if (options.mn_args != NULL)
    {
        store_cmd("%sEI%s\e", options.mn_args, p);

        options.mn_args = NULL;
    }
    else
    {
        store_cmd("EI%s\e", p);
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
    static const char *badopt  = "Unknown option";
    static const char *usehelp = "use --help for a list of options";

    assert(argv != NULL);               // Error if no argument list
    assert(argv[0] != NULL);            // Error if no strings in list

    // These two assertions confirm the standard behavior of getopt_long()
    // regarding the ordering of option and non-option arguments.

    assert(optstring[0] != '+' && optstring[0] != '-'); // Verify assumptions
    assert(getenv("POSIXLY_CORRECT") == NULL); // Verify assumptions

    optind = 0;                         // Reset from any previous calls
    opterr = 0;                         // Suppress any error messages

    int c;
    int lastopt = 1;                    // Used to analyze errors (see below)

    while ((c = getopt_long(argc, (char * const *)argv,
                            optstring, long_options, NULL)) != -1)
    {
        switch (c)
        {
            // Options that just set values

            case OPT_create:       options.create   = 1;        break;
            case OPT_exit:         options.exit     = true;     break;
            case OPT_make:         options.make     = true;     break;
            case OPT_mung:         options.mung     = true;     break;
            case OPT_nocreate:     options.create   = -1;       break;
            case OPT_nodefaults:   teco_init        = NULL;
                                   teco_memory      = NULL;
                                   //lint -fallthrough
            case OPT_nodisplay:    options.display  = false;
                                   teco_vtedit      = NULL;     break;
            case OPT_noinitialize: teco_init        = NULL;     break;
            case OPT_nomemory:     teco_memory      = false;    break;
            case OPT_noread_only:  options.readonly = false;    break;
            case OPT_practice:     options.practice = true;     break; // (Hidden)
            case OPT_read_only:    options.readonly = true;     break;

            // Options that call functions

            case OPT_arguments:    opt_arguments(argv);         break;
            case OPT_display:      opt_display();               break;
            case OPT_execute:      opt_execute(argv);           break;
            case OPT_formfeed:     push_opt('F', NULL);         break;
            case OPT_help:         opt_help();                  break;
            case OPT_initialize:   opt_initialize(argv);        break;
            case OPT_keys:         opt_keys();                  break;
            case OPT_log:          opt_log(argv);               break;
            case OPT_noformfeed:   push_opt('f', NULL);         break;
            case OPT_scroll:       opt_scroll(argv);            break;
            case OPT_text:         push_opt('T', optarg);       break;
            case OPT_version:      opt_version();               break;

            case ':':
                quit("Missing argument for %s option", argv[optind - 1]);

            case '?':
            default:
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
                    quit("%s '-%c': %s", badopt, optopt, usehelp);
                }
                else
                {
                    //  If an option has an argument which isn't required or
                    //  allowed, getopt_long() treats the entire string as an
                    //  option. So we look for an equals sign delimiting the
                    //  option from the argument, and if found we complain
                    //  about the extraneous argument rather than carping
                    //  about an unknown option.

                    const char *arg = argv[optind - 1];
                    const char *p;

                    if ((p = strchr(arg, '=')) != NULL)
                    {
                        quit("No arguments allowed for option '%.*s'",
                             p - arg, arg);
                    }
                    else
                    {
                        quit("%s '%s': %s", badopt, arg, usehelp);
                    }
                }
        }

        lastopt = optind;
    }
}


///
///  @brief    Pop command-line options from stack.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void pop_opts(void)
{
    for (uint i = 0; i < options.next; ++i)
    {
        const char *arg = options.args[i];

        switch (options.stack[i])
        {
            case 'A': options.mn_args = arg;    break;

            case 'E': parse_mung(arg);          break;

            case 'F': store_cmd("1,0E3");       break;

            case 'f': store_cmd("0,1E3");       break;

            case 'L': store_cmd("EL%s\e", arg); break;

            case 'T': store_cmd("I%s\e", arg);  break;

            default:                            break;
        }
    }

    options.next = 0;
}


///
///  @brief    Push option on stack. We do this so that we can always ensure
///            that any --initialize option can always be stored first in the
///            command string that we create.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void push_opt(char option, const char *arg)
{
    if (options.next == NOPTIONS)
    {
        quit("Too many options");
    }

    options.stack[options.next] = option;
    options.args[options.next] = arg;

    ++options.next;
}


///
///  @brief    Print error message and exit with failure. Note that we add a
///            newline to the output text.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static noreturn void quit(const char *format, ...)
{
    assert(format != NULL);

    va_list args;

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    fputc('\n', stdout);

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
        quit("No memory for command-line options");
    }

    cbuf->len += size;

    if (cbuf->size > cbuf->len)         // Add a space to make it more readable
    {
        cbuf->data[cbuf->len++] = ' ';
    }
}
