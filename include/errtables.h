///
///  @file    errtables.h
///  @brief   TECO error tables
///
///  *** Automatically generated from template file. DO NOT MODIFY. ***
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

///  @struct  errlist
///  @brief   Structure of error message

struct errlist
{
    const char *code;           ///< Three-letter error code
    const char *text;           ///< Short description of error
};

///  @struct  errlist
///  @var     errlist
///
///  @brief   TECO error messages

static struct errlist errlist[] =
{
    [E_ARG] = { "ARG",  "Improper arguments" },
    [E_ATS] = { "ATS",  "Invalid or extraneous at-sign" },
    [E_BAT] = { "BAT",  "Bad tag '!%s!'" },
    [E_BNI] = { "BNI",  "Right angle bracket not in iteration" },
    [E_CHR] = { "CHR",  "Invalid character for command" },
    [E_CMD] = { "CMD",  "An EG or EZ command is too long" },
    [E_COL] = { "COL",  "Invalid or extraneous colon" },
    [E_CPQ] = { "CPQ",  "Can't pop into Q-register" },
    [E_DET] = { "DET",  "%s" },
    [E_DIV] = { "DIV",  "Division by zero" },
    [E_DPY] = { "DPY",  "Display mode error" },
    [E_DTB] = { "DTB",  "Delete too big" },
    [E_DUP] = { "DUP",  "Duplicate tag '!%s!'" },
    [E_ERR] = { "ERR",  "%s" },
    [E_EXT] = { "EXT",  "Extended feature not enabled" },
    [E_FIL] = { "FIL",  "Invalid file '%s'" },
    [E_FNF] = { "FNF",  "File not found '%s'" },
    [E_IAA] = { "IAA",  "Invalid A argument" },
    [E_ICE] = { "ICE",  "Invalid ^E command in search argument" },
    [E_IEC] = { "IEC",  "Invalid character '%s' after E" },
    [E_IFC] = { "IFC",  "Invalid character '%s' after F" },
    [E_IFE] = { "IFE",  "Ill-formed numeric expression" },
    [E_IFN] = { "IFN",  "Invalid character '%s' in filename" },
    [E_IIA] = { "IIA",  "Invalid insert argument" },
    [E_ILL] = { "ILL",  "Invalid command '%s'" },
    [E_ILN] = { "ILN",  "Invalid number" },
    [E_IMA] = { "IMA",  "Invalid m argument" },
    [E_INA] = { "INA",  "Invalid n argument" },
    [E_IPA] = { "IPA",  "Invalid P argument" },
    [E_IQC] = { "IQC",  "Invalid quote character" },
    [E_IQN] = { "IQN",  "Invalid Q-register name '%s'" },
    [E_IRA] = { "IRA",  "Invalid radix argument to ^R" },
    [E_ISA] = { "ISA",  "Invalid search argument" },
    [E_ISS] = { "ISS",  "Invalid search string" },
    [E_IUC] = { "IUC",  "Invalid character '%s' following ^" },
    [E_KEY] = { "KEY",  "Keyword '%s' not found" },
    [E_LOC] = { "LOC",  "Invalid location for tag '!%s!'" },
    [E_MAP] = { "MAP",  "Missing apostrophe" },
    [E_MAX] = { "MAX",  "Internal program limit reached" },
    [E_MEM] = { "MEM",  "Memory overflow" },
    [E_MLP] = { "MLP",  "Missing left parenthesis" },
    [E_MQN] = { "MQN",  "Missing Q-register name" },
    [E_MRA] = { "MRA",  "Missing right angle bracket" },
    [E_MRP] = { "MRP",  "Missing right parenthesis" },
    [E_MSC] = { "MSC",  "Missing start of conditional" },
    [E_NAB] = { "NAB",  "No argument before 1's complement operator" },
    [E_NAC] = { "NAC",  "No argument before comma" },
    [E_NAE] = { "NAE",  "No argument before equals sign" },
    [E_NAP] = { "NAP",  "No argument before right parenthesis" },
    [E_NAQ] = { "NAQ",  "No argument before quote" },
    [E_NAS] = { "NAS",  "No argument before semi-colon" },
    [E_NAT] = { "NAT",  "Cannot have n argument and text string" },
    [E_NAU] = { "NAU",  "No argument before U command" },
    [E_NCA] = { "NCA",  "Negative argument to comma" },
    [E_NFI] = { "NFI",  "No file for input" },
    [E_NFO] = { "NFO",  "No file for output" },
    [E_NON] = { "NON",  "No n argument after m argument" },
    [E_NOT] = { "NOT",  "O command has no tag" },
    [E_NPA] = { "NPA",  "Negative or 0 argument to P" },
    [E_NYA] = { "NYA",  "Numeric argument with Y" },
    [E_NYI] = { "NYI",  "Not yet implemented" },
    [E_OFO] = { "OFO",  "Output file already open" },
    [E_PDO] = { "PDO",  "Push-down list overflow" },
    [E_POP] = { "POP",  "Attempt to move pointer off page with '%s'" },
    [E_SNI] = { "SNI",  "Semi-colon not in iteration" },
    [E_SRH] = { "SRH",  "Search failure: '%s'" },
    [E_TAG] = { "TAG",  "Missing tag: '!%s!'" },
    [E_TXT] = { "TXT",  "Invalid text delimiter '%s'" },
    [E_UTC] = { "UTC",  "Unterminated command string" },
    [E_UTM] = { "UTM",  "Unterminated macro" },
    [E_XAB] = { "XAB",  "Execution aborted" },
    [E_YCA] = { "YCA",  "Y command aborted" },

};

///  @var    errhelp
///
///  @brief  Detailed help for TECO errors.

static const char *errhelp[] =
{
    [E_ARG] = "Three or more numeric arguments are given "
              "(e.g., 1,2,3D or H,cT).",
    [E_ATS] = "At sign not allowed for command, or more than "
              "one at sign specified.",
    [E_BAT] = "An O command was specified with an invalid tag. "
              "Tags may only contain graphic ASCII characters "
              "(excluding commas which are reserved for computed "
              "GOTOs). Leading and trailing spaces are ignored, "
              "but embedded spaces are not allowed.",
    [E_BNI] = "A right angle bracket was not matched by a left "
              "angle bracket (Note: a loop in a Q-register "
              "macro must be complete within the macro.)",
    [E_CHR] = "The numeric argument to an EE command was "
              "not an ASCII character.",
    [E_CMD] = "An EG or EZ command was longer than 4095 characters.",
    [E_COL] = "A colon preceded a command that does not allow "
              "colons, or there were too many colons specified "
              "for the command.",
    [E_CPQ] = "A ] command has been executed and there is "
              "nothing saved on the Q-register push down list.",
    [E_DET] = "An error occurred when attempting to detach TECO "
              "from the user's terminal with a 0,64 ET command (or "
              "equivalent command), or if the detach feature is "
              "not supported in the current operating environment.",
    [E_DIV] = "An expression tried to divide a number by zero.",
    [E_DPY] = "Display mode support is either missing or disabled.",
    [E_DTB] = "A D command attempted to delete text outside "
              "the current page.",
    [E_DUP] = "An O command found a duplicate tag within the "
              "command string.",
    [E_ERR] = "A system call failed. The error message text "
              "explains the error.",
    [E_EXT] = "A command attempted to use an extended feature "
              "which is not currently enabled.",
    [E_FIL] = "An attempt was made to open a directory, FIFO, "
              "socket, or similar file specification instead "
              "of a regular file.",
    [E_FNF] = "The requested input file could not be located. "
              "If this occurred within a macro, the "
              "colon-modified command may be necessary.",
    [E_IAA] = "The argument preceding a :A command is "
              "negative or zero.",
    [E_ICE] = "A search argument contains a ^E command that "
              "is either not defined or incomplete.",
    [E_IEC] = "An invalid E command has been executed.",
    [E_IFC] = "An invalid F command has been executed.",
    [E_IFE] = "The numeric expression preceding a command "
              "doesn't make sense. For example, 5+ "
              "isn't a complete expression.",
    [E_IFN] = "A file name contained one or more invalid "
              "characters. Check your operating system "
              "documentation to determine what characters "
              "are valid for file names.",
    [E_IIA] = "A command of the form \"nItext`\" was attempted. "
              "This combination of character and text "
              "insertion is not allowed.",
    [E_ILL] = "An attempt has been made to execute an invalid "
              "TECO command.",
    [E_ILN] = "An 8 or 9 was used in a digit string for an "
              "octal number, or a 0x or 0X prefix was not "
              "followed by a hexadecimal number.",
    [E_IMA] = "An m argument was provided to a command which "
              "does not allow it.",
    [E_INA] = "An n argument was provided to a command which "
              "does not allow it.",
    [E_IPA] = "The argument preceding a P or PW command is "
              "negative or zero.",
    [E_IQC] = "One of the valid \" qualifiers did not "
              "follow the quote character.",
    [E_IQN] = "An invalid Q-register name was specified in one "
              "of the Q-register commands.",
    [E_IRA] = "The argument to a ^R radix command must be 8, 10 "
              "or 16.",
    [E_ISA] = "The argument preceding a search command is 0. "
              "This argument must not be 0.",
    [E_ISS] = "One of the search string special characters "
              "(^Q, ^V, ^W, etc.) would have modified the "
              "search string delimiter (usually <ESC>).",
    [E_IUC] = "The character following a ^ must have an ASCII "
              "value between 64 and 95 inclusive, or between "
              "141 and 172 inclusive.",
    [E_KEY] = "An invalid keyword was specified for an F1, F2, "
              "F3, F4, FM, or FQ command.",
    [E_LOC] = "An O command cannot jump to a location inside a "
              "loop, other than the one the O command may be in, "
              "or inside a conditional.",
    [E_MAP] = "Every conditional (started with the \" "
              "command) must be closed with the ' command.",
    [E_MAX] = "Loops and conditionals are limited to a maximum "
              "of 32 levels, macros are limited to a depth of 64 "
              "levels, and file names are limited to 4095 characters.",
    [E_MEM] = "Insufficient memory available to complete the "
              "current command.",
    [E_MLP] = "There is a right parenthesis trhat is not matched "
              "by a corresponding left parenthesis.",
    [E_MQN] = "A command or match control construct did not "
              "include a required Q-register name.",
    [E_MRA] = "There is a left angle bracket that has no matching "
              "right angle bracket. A loop must be complete "
              "within a macro or command string.",
    [E_MRP] = "There is a left parenthesis that is not matched by "
              "a corresponding right parenthesis.",
    [E_MSC] = "A ' command (end of conditional) was "
              "encountered. Every ' command must be "
              "matched by a preceding \" (start of a "
              "conditional) command.",
    [E_NAB] = "The ^_ command must be preceded by either a "
              "specific numeric argument or a command that "
              "returns a numeric value.",
    [E_NAC] = "A command has been executed in which a , is "
              "not preceded by a numeric argument.",
    [E_NAE] = "The =, ==, or === command must be preceded by "
              "either a specific numeric argument or a command "
              "that returns a numeric value.",
    [E_NAP] = "A right parenthesis has been encountered and "
              "is not properly preceded by a specific numeric "
              "argument or a command that returns a numeric value.",
    [E_NAQ] = "The \" commands must be preceded by a single "
              "numeric argument on which the decision to "
              "execute the following commands or skip to "
              "the matching ' is based.",
    [E_NAS] = "The ; command must be preceded by a single "
              "numeric argument on which the decision to "
              "execute the following commands or skip to "
              "the matching > is based.",
    [E_NAT] = "Both an n argument and a string were specified for "
              "a command which does not allow more than one.",
    [E_NAU] = "The U comand must be preceded by either a specific "
              "numeric argument or a command that returns a "
              "numeric value.",
    [E_NCA] = "A comma was preceded by a negative number.",
    [E_NFI] = "Before issuing an input command, such as Y, it "
              "is necessary to open an input file by use of a "
              "command such as ER or EB.",
    [E_NFO] = "Before issuing an output command, such as N or "
              "or P, it is necessary to open an output file with "
              "a command such as EW or EB.",
    [E_NON] = "An m argument was not followed by an n argument.",
    [E_NOT] = "No tag was found for an O command.",
    [E_NPA] = "A P command was preceded by a negative or 0 argument.",
    [E_NYA] = "The Y command must not be preceded by either a "
              "numeric argument or a command that returns a "
              "numeric value.",
    [E_NYI] = "A command was issued that is not yet implemented "
              "in this version of TECO.",
    [E_OFO] = "A command has been executed which tried to "
              "create an output file, but an output file "
              "currently is open. It is typically appropriate "
              "to use the EC or EK command as the situation "
              "calls for to close the output file.",
    [E_PDO] = "The command string has become too complex. "
              "Simplify it.",
    [E_POP] = "A J, C or R command has been executed which "
              "attempted to move the pointer off the page. "
              "The result of executing one of these commands "
              "must leave the pointer between 0 and Z, "
              "The characters referenced by a D or nA "
              "command must also be within the buffer limits.",
    [E_SNI] = "A ; command has been executed outside of a "
              "loop.",
    [E_SRH] = "A search command not preceded by a colon "
              "modifier and not within an iteration has "
              "failed to find the specified \" command. "
              "After an S search fails the pointer is "
              "left at the beginning of the buffer. "
              "After an N or _ search fails the last "
              "page of the input file has been input "
              "and, in the case of N, output, and the "
              "buffer is cleared. In the case of an N "
              "search it is usually necessary to close "
              "the output file and reopen it.",
    [E_TAG] = "The tag specified by an O command cannot "
              "be found. This tag must be in the same macro "
              "level as the O command referencing it.",
    [E_TXT] = "Text delimiters must be graphic ASCII characters "
              "in the range of [33,126], or control characters "
              "in the range of [1,26]. Characters such as spaces "
              "or ESCapes may not be used for delimiters.",
    [E_UTC] = "This is a general error which is usually "
              "caused by an unterminated insert, search, "
              "or filename argument, an unterminated ^A "
              "message, an unterminated tag or comment "
              "(i.e., unterminated ! construct), or a "
              "missing ' character which closes a "
              "conditional execution command.",
    [E_UTM] = "This error is that same as the ?UTC error "
              "except that the unterminated command was "
              "executing from a Q-register (i.e., it was "
              "a macro). (Note: An entire command sequence "
              "stored in a Q-register must be complete within "
              "the Q-register.)",
    [E_XAB] = "Execution of TECO was aborted. This is usually "
              "due to the typing of <CTRL/C>.",
    [E_YCA] = "An attempt has been made to execute a Y "
              "or _ search command with an output file "
              "open, that would cause text in the edit "
              "buffer to be erased without outputting it "
              "to the output file. The ED command controls "
              "this check.",

};
