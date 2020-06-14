///
///  @file    errors.c
///  @brief   TECO error messages and error functions.
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

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"
#include "term.h"
#include "window.h"


#define ERR_BUF_SIZE    64      ///< Size of error buffer

///  @struct  err_table
///  @brief   Structure of error table.

struct err_table
{
    const char *code;           ///< Three-letter error code
    const char *text;           ///< Short description of error
};

///  @struct  err_table
///  @var     err_table
///  @brief   Text definitions for TECO error messages.

static struct err_table err_table[] =
{
    [E_NUL] = { "---",  "Unknown error code" },
    [E_ARG] = { "ARG",  "Improper arguments" },
    [E_BNI] = { "BNI",  "> not in iteration" },
    [E_BRC] = { "BRC",  "Invalid braced expression" },
    [E_CHR] = { "CHR",  "Invalid character for command" },
    [E_CON] = { "CON",  "Confused use of conditionals" },
    [E_CPQ] = { "CPQ",  "Can't pop into Q-register" },
    [E_DEV] = { "DEV",  "Invalid device" },
    [E_DIV] = { "DIV",  "Division by zero" },
    [E_DTB] = { "DTB",  "Delete too big" },
    [E_DUP] = { "DUP",  "Duplicate tag !%s!" },
    [E_EGC] = { "EGC",  "EG command is too long" },
    [E_FER] = { "FER",  "File error" },
    [E_FNF] = { "FNF",  "File not found '%s'" },
    [E_ICE] = { "ICE",  "Illegal ^E command in search argument" },
    [E_IEC] = { "IEC",  "Illegal character '%s' after E" },
    [E_IFC] = { "IFC",  "Illegal character '%s' after F" },
    [E_IFE] = { "IFE",  "Ill-formed numeric expression" },
    [E_IFN] = { "IFN",  "Illegal character '%s' in filename" },
    [E_IIA] = { "IIA",  "Illegal insert arg" },
    [E_ILL] = { "ILL",  "Illegal command '%s'" },
    [E_ILN] = { "ILN",  "Illegal number" },
    [E_INI] = { "INI",  "Initialization error" },
    [E_INP] = { "INP",  "Input error" },
    [E_IPA] = { "IPA",  "Negative or 0 argument to P" },
    [E_IQC] = { "IQC",  "Illegal \" character" },
    [E_IQN] = { "IQN",  "Illegal Q-register name '%s'" },
    [E_IRA] = { "IRA",  "Illegal radix argument to ^R" },
    [E_ISA] = { "ISA",  "Illegal search argument" },
    [E_ISS] = { "ISS",  "Illegal search string" },
    [E_IUC] = { "IUC",  "Illegal character '%s' following ^" },
    [E_MAP] = { "MAP",  "Missing '" },
    [E_MEM] = { "MEM",  "Memory overflow" },
    [E_MIX] = { "MIX",  "Maximum insert string exceeded" },
    [E_MLA] = { "MLA",  "Missing left angle bracket" },
    [E_MLX] = { "MLX",  "Maximum loop depth exceeded" },
    [E_MLP] = { "MLP",  "Missing (" },
    [E_MMX] = { "MMX",  "Maximum macro depth exceeded" },
    [E_MNA] = { "MNA",  "Missing n argument" },
    [E_MOD] = { "MOD",  "Invalid command modifier" },
    [E_MQX] = { "MQX",  "Maximum Q-register depth exceeded" },
    [E_MRA] = { "MRA",  "Missing right angle bracket" },
    [E_MRP] = { "MRP",  "Missing )" },
    [E_MSC] = { "MSC",  "Missing start of conditional" },
    [E_NAB] = { "NAB",  "No arg before ^_" },
    [E_NAC] = { "NAC",  "No arg before ," },
    [E_NAE] = { "NAE",  "No arg before =" },
    [E_NAP] = { "NAP",  "No arg before )" },
    [E_NAQ] = { "NAQ",  "No arg before \"" },
    [E_NAS] = { "NAS",  "No arg before ;" },
    [E_NAU] = { "NAU",  "No arg before U" },
    [E_NCA] = { "NCA",  "Negative argument to ," },
    [E_NFI] = { "NFI",  "No file for input" },
    [E_NFO] = { "NFO",  "No file for output" },
    [E_NOW] = { "NOW",  "Window support not enabled" },
    [E_NPA] = { "NPA",  "Negative or 0 argument to P" },
    [E_NRO] = { "NRO",  "No room for output" },
    [E_NTF] = { "NTF",  "No tag found" },
    [E_NYA] = { "NYA",  "Numeric argument with Y" },
    [E_NYI] = { "NYI",  "Not yet implemented" },
    [E_OFO] = { "OFO",  "Output file already open" },
    [E_OUT] = { "OUT",  "Output error" },
    [E_PDO] = { "PDO",  "Push-down list overflow" },
    [E_PES] = { "PES",  "Attempt to pop empty stack" },
    [E_POP] = { "POP",  "Attempt to move pointer off page with '%s'" },
    [E_SNI] = { "SNI",  "; not in iteration" },
    [E_SRH] = { "SRH",  "Search failure '%s'" },
    [E_STL] = { "STL",  "String too long" },
    [E_SYS] = { "SYS",  "%s" },
    [E_TAG] = { "TAG",  "Missing tag !%s!" },
    [E_T10] = { "T10",  "TECO-10 command not implemented" },
    [E_UCD] = { "UCD",  "Unable to close and delete output file '%s'" },
    [E_UCI] = { "UCI",  "Unable to close input file" },
    [E_UCO] = { "UCO",  "Unable to close output file" },
    [E_UFI] = { "UFI",  "Unable to open file '%s' for input" },
    [E_UFO] = { "UFO",  "Unable to open file '%s' for output" },
    [E_UMA] = { "UMA",  "Unused m argument" },
    [E_UNA] = { "UNA",  "Unused n argument" },
    [E_URC] = { "URC",  "Unable to read character from terminal" },
    [E_URE] = { "URE",  "Unable to read TECO command file" },
    [E_URL] = { "URL",  "Unable to read line from input file" },
    [E_UWL] = { "UWL",  "Unable to write line to output file" },
    [E_UTC] = { "UTC",  "Unterminated command string" },
    [E_UTL] = { "UTL",  "Unterminated loop" },
    [E_UTM] = { "UTM",  "Unterminated macro" },
    [E_UTQ] = { "UTQ",  "Unterminated quote" },
    [E_WIN] = { "WIN",  "Window initialization error" },
    [E_XAB] = { "XAB",  "Execution aborted" },
    [E_YCA] = { "YCA",  "Y command aborted" },
};

///  @var    verbose
///  @brief  Verbose descriptions of TECO error messages.

static const char *verbose[] =
{
    [E_ARG] = "Three arguments are given (a,b,c or H,c).",

    [E_BNI] = "There is a close angle bracket not matched by an open "
              "angle bracket somewhere to its left. (Note: an "
              "iteration in a macro stored in a Q-register must be "
              "complete within the Q-register.)",

    [E_BRC] = "There is a close square bracket not matched by an open "
              "square bracket somewhere to its left.",

    [E_CHR] = "A non-ASCII character preceded an EE command.",

    [E_CON] = "Conditionals, parenthesized arguments, and iterations must "
              "be properly nested. The user probably used some construct "
              "like: N\"E...(...' where an iteration or parenthesized "
              "argument is begin in a conditional but not terminated in "
              "the same conditional.",

    [E_CPQ] = "A ] command has been executed and there is nothing saved on "
              "the Q-register push down list.",

    [E_DEV] = "A file specification string in an E command contains an "
              "unknown device name.",

    [E_DIV] = "An attempt was made to divide a number by zero.",

    [E_DTB] = "An nD command has been attempted which is not contained "
              "within the current page.",

    [E_DUP] = "An O command found a duplicate tag within the command string.",

    [E_EGC] = "An EG command was longer than 200 characters.",

    [E_FER] = "The file specified in an ER, EW, or EB command was not found.",

    [E_FNF] = "The requested input file could not be located. If this "
              "occurred within a macro, the colon modified ER or EB "
              "command may be necessary.",

    [E_ICE] = "A search argument contains a ^E command that is either "
              "not defined or incomplete. The only valid ^E commands "
              "in search arguments are: ^EA, ^ED, ^EV, ^EW, ^EL, ^ES, "
              "^E<nnn> and ^E[a,b,c...]",

    [E_IEC] = "An invalid E command has been executed. The E character "
              "must be followed by an alphabetic to form a legal E "
              "command (i.e. ER or EX).",

    [E_IFC] = "An illegal F command has been executed.",

    [E_IFE] = "The numeric expression preceding a command doesn't make "
              "sense. For example, 5+ isn't a complete expression.",

    [E_IFN] = NULL,

    [E_IIA] = "A command of the form \"nItext$\" was attempted. This "
              "combination of character and text insertion is illegal.",

    [E_ILL] = "An attempt has been made to execute an invalid TECO "
              "command.",

    [E_ILN] = "An 8 or 9 has been entered when the radix of TECO is set "
              "to octal.",

    [E_INI] = "A fatal error occurred during TECO initialization.",

    [E_INP] = NULL,

    [E_IPA] = "The argument preceding a P or PW command is negative or 0.",

    [E_IQC] = "One of the valid \" commands did not follow the \". Refer "
              "to Section 5.14 (conditional execution commands) for "
              "the legal set of commands.",

    [E_IQN] = "An illegal Q-register name was specified in one of the "
              "Q-register commands.",

    [E_IRA] = "The argument to a ^R radix command must be 8, 10 or 16.",

    [E_ISA] = "The argument preceding a search command is 0. This "
              "argument must not be 0.",

    [E_ISS] = "One of the search string special characters (^Q, ^V, ^W, "
              "etc.) would have modified the search string delimiter "
              "(usually ESCAPE).",

    [E_IUC] = "The character following a ^ must have an ASCII value "
              "between 100 and 137 inclusive or between 141 and 172 "
              "inclusive.",

    [E_MAP] = "Every conditional (opened with the \" command) must be "
              "closed with the ' command.",

    [E_MEM] = "Insufficient memory available to complete the current "
              "command. Make sure the Q-register area does not contain "
              "much unnecessary text. Breaking up the text area into "
              "multiple pages might be useful.",

    [E_MLA] = "There is a right angle bracket that has no matching left "
              "angle bracket. An iteration must be complete within the "
              "macro or command.",

    [E_MLP] = "There is a right parenthesis trhat is not matched by a "
              "corresponding left parenthesis.",

    [E_MNA] = "An m argument was not followed by an n argument.",

    [E_MOD] = "A modifier (:, ::, or @) was specified that was invalid "
              "for a command, occurred in the middle of an expression, "
              "or duplicated another modifier.",

    [E_MRA] = "There is a left angle bracket that has no matching right "
              "angle bracket. An iteration must be complete within the "
              "macro or command.",

    [E_MRP] = "There is a left parenthesis that is not matched by a "
              "corresponding right parenthesis.",

    [E_MSC] = "A ' command (end of conditional) was encountered. Every "
              "command must be matched by a preceding \" (start of "
              "conditional) command.",

    [E_NAB] = "The ^_ command must be preceded by either a specific "
              "numeric argument or a command that returns a numeric "
              "value.",

    [E_NAC] = "A command has been executed in which a , is not preceded "
              "by a numeric argument.",

    [E_NAE] = "The =, ==, or === command must be preceded by either a "
              "specific numeric argument or a command that returns a "
              "numeric value.",

    [E_NAP] = "A ) parenthesis has been encountered and is not properly "
              "preceded by a specific numeric argument or a command "
              "that returns a numeric value.",

    [E_NAQ] = "The \" commands must be preceded by a single numeric "
              "argument on which the decision to execute the following "
              "commands or skip to the matching ' is based.",

    [E_NAS] = "The , command must be preceded by a single numeric "
              "argument on which the decision to execute the following "
              "commands or skip to the matching > is based.",

    [E_NAU] = "The U comand must be preceded by either a specific "
              "numeric argument or a command that returns a numeric "
              "value.",

    [E_NCA] = "A comma was preceded by a negative number.",

    [E_NFI] = "Before issuing an input command, such as Y, it is "
              "necessary to open an input file by use of a command such "
              "as ER or EB.",

    [E_NFO] = "Before issuing an output command, such as N or P, "
              "it is necessary to open an output file by use of a "
              "command such as EW or EB.",

    [E_NPA] = "A P command was issued with an argument that was zero "
              "or negative.",

    [E_NRO] = NULL,

    [E_NTF] = "The O command did not include a tag, or an nO command "
              "had a value that was zero, negative, or out of range.",

    [E_NYA] = "The Y command must not be preceded by either a numeric "
              "argument or a command that returns a numeric value.",

    [E_NYI] = "A command was issued that is not yet implemented in this "
              "version of TECO.",

    [E_OFO] = "A command has been executed which tried to create an "
              "output file, but an output file currently is open. It "
              "is typically appropriate to use the EC or EK command as "
              "the situation calls for to close the output file.",

    [E_PDO] = "The command string has become too complex. Simplify it.",

    [E_PES] = "A ] command (pop off q-register stack into a q-register) "
              "was encountered when there was nothing on the q-register "
              "stack.",

    [E_POP] = "A J, C or R command has been executed which attempted to "
              "move the pointer off the page. The result of executing "
              "one of these commands must leave the pointer between 0 "
              "and Z, inclusive. The characters referenced by a D or "
              "nA command must also be within the buffer boundary.",

    [E_SNI] = "A , command has been executed outside of an open "
              "iteration bracket. This command may only be executed "
              "within iteration brackets.",

    [E_SRH] = "A search command not preceded by a colon modifier and "
              "not within an iteration has failed to find the specified "
              "\"text\". After an S search fails the pointer is left at "
              "the beginning of the buffer. After an N or _ search "
              "fails the last page of the input file has been input "
              "and, in the case of N, output, and the buffer is "
              "cleared. In the case of an N search it is usually "
              "necessary to close the output file and reopen it for "
              "continued editing.",

    [E_STL] = "A search or file name string is too long. This is most "
              "likely the result of a missing ESCAPE after the string.",

    [E_SYS] = "A system call failed. The error message text explains the error.",

    [E_TAG] = "The tag !tag! specified by an O command cannot be "
              "found. This tag must be in the same macro level as the "
              "O command referencing it.",

    [E_T10] = "A TECO-10 command cannot be or has not been implemented.",

    [E_UMA] = "An m argument was provided to a command which does not allow it.",

    [E_UNA] = "An n argument was provided to a command which does not allow it.",

    [E_UTC] = "This is a general error which is usually caused by an "
              "unterminated insert, search, or filename argument, an "
              "unterminated ^A message, an unterminated tag or comment "
              "(i.e., unterminated ! construct), or a missing ' "
              "character which closes a conditional execution command.",

    [E_UTL] = "A loop was not properly terminated inside a conditional.",

    [E_UTM] = "This error is that same as the ?UTC error except that the "
              "unterminated command was executing from a Q-register "
              "(i.e., it was a macro). (Note: An entire command "
              "sequence stored in a q-register must be complete within "
              "the Q-register.)",

    [E_UTQ] = "A conditional was not properly terminated inside a loop.",

    [E_WIN] = "Window error occurred. More information is TBD.",

    [E_XAB] = "Execution of TECO was aborted. This is usually due to the "
              "typing of <CTRL/C>.",

    [E_YCA] = "An attempt has been made to execute a Y or _ search "
              "command with an output file open, that would cause text "
              "in the text buffer to be erased without outputting it to "
              "the output file. The ED command controls this check.",
};


int last_error = E_NUL;                 ///< Last error encountered

static char err_buf[ERR_BUF_SIZE];      ///< Buffer for error argument

static const char *err_str = err_buf;   ///< Error message argument


///
///  @brief    Print verbose error message.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void help_err(int err_teco)
{
    if (err_teco <= 0 || (uint)err_teco > countof(verbose))
    {
        return;
    }

    const char *text = verbose[err_teco];

    if (text == NULL)
    {
        return;
    }

    const char *start = text;
    const char *end = start;
    uint pos = 0;
    uint maxlines = 20;

    while (maxlines > 0)
    {
        const char *next = strchr(end + pos, ' ');
        uint len;

        if (next != NULL)
        {
            len = (uint)(next - start);

            if (len > (uint)w.width)
            {
                print_str("%.s", (end - start) - 1, start);
                print_chr(CRLF);

                --maxlines;

                start = end;
                pos = 0;
            }
            else
            {
                end = next + 1;
            }
        }
        else
        {
            len = (uint)strlen(start);

            print_str("%.*s", (int)len, start);
            print_chr(CRLF);

            break;
        }
    }
}


///
///  @brief    Print error message and return to main().
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

noreturn void print_err(int err_teco)
{
    // If CTRL/C and we're not executing a command, don't print error.

    if (err_teco != E_XAB || f.e0.exec)
    {
        if (err_teco == E_SYS)
        {
            f.eh.verbose = 2;           // Force detail for system errors

            err_str = strerror(errno);  // Convert errno to string
        }
        else if ((uint)err_teco > countof(err_table))
        {
            err_teco = E_NUL;
        }

        const char *code = err_table[err_teco].code;
        const char *text = err_table[err_teco].text;

        print_str("?%s", code);         // Always print code

        if (f.eh.verbose != 1)          // Need to print more?
        {
            print_str("   ");
            print_str(text, err_str);
        }

        print_chr(CRLF);

        last_error = err_teco;
    }

    // Done printing error, now how are we supposed to get out of here?

    reset_if();                         // Reset conditional stack
    reset_loop();                       // Reset loop stack

    if (f.et.abort || !main_active)     // Abort on error, or no main loop?
    {
        exit(EXIT_FAILURE);             // Yes, we're all done here
    }

    longjmp(jump_main, 2);              // Back to the shadows again!
}


///
///  @brief    Print error and jump back to main loop.
///
///  @returns  N/A
///
////////////////////////////////////////////////////////////////////////////////

noreturn void printc_err(int err_teco, int c)
{
    if (isprint(c))
    {
        uint nbytes = (uint)snprintf(err_buf, sizeof(err_buf), "%c", c);

        assert(nbytes < sizeof(err_buf));
    }
    else if (!isascii(c))               // 8-bit character?
    {
        uint nbytes = (uint)snprintf(err_buf, sizeof(err_buf), "[%02x]", c);

        assert(nbytes < sizeof(err_buf));
    }
    else                                // Must be a control character
    {
        uint nbytes = (uint)snprintf(err_buf, sizeof(err_buf), "^%c", c + 'A' - 1);

        assert(nbytes < sizeof(err_buf));
    }

    err_str = err_buf;

    print_err(err_teco);                // Print error and return to main()
}


///
///  @brief    Print error message with string argument and return to main().
///
///  @returns  N/A
///
////////////////////////////////////////////////////////////////////////////////

noreturn void prints_err(int err_teco, const char *str)
{
    err_str = str;

    print_err(err_teco);                // Print error and return to main()
}
