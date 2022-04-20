///
///  @file       options.h
///
///  @brief      Header file for teco utility.
///
/* (INSERT: WARNING NOTICE) */
///
///  @brief      External resources:
///
///  <a href="options.html">List of Command-line options</a>
///
///  @brief      Process command-line options for TECO editor.
///
///  @copyright 2019-2022 Franklin P. Johnston / Nowwith Treble Software
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
    "  teco abc               Open 'abc' for input and output.",
    "  teco -R abc            Open 'abc' for input only.",
    "  teco abc xyz           Open 'abc' for input and 'xyz' for output.",
    "  teco abc -O xyz        Equivalent to 'teco abc xyz'",
    "  teco -E abc            Execute the file 'abc' as a TECO macro.",
    "",
    "Environment variables:",
    "",
    "  TECO_INIT              Default initialization file, executed at startup.",
    "  TECO_LIBRARY           Directory of library for TECO macros.",
    "  TECO_MEMORY            File that contains name of last file edited.",
    "  TECO_VTEDIT            Default file for initialization of display mode.",
/* (INSERT: HELP OPTIONS) */
    NULL
};

///  @enum     option_t
///  case values for command-line options.

enum option_t
{
/* (INSERT: ENUM OPTIONS) */
};

/* (INSERT: DEBUG OPTIONS) */

///  @var optstring
///  String of short options parsed by getopt_long().

static const char * const optstring = ":/* (INSERT: SHORT OPTIONS) */";

///  @var    long_options[]
///  @brief  Table of command-line options parsed by getopt_long().

static const struct option long_options[] =
{
/* (INSERT: LONG OPTIONS) */
    { "mung",           no_argument,        NULL,  DEL    },  // --mung option (hidden)
    { NULL,             no_argument,        NULL,  0      },  // Markers for end of list
};

