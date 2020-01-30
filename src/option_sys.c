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
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "eflags.h"


///  @enum     option_t
///  case values for command-line options.

enum option_t
{
    OPTION_l = 'l',
    OPTION_m = 'm',
    OPTION_n = 'n',
    OPTION_w = 'w',
    OPTION_x = 'x'
};

///  @var optstring
///  String of short options parsed by getopt_long().

static const char * const optstring = "l:m:nwx";

///  @var    long_options[]
///  @brief  Table of command-line options parsed by getopt_long().

static const struct option long_options[] =
{
    { "dry-run", no_argument,        NULL,  'n'    },
    { "exit",    no_argument,        NULL,  'x'    },
    { "log",     required_argument,  NULL,  'l'    },
    { "mung",    required_argument,  NULL,  'm'    },
    { "window",  no_argument,        NULL,  'w'    },
    { NULL,      no_argument,        NULL,  0      },  // Markers for end of list
};

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
    const char * const argv[])          ///< List of arguments
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

    while ((c = getopt_long(argc, (char * const *)argv,
                            optstring, long_options, &idx)) != -1)
    {
        switch (c)
        {
            case OPTION_l:
                log_file = optarg;

                break;

            case OPTION_m:
                mung_file = optarg;

                break;

            case OPTION_n:
                f.e0.dryrun = true;

                break;

            case OPTION_w:
                f.e0.window = true;

                break;

            case OPTION_x:
                f.e0.exit = true;

                break;

            case '?':
                exit(EXIT_FAILURE);

                break;

            default:
                // TODO: add a warning/error message here

                break;
        }
    }
}
