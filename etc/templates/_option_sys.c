///
///  @file    _option_sys.c
///
///  @brief   Auxiliary file for option_sys.c, containing help message.
///
///           This file defines data that is statically stored, rather than
///           just being declared, and is intended to be used by only one
///           source file, through use of the #include directive. This is why
///           it is created in the include/ directory, with a file type of
///           .c, and with the same name (prefaced by a leading underscore)
///           as the file which will use it.
///
/* (INSERT: WARNING NOTICE) */
///
///  @brief   External resources:
///
///  <a href="options.html">List of Command-line options</a>
///
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
    "  teco foo               Open 'foo' for input and output.",
    "  teco -R foo            Open 'foo' for input only.",
    "  teco foo baz           Open 'foo' for input and 'baz' for output.",
    "  teco foo -O baz        Equivalent to 'teco foo baz'",
    "  teco -E foo            Execute the file 'foo' as a TECO macro.",
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

///  @var optstring
///  String of short options parsed by getopt_long().

static const char * const optstring = ":/* (INSERT: SHORT OPTIONS) */";

///  @var    long_options[]
///  @brief  Table of command-line options parsed by getopt_long().

static const struct option long_options[] =
{
/* (INSERT: LONG OPTIONS) */
    { NULL,             no_argument,        NULL,  0               },  // Markers for end of list
};

