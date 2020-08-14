///
///  @file       _options.h
///
///  @brief      Header file for teco utility.
///              *** Automatically generated file. DO NOT MODIFY. ***
///
///  @brief      External resources:
///
///  <a href="options.html">List of Command-line options</a>
///
///  @brief      Process command-line options for TECO editor.
///
///  @author     Franklin P. Johnston / Nowwith Treble Software
///
///  @bug        No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
    "  TECO_MEMORY            File that contains name of last file edited.",
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
    "  -X, --exit             Exit from TECO when indirect command file is closed.",
    "",
    "Initialization options:",
    "",
    "  -I, --initialize=xyz   Use initialization file 'xyz' at startup.",
    "  -i, --noinitialize     Don't use an initialization file (ignore TECO_INIT).",
    "  -M, --memory           Use TECO_MEMORY to get name of last file edited.",
    "  -m, --nomemory         Ignore TECO_MEMORY.",
    "",
    "Window options:",
    "",
    "  -S, --scroll=n         Use 'n' lines for scrolling region (implies -W).",
    "  -W, --window           Enable window mode.",
    "",
    "Debug options:",
    "",
    "  -L, --log=xyz          Saves input and output in log file 'xyz'.",
    "  -Z, --zero             Strictly enforce command syntax (zero tolerance).",
    "",
    "Miscellaneous options:",
    "",
    "  -H, --help             Print this help message",
    NULL
};

///  @enum     option_t
///  case values for command-line options.

enum option_t
{
    OPTION_A = 'A',
    OPTION_B = 'B',
    OPTION_C = 'C',
    OPTION_E = 'E',
    OPTION_H = 'H',
    OPTION_I = 'I',
    OPTION_L = 'L',
    OPTION_M = 'M',
    OPTION_O = 'O',
    OPTION_R = 'R',
    OPTION_S = 'S',
    OPTION_W = 'W',
    OPTION_X = 'X',
    OPTION_Z = 'Z',
    OPTION_c = 'c',
    OPTION_i = 'i',
    OPTION_m = 'm',
    OPTION_o = 'o',
    OPTION_r = 'r'
};

///  @var optstring
///  String of short options parsed by getopt_long().

static const char * const optstring = "A:B:CE:HI::L:MO:RS:WXZ::cimor";

///  @var    long_options[]
///  @brief  Table of command-line options parsed by getopt_long().

static const struct option long_options[] =
{
    { "argument",       required_argument,  NULL,  'A'    },
    { "buffer",         required_argument,  NULL,  'B'    },
    { "create",         no_argument,        NULL,  'C'    },
    { "execute",        required_argument,  NULL,  'E'    },
    { "help",           no_argument,        NULL,  'H'    },
    { "initialize",     optional_argument,  NULL,  'I'    },
    { "log",            required_argument,  NULL,  'L'    },
    { "memory",         no_argument,        NULL,  'M'    },
    { "output",         required_argument,  NULL,  'O'    },
    { "read-only",      no_argument,        NULL,  'R'    },
    { "scroll",         required_argument,  NULL,  'S'    },
    { "window",         no_argument,        NULL,  'W'    },
    { "exit",           no_argument,        NULL,  'X'    },
    { "zero",           optional_argument,  NULL,  'Z'    },
    { "nocreate",       no_argument,        NULL,  'c'    },
    { "noinitialize",   no_argument,        NULL,  'i'    },
    { "nomemory",       no_argument,        NULL,  'm'    },
    { "nooutput",       no_argument,        NULL,  'o'    },
    { "noread-only",    no_argument,        NULL,  'r'    },
    { NULL,             no_argument,        NULL,  0      },  // Markers for end of list
};

