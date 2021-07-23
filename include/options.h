///
///  @file       options.h
///
///  @brief      Header file for teco utility.
///
///  *** Automatically generated file. DO NOT MODIFY. ***
///
///  @brief      External resources:
///
///  <a href="options.html">List of Command-line options</a>
///
///  @brief      Process command-line options for TECO editor.
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

///  @brief  Help text string.

static const char * const help_text[] =
{
    "Usage: teco [options] [file]...",
    "",
    "TECO (Text Editor and Corrector) is a character-oriented text",
    "editing language for reading and writing ASCII text files.",
    "",
    "Examples:",
    "",
    "  teco abc               Open file 'abc' for input and output.",
    "  teco -R abc            Open file 'abc' for input only.",
    "  teco -O xyz abc        Open file 'abc' for input and file 'xyz' for output.",
    "  teco -E abc            Execute file 'abc' as a TECO macro.",
    "",
    "Environment variables:",
    "",
    "  TECO_INIT              Default initialization file, executed at startup.",
    "  TECO_LIBRARY           Directory of library for TECO macros.",
    "  TECO_MEMORY            File that contains name of last file edited.",
    "  TECO_VTEDIT            Default file for initialization of display mode.",
    "",
    "Text file options:",
    "",
    "  -C, --create           Create new file if file does not exist.",
    "  -c, --nocreate         Do not create new file if file does not exist.",
    "  -O, --output=abc       Open file 'abc' for output.",
    "  -o, --nooutput         Use same name for output file as input file.",
    "  -R, --read-only        Open file for input only.",
    "  -r, --noread-only      Open file for input and output.",
    "",
    "Indirect command file options:",
    "",
    "  -A, --argument=n       Store numeric value 'n' in Q-register A.",
    "  -B, --buffer=xyz       Store text 'xyz' in edit buffer.",
    "  -E, --execute=xyz      Execute TECO macro in file 'xyz'.",
    "",
    "Initialization options:",
    "",
    "  -I, --initialize=xyz   Use initialization file 'xyz' at startup.",
    "  -i, --noinitialize     Ignore TECO_INIT environment variable.",
    "  -M, --memory           Use TECO_MEMORY to get name of last file edited.",
    "  -m, --nomemory         Ignore TECO_MEMORY environment variable.",
    "",
    "Display options:",
    "",
    "  -D, --display          Enable display mode.",
    "  -S, --scroll=n         Use 'n' lines for scrolling region (implies -D).",
    "  -V, --vtedit=xyz       Use macro in file 'xyz' to initialize display.",
    "  -v, --novtedit         Ignore TECO_VTEDIT environment variable.",
    "",
    "Debug options:",
    "",
    "  -F, --formfeed         Enables FF as a page delimiter.",
    "  -f, --noformfeed       Disables FF as a page delimiter.",
    "  -L, --log=xyz          Saves input and output in log file 'xyz'.",
    "  -Z, --zero=n           Enable syntax restrictions by setting E2 to 'n'.",
    "",
    "Miscellaneous options:",
    "",
    "  -n, --nodefaults       Disable all defaults (equivalent to -c -i -m -v).",
    "  -H, --help             Print this help message.",
    "  -X, --exit             Exit from TECO after executing all command-line options.",
    NULL
};

///  @enum     option_t
///  case values for command-line options.

enum option_t
{
    OPTION_A = 'A',
    OPTION_B = 'B',
    OPTION_C = 'C',
    OPTION_D = 'D',
    OPTION_E = 'E',
    OPTION_F = 'F',
    OPTION_H = 'H',
    OPTION_I = 'I',
    OPTION_L = 'L',
    OPTION_M = 'M',
    OPTION_O = 'O',
    OPTION_R = 'R',
    OPTION_S = 'S',
    OPTION_V = 'V',
    OPTION_X = 'X',
    OPTION_Z = 'Z',
    OPTION_c = 'c',
    OPTION_f = 'f',
    OPTION_i = 'i',
    OPTION_m = 'm',
    OPTION_n = 'n',
    OPTION_o = 'o',
    OPTION_r = 'r',
    OPTION_v = 'v'
};

///  @var optstring
///  String of short options parsed by getopt_long().

static const char * const optstring = "A:B:CDE:FHI::L:MO:RS:V::XZ::cfimnorv";

///  @var    long_options[]
///  @brief  Table of command-line options parsed by getopt_long().

static const struct option long_options[] =
{
    { "argument",       required_argument,  NULL,  'A'    },
    { "buffer",         required_argument,  NULL,  'B'    },
    { "create",         no_argument,        NULL,  'C'    },
    { "display",        no_argument,        NULL,  'D'    },
    { "execute",        required_argument,  NULL,  'E'    },
    { "formfeed",       no_argument,        NULL,  'F'    },
    { "help",           no_argument,        NULL,  'H'    },
    { "initialize",     optional_argument,  NULL,  'I'    },
    { "log",            required_argument,  NULL,  'L'    },
    { "memory",         no_argument,        NULL,  'M'    },
    { "output",         required_argument,  NULL,  'O'    },
    { "read-only",      no_argument,        NULL,  'R'    },
    { "scroll",         required_argument,  NULL,  'S'    },
    { "vtedit",         optional_argument,  NULL,  'V'    },
    { "exit",           no_argument,        NULL,  'X'    },
    { "zero",           optional_argument,  NULL,  'Z'    },
    { "nocreate",       no_argument,        NULL,  'c'    },
    { "noformfeed",     no_argument,        NULL,  'f'    },
    { "noinitialize",   no_argument,        NULL,  'i'    },
    { "nomemory",       no_argument,        NULL,  'm'    },
    { "nodefaults",     no_argument,        NULL,  'n'    },
    { "nooutput",       no_argument,        NULL,  'o'    },
    { "noread-only",    no_argument,        NULL,  'r'    },
    { "novtedit",       no_argument,        NULL,  'v'    },
    { NULL,             no_argument,        NULL,  0      },  // Markers for end of list
};

