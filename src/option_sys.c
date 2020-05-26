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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if     !defined(__DECC)

#include <getopt.h>

#endif

#include "teco.h"
#include "ascii.h"
#include "eflags.h"


// TODO: the following conditional code is temporary until we figure
//       out how to process command-line options with VMS C compiler.

#if     defined(__DECC)

static const int no_argument = 0;
static const int required_argument = 1;
static const int optional_argument = 2;

struct option
{
    const char* name;
    int has_arg;
    int* flag;
    int val;
};

static int getopt_long(int argc, char* const argv[],
    const char* optstring, const struct option* longopts, int* longindex);

static int getopt_long(int argc, char* const argv[],
    const char* optstring, const struct option* longopts, int* longindex)
{
    return -1;
}

#endif


///  @enum     option_t
///  case values for command-line options.

enum option_t
{
    OPTION_c = 'c',
    OPTION_d = 'd',
    OPTION_e = 'e',
    OPTION_l = 'l',
    OPTION_m = 'm',
    OPTION_n = 'n',
    OPTION_r = 'r',
    OPTION_s = 's',
    OPTION_w = 'w',
    OPTION_x = 'x'
};

///  @var optstring
///  String of short options parsed by getopt_long().

static const char * const optstring = "cde:l:m:nrs:wx";

///  @var    long_options[]
///  @brief  Table of command-line options parsed by getopt_long().

static const struct option long_options[] =
{
    { "create",    no_argument,        NULL,  'c'    },
    { "execute",   required_argument,  NULL,  'e'    },
    { "log",       required_argument,  NULL,  'l'    },
    { "dry-run",   no_argument,        NULL,  'd'    },
    { "mung",      required_argument,  NULL,  'm'    },
    { "nomemory",  no_argument,        NULL,  'n'    },
    { "readonly",  no_argument,        NULL,  'r'    },
    { "read_only", no_argument,        NULL,  'r'    },
    { "ro",        no_argument,        NULL,  'r'    },
    { "scroll",    required_argument,  NULL,  's'    },
    { "window",    no_argument,        NULL,  'w'    },
    { "exit",      no_argument,        NULL,  'x'    },
    { NULL,        no_argument,        NULL,  0      },  // Markers for end of list
};

// Local functions

static void read_memory(void);

static void store_cmd(const char *p);

static void write_memory(const char *file);

    
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
    char command[64];
    char edit = 'B';                    // Assume EB command
    const char *error = NULL;
    bool nomemory = false;

    optind = 0;                         // Reset from any previous calls
    opterr = 0;                         // Suppress any error messages

    // If TECO_INIT is defined, then assume it has the name of our
    // initialization file. If not defined, then use the default name.

    const char *eifile = getenv("TECO_INIT") ?: "teco.ini";

    sprintf(command, ":@EI|%s| ", eifile);
    store_cmd(command);

    while ((c = getopt_long(argc, (char * const *)argv,
                            optstring, long_options, &idx)) != -1)
    {
        command[0] = NUL;

        switch (c)
        {
            case OPTION_c:
                edit = 'W';
                error = "make";

                break;

            case OPTION_d:
                f.e0.dryrun = true;

                break;

            case OPTION_e:
                sprintf(command, "%s ", optarg);
                store_cmd(command);

                break;

            case OPTION_l:
                sprintf(command, "@EL/%s/ ", optarg);
                store_cmd(command);

                break;

            case OPTION_m:
                if (optarg[0] == '-')
                {
                    printf("?How can I mung nothing?\r\n");

                    exit(EXIT_FAILURE);
                }

                sprintf(command, "@EI|%s| ", optarg);
                store_cmd(command);

                break;

            case OPTION_n:
                nomemory = true;

                break;

            case OPTION_r:
                edit = 'R';
                error = "inspect";

                break;

            case OPTION_s:
                sprintf(command, "1W %s,7:W ", optarg);
                store_cmd(command);

                break;

            case OPTION_w:
                strcpy(command, "1W ");
                store_cmd(command);

                break;

            case OPTION_x:
                f.e0.exit = true;

                break;

            default:
                printf("%%Ignoring unknown option: '%s'\r\n", argv[optind - 1]);

                break;
        }
    }

    if ((argc -= optind) > 1)
    {
        printf("?Too many parameters\r\n");

        exit(EXIT_FAILURE);
    }
    else if (argc == 1)                 // We have EB, ER, or EW w/ file
    {
        sprintf(command, "@E%c|%s| %c ", edit, argv[optind],
                edit == 'R' ? 'Y' : 'P');
        store_cmd(command);

        if (edit != 'R')                // Save memory if EB or EW
        {
            write_memory(argv[optind]);
        }
    }
    else if (error != NULL)             // Error if ER or EW w/o file
    {
        printf("?How can I %s nothing?\r\n", error);

        exit(EXIT_FAILURE);
    }
    else if (!nomemory)
    {
        read_memory();
    }
}


///
///  @brief    Read file specification from memory file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_memory(void)
{
    const char *memory = getenv("TECO_MEMORY") ?: "teco.mem";
    FILE *fp;
    char file[128 + 1];                // TODO: magic number
    char command[128 + 8 + 1];         // TODO: magic numbers

    if ((fp = fopen(memory, "r")) == NULL)
    {
        if (errno != ENOENT && errno != ENODEV)
        {
            printf("%%Can't open memory file '%s'\r\n", memory);
        }

        return;
    }

    if (fgets(file, sizeof(file), fp) == NULL)
    {
        printf("%%Can't read from memory file '%s'\r\n", memory);
    }
    else
    {
        uint len = (uint)strlen(file);

        while (len && iscntrl(file[len - 1]))
        {
            file[--len] = '\0';
        }

        if (len != 0)
        {
            printf("%%Editing file '%s'\r\n", file);
            sprintf(command, "@EB|%s| P ", file);
            store_cmd(command);
        }
    }

    fprintf(fp, "%s\n", file);

    fclose(fp);

}


///
///  @brief    Store command-line option in command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void store_cmd(const char *p)
{
    assert(p != NULL);

    int c;

    while ((c = *p++) != NUL)
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

static void write_memory(const char *file)
{
    const char *memory = getenv("TECO_MEMORY") ?: "teco.mem";
    FILE *fp;

    if ((fp = fopen(memory, "w")) == NULL)
    {
        printf("%%Can't open memory file '%s'\r\n", memory);

        return;
    }

    fprintf(fp, "%s\n", file);

    fclose(fp);

}
