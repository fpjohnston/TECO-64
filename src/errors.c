///
///  @file    errors.c
///  @brief   TECO error messages and error functions.
///
///  @bug     No known bugs.
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

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "cmd.h"
#include "eflags.h"
#include "errors.h"
#include "term.h"
#include "window.h"


#define ERR_BUF_SIZE    64          ///< Size of error buffer

#define MAX_LINES       20          ///< Max. lines in error message

int last_error = E_NUL;             ///< Last error encountered

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
    [E_BOA] = { "BOA",  "O argument is out of range" },
    [E_CHR] = { "CHR",  "Invalid character for command" },
    [E_DIV] = { "DIV",  "Division by zero" },
    [E_DTB] = { "DTB",  "Delete too big" },
    [E_DUP] = { "DUP",  "Duplicate tag '!%s!'" },
    [E_EGC] = { "EGC",  "EG command is too long" },
    [E_FIL] = { "FIL",  "Illegal file '%s'" },
    [E_FNF] = { "FNF",  "File not found '%s'" },
    [E_ICE] = { "ICE",  "Illegal ^E command in search argument" },
    [E_IEC] = { "IEC",  "Illegal character '%s' after E" },
    [E_IFC] = { "IFC",  "Illegal character '%s' after F" },
    [E_IFE] = { "IFE",  "Ill-formed numeric expression" },
    [E_IFN] = { "IFN",  "Illegal character '%s' in filename" },
    [E_IIA] = { "IIA",  "Illegal insert arg" },
    [E_ILL] = { "ILL",  "Illegal command '%s'" },
    [E_ILN] = { "ILN",  "Illegal number" },
    [E_IMA] = { "IMA",  "Illegal m argument" },
    [E_INI] = { "INI",  "%s" },
    [E_IQC] = { "IQC",  "Illegal quote character" },
    [E_IQN] = { "IQN",  "Illegal Q-register name '%s'" },
    [E_IRA] = { "IRA",  "Illegal radix argument to ^R" },
    [E_ISA] = { "ISA",  "Illegal search argument" },
    [E_ISS] = { "ISS",  "Illegal search string" },
    [E_IUC] = { "IUC",  "Illegal character '%s' following ^" },
    [E_MAT] = { "MAT",  "No match for file specification" },
    [E_MEM] = { "MEM",  "Memory overflow" },
    [E_MLA] = { "MLA",  "Missing left angle bracket" },
    [E_MLP] = { "MLP",  "Missing left parenthesis" },
    [E_MOD] = { "MOD",  "Invalid command modifier" },
    [E_MRP] = { "MRP",  "Missing right parenthesis" },
    [E_MSC] = { "MSC",  "Missing start of conditional" },
    [E_NAB] = { "NAB",  "No argument before 1's complement operator" },
    [E_NAC] = { "NAC",  "No argument before comma" },
    [E_NAE] = { "NAE",  "No argument before equals sign" },
    [E_NAP] = { "NAP",  "No argument before right parenthesis" },
    [E_NAQ] = { "NAQ",  "No argument before quote" },
    [E_NAS] = { "NAS",  "No argument before semi-colon" },
    [E_NAU] = { "NAU",  "No argument before U command" },
    [E_NCA] = { "NCA",  "Negative argument to comma" },
    [E_NFI] = { "NFI",  "No file for input" },
    [E_NFO] = { "NFO",  "No file for output" },
    [E_NOA] = { "NOA",  "O argument is non-positive" },
    [E_NON] = { "NON",  "No n argument after m argument" },
    [E_NOT] = { "NOT",  "O command has no tag" },
    [E_NOW] = { "NOW",  "Window support not enabled" },
    [E_NPA] = { "NPA",  "P or PW argument is negative" },
    [E_NYA] = { "NYA",  "Numeric argument with Y" },
    [E_NYI] = { "NYI",  "Not yet implemented" },
    [E_OFO] = { "OFO",  "Output file already open" },
    [E_PDO] = { "PDO",  "Push-down list overflow" },
    [E_PES] = { "PES",  "Can't pop from empty push-down stack" },
    [E_POP] = { "POP",  "Attempt to move pointer off page with '%s'" },
    [E_SNI] = { "SNI",  "Semi-colon not in iteration" },
    [E_SRH] = { "SRH",  "Search failure '%s'" },
    [E_SYS] = { "SYS",  "%s" },
    [E_TAG] = { "TAG",  "Missing tag '!%s!'" },
    [E_UTC] = { "UTC",  "Unterminated command string" },
    [E_UTL] = { "UTL",  "Unterminated loop" },
    [E_UTM] = { "UTM",  "Unterminated macro" },
    [E_UTQ] = { "UTQ",  "Unterminated quote" },
    [E_WIN] = { "WIN",  "Window initialization error" },
    [E_XAB] = { "XAB",  "Execution aborted" },
    [E_YCA] = { "YCA",  "Y command aborted" },
    [E_ZPA] = { "ZPA",  "P or PW argument is zero" },
};

///  @var    verbose
///
///  @brief  Verbose descriptions of TECO error messages.

static const char *verbose[] =
{
    [E_ARG] = "Three arguments are given (a,b,c or H,c).",

    [E_BOA] = "The argument for an O command was out of range",

    [E_CHR] = "A non-ASCII character preceded an EE command.",

    [E_DIV] = "An attempt was made to divide a number by zero.",

    [E_DTB] = "An nD command has been attempted which is not contained "
              "within the current page.",

    [E_DUP] = "An O command found a duplicate tag within the command string.",

    [E_EGC] = "An EG command was longer than 200 characters.",

    [E_FIL] = "An attempt was made to open a directory, FIFO, socket, or "
              "similar file specification instead of a regular file. ",

    [E_FNF] = "The requested input file could not be located. If this occurred "
              "within a macro, the colon-modified command may be necessary.",

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

    [E_IIA] = "A command of the form \"nItext$\" was attempted. This "
              "combination of character and text insertion is illegal.",

    [E_ILL] = "An attempt has been made to execute an invalid TECO "
              "command.",

    [E_ILN] = "An 8 or 9 has been entered when the radix of TECO is set "
              "to octal.",

    [E_IMA] = "An m argument was provided to a command which does not allow it.",

    [E_INI] = "A fatal error occurred during TECO initialization.",

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

    [E_MAT] = "No match was found for the file specification for an EN "
              "command.",

    [E_MEM] = "Insufficient memory available to complete the current "
              "command. Make sure the Q-register area does not contain "
              "much unnecessary text. Breaking up the text area into "
              "multiple pages might be useful.",

    [E_MLA] = "There is a right angle bracket that has no matching left "
              "angle bracket. An iteration must be complete within the "
              "macro or command.",

    [E_MLP] = "There is a right parenthesis trhat is not matched by a "
              "corresponding left parenthesis.",

    [E_MOD] = "A modifier (:, ::, or @) was specified that was invalid "
              "for a command, occurred in the middle of an expression, "
              "or duplicated another modifier.",

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

    [E_NOA] = "The argument for an O command was <= 0.",

    [E_NON] = "An m argument was not followed by an n argument.",

    [E_NOT] = "No tag was found for an O command.",

    [E_NPA] = "The argument preceding a P or PW command is negative.",

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

    [E_SYS] = "A system call failed. The error message text explains the error.",

    [E_TAG] = "The tag !tag! specified by an O command cannot be "
              "found. This tag must be in the same macro level as the "
              "O command referencing it.",

    [E_UTC] = "This is a general error which is usually caused by an "
              "unterminated insert, search, or filename argument, an "
              "unterminated ^A message, an unterminated tag or comment "
              "(i.e., unterminated ! construct), or a missing ' "
              "character which closes a conditional execution command.",

    [E_UTL] = "There is a right angle bracket that has no matching left "
              "angle bracket. An iteration must be complete within the "
              "macro or command.",

    [E_UTM] = "This error is that same as the ?UTC error except that the "
              "unterminated command was executing from a Q-register "
              "(i.e., it was a macro). (Note: An entire command "
              "sequence stored in a q-register must be complete within "
              "the Q-register.)",

    [E_UTQ] = "Every conditional started with the \" command must be "
              "terminated with the ' command.",

    [E_WIN] = "Window error occurred. More information is TBD.",

    [E_XAB] = "Execution of TECO was aborted. This is usually due to the "
              "typing of <CTRL/C>.",

    [E_YCA] = "An attempt has been made to execute a Y or _ search "
              "command with an output file open, that would cause text "
              "in the text buffer to be erased without outputting it to "
              "the output file. The ED command controls this check.",

    [E_ZPA] = "The argument preceding a P or PW command is zero.",
};


///
///  @brief    Print verbose error message.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_help(int error)
{
    if (error <= 0 || (uint)error > countof(verbose))
    {
        return;
    }

    const char *text = verbose[error];

    if (text == NULL)
    {
        return;
    }

    const char *start = text;
    const char *end = start;
    uint pos = 0;
    uint maxlines = MAX_LINES;

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
///  @brief    TECO exception handler. Can be called for one of the following
///            conditions:
///
///            1. A bad command (e.g., an invalid Q-register name, or more than
///               two numeric arguments).
///
///            2. A command which could not be successfully executed (e.g.,
///               search string not found, or requested input file with no read
///               permissions).
///
///            3. An unexpected event (e.g., out of memory).
///
///            Note that this function is not used for processor errors such as
///            dereferencing an invalid pointer. 
///
///            The specific error code is used to determine what arguments (if
///            any) have also been passed by the caller. There are four cases
///            possible:
///
///            1. Errors that include a single character (e.g., E_POP) that is
///               interpolated in the error message. If the character is a 7-bit
///               printable character, it is output as is; if it is an 8-bit
///               character, it is output as [xx]; if it is a control character,
///               it is output as ^x.
///
///            3. The E_SYS error code, which includes a string that contains
///               the name of a file (or which is NULL) that is appended after
///               the error message.
///
///            3. Other error codes that include a string that is interpolated
///               in the error message.
///
///            4. Error codes that do not require any special processing.
///
///            In addition to those four cases, E_XAB is treated specially if
///            a command is not current being executed, and just causes a return
///            to main program level without any message being printed.
///
///  @returns  n/a (longjmp back to main program).
///
////////////////////////////////////////////////////////////////////////////////

noreturn void throw(int error, ...)
{
    char err_buf[ERR_BUF_SIZE];
    const char *file_str = NULL;
    const char *err_str;
    int c;
    int nbytes;

    va_list args;

    va_start(args, error);

    switch (error)
    {
        case E_IEC:
        case E_IFC:
        case E_IFN:
        case E_ILL:
        case E_IQN:
        case E_IUC:
        case E_POP:
            c = va_arg(args, int);

            if (c >= DEL)               // DEL or 8-bit character?
            {
                nbytes = snprintf(err_buf, sizeof(err_buf), "[%02x]", c);
            }
            else if (isprint(c))        // Printable character?
            {
                nbytes = snprintf(err_buf, sizeof(err_buf), "%c", c);
            }
            else                        // Must be a control character
            {
                nbytes = snprintf(err_buf, sizeof(err_buf), "^%c", c + 'A' - 1);
            }

            assert((uint)nbytes < sizeof(err_buf)); // Error if snprintf() failed

            err_str = err_buf;

            break;

        case E_SYS:
            err_str = strerror(errno);  // Convert errno to string
            file_str = va_arg(args, const char *);

            break;

        case E_DUP:
        case E_FIL:
        case E_FNF:
        case E_INI:
        case E_SRH:
        case E_TAG:
            err_str = va_arg(args, const char *);

            break;

        default:
            if ((uint)error > countof(err_table))
            {
                error = E_NUL;
            }

            err_str = NULL;

            break;
    }

    va_end(args);

    const char *code = err_table[error].code;
    const char *text = err_table[error].text;

    // Ensure that '?' command only prints command string up to error.

    term_block->len = term_block->pos = cbuf->pos;

    // If CTRL/C and we're not executing a command, don't print error.

    if (error != E_XAB || f.e0.exec)
    {
        print_str("?%s", code);         // Always print code

        if (f.eh.verbose != 1)          // Need to print more?
        {
            print_str("   ");
            print_str(text, err_str);

            if (error == E_SYS && file_str != NULL)
            {
                print_str(" for '%s'", file_str);
            }
        }
        
        print_chr(CRLF);

        last_error = error;
    }

    // We reset here rather than in the main loop, since we might exit
    // rather than return there.

    reset();                            // Reset for next command

    if (f.et.abort)                     // Abort on error?
    {
        exit(EXIT_FAILURE);             // Yes, we're all done here
    }

    longjmp(jump_main, 2);              // Back to the shadows again!
}
