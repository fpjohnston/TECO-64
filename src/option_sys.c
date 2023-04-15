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
#include "cbuf.h"
#include "eflags.h"
#include "file.h"
#include "options.h"
#include "term.h"
#include "version.h"


///
///   @struct  cmdq
///
///   @brief   Structure for queueing commands created from command-line options.
///

struct cmdq
{
    struct cmdq *next;                  ///< Next in queue
    char *text;                         ///< TECO command
};

static struct cmdq *qhead;              ///< Command list head
static struct cmdq *qtail;              ///< Command list tail

//  Queue insertion options

enum
{
    HEAD = 1,               ///< Push at head of queue
    TAIL                    ///< Push at tail of queue
};

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
    bool display;           ///< --display option
    bool readonly;          ///< --read-only option
    bool exit;              ///< --exit option
    bool mung;              ///< --mung option
    bool print;             ///< --print option
    bool quit;              ///< --quit option
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

static void pop_cmds(void);

static void print_cmd(const char *cmd);

static void push_cmd(int where, const char *format, ...);

static void push_EB(const char *file);

static void push_ER(const char *file);

static void push_EW(const char *file);

static void read_options(int argc, const char *const argv[]);


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
        push_cmd(HEAD, "EI%s\e ", teco_init);
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

    if (options.exit)                   // Should we exit at end of commands?
    {
        push_cmd(TAIL, "EX ");
    }
    else if (!f.e0.i_redir)
    {
        if (teco_vtedit != NULL)        // Do we have a display initialization file?
        {
            push_cmd(TAIL, "EI%s\e ", teco_vtedit);
        }

        if (options.display)            // Enable display if requested
        {
            push_cmd(TAIL, "-1W ");

            if (options.scroll != 0)    // Set up scrolling if requested
            {
                push_cmd(TAIL, "%u,7:W\e ", options.scroll);
            }
        }
    }

    if (qhead != NULL)                  // Anything stored?
    {
        push_cmd(TAIL, "\e\e");
    }

    pop_cmds();                         // Now pop all commands and store them
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

    assert(qhead == NULL);
    assert(qtail == NULL);

    read_options(argc, argv);

    if (options.quit)                   // --quit implies --print
    {
        options.print = true;
    }

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

    if (options.print)
    {
        printf("Commands: ");
    }

    exec_options();

    if (options.print)
    {
        fputc('\n', stdout);
    }

    if (options.quit)
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
        
        if (teco_memory != NULL)        // Try to read memory file
        {
            read_memory(memory, (uint)sizeof(memory));

            if (memory[0] != NUL)
            {
                push_EB(memory);

                return;
            }
        }

        // Here if we have no file to open, so just start TECO, unless the
        // --read-only option was used, in which case we print an error.

        if (options.readonly)
        {
            push_cmd(TAIL, ":^A?How can I inspect nothing?\1 ");

            options.exit = true;
        }

        return;
    }

    // Here if we have an input file

    if (outfile == NULL)                // If no output file, use EW or EB
    {
        if (access(infile, F_OK) != 0 && options.create)
        {
            push_EW(infile);            // EW if no file and okay to create one
        }
        else
        {
            push_EB(infile);            // Else try EB
        }
    }
    else                                // Open separate input and output files
    {
        push_ER(infile);
        push_EW(outfile);

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
    push_cmd(TAIL, "%s%s\e ", args, option);
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
    check_file("-L or --log");
    push_cmd(TAIL, "EL%s\e ", optarg);
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
            push_cmd(TAIL, "%.*s\e ", (int)nbytes, file + 1);

            return;
        }
    }

    if (args == NULL)
    {
        args = "";
    }

    push_cmd(TAIL, "%sEI%s\e ", args, file);
}


///
///  @brief    Parse -M or --memory option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_M(void)
{
    check_file("-M or --memory");

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

static void parse_T(void)
{
    check_arg("argument", "-T or --text");
    push_cmd(TAIL, "I%s\e ", optarg);
}


///
///  @brief    Parse -V or --vtedit option.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void parse_V(void)
{
    check_file("-V or --vtedit");

    teco_vtedit = optarg;
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
///  @brief    Pop all commands off queue and store them in command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void pop_cmds(void)
{
    struct cmdq *cmd;

    qtail = NULL;

    while ((cmd = qhead) != NULL)
    {
        int c;
        const char *p = cmd->text;

        if (options.print)
        {
            print_cmd(cmd->text);
        }

        while ((c = *p++) != NUL)
        {
            store_cbuf(c);
        }

        qhead = cmd->next;

        free_mem(&cmd->text);
        free_mem(&cmd);
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

    int c;

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
}


///
///  @brief    Save next command in command list.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void push_cmd(int where, const char *format, ...)
{
    va_list args;

    va_start(args, format);

    // First figure out how many bytes we'll need for this string

    int nbytes = vsnprintf(NULL, 0uL, format, args);

    va_end(args);

    assert(nbytes >= 0);

    uint size = (uint)nbytes;

    // Allocate a command block and initialize it

    struct cmdq *cmd = alloc_mem((uint)sizeof(struct cmdq));

    cmd->next = NULL;
    cmd->text = alloc_mem(size + 1);
    
    // Now copy the command for real

    va_start(args, format);

    vsnprintf(cmd->text, (size_t)size + 1, format, args);

    va_end(args);

    if (qhead == NULL)
    {
        qhead = qtail = cmd;
    }
    else if (where == HEAD)             // Add to head of queue?
    {
        cmd->next = qhead;
        qhead = cmd;
    }
    else                                // No, add to tail
    {
        qtail->next = cmd;
        qtail = cmd;
    }

}


///
///  @brief    Store EB command. Note that if the file open fails, the rest of
//             the command string will be aborted, which means that the CTRL/A
//             command won't be executed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void push_EB(const char *file)
{
    if (options.readonly)               // Should we open for read only?
    {
        push_ER(file);
    }
    else                                // No -- open for backup
    {
        push_cmd(TAIL, "EB%s\e Y :^AEditing file: %s\1 ", file, file);
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

static void push_ER(const char *file)
{
    push_cmd(TAIL, "ER%s\e Y :^AReading file: %s\1 ", file, file);
}


///
///  @brief    Store EW command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void push_EW(const char *file)
{
    push_cmd(TAIL, ":^AWriting file: %s\1 EW%s\e ", file, file);
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
            case OPTION_D: options.display = true;      break;
            case OPTION_E: parse_E();                   break;
            case OPTION_F: push_cmd(TAIL, "1,0E3 ");    break;
            case OPTION_f: push_cmd(TAIL, "0,1E3 ");    break;
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
            case OPTION_v: teco_vtedit = NULL;          break;
            case OPTION_X: options.exit = true;         break;

            // Hidden options start here

            case OPTION_version: parse_version();       break;
            case OPTION_mung:    options.mung = true;   break;
            case OPTION_print:   options.print = true;  break;
            case OPTION_quit:    options.quit  = true;  break;

#if     defined(DEBUG)

            // Debug options start here

            case OPTION_keys: parse_keys();             break;
            case OPTION_E1:   parse_eflag("E1 ");       break;
            case OPTION_E2:   parse_eflag("E2 ");       break;
            case OPTION_E3:   parse_eflag("E3 ");       break;
            case OPTION_E4:   parse_eflag("E4 ");       break;

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
