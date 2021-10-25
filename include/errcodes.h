///
///  @file    errcodes.h
///  @brief   TECO error codes
///
///  *** Automatically generated from template file. DO NOT MODIFY. ***
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

#if     !defined(_ERRCODES_H)

#define _ERRCODES_H

extern int last_error;

///  @enum   errcodes
///  @brief  Definitions of TECO error messages.

enum errcodes
{
    E_NUL,          ///< No error
    E_ARG,          ///< Improper arguments
    E_ATS,          ///< Invalid or extraneous at-sign
    E_BAT,          ///< Bad tag '!foo!'
    E_BNI,          ///< Right angle bracket not in iteration
    E_CFG,          ///< Command not configured
    E_CHR,          ///< Invalid character for command
    E_CMD,          ///< An EG or EZ command is too long
    E_COL,          ///< Invalid or extraneous colon
    E_CPQ,          ///< Can't pop into Q-register
    E_DIV,          ///< Division by zero
    E_DPY,          ///< Display mode error
    E_DTB,          ///< Delete too big
    E_DUP,          ///< Duplicate tag '!foo!'
    E_ERR,          ///< (error message)
    E_EXT,          ///< Extended feature not enabled
    E_FIL,          ///< Invalid file 'foo'
    E_FNF,          ///< File not found 'foo'
    E_IAA,          ///< Invalid A argument
    E_ICE,          ///< Invalid ^E command in search argument
    E_IE1,          ///< Invalid E1 command
    E_IEC,          ///< Invalid character 'x' after E
    E_IFC,          ///< Invalid character 'x' after F
    E_IFE,          ///< Ill-formed numeric expression
    E_IFN,          ///< Invalid character 'x' in filename
    E_IIA,          ///< Invalid insert argument
    E_ILL,          ///< Invalid command 'x'
    E_ILN,          ///< Invalid number
    E_IMA,          ///< Invalid m argument
    E_INA,          ///< Invalid n argument
    E_IPA,          ///< Invalid P argument
    E_IQC,          ///< Invalid quote character
    E_IQN,          ///< Invalid Q-register name 'x'
    E_IRA,          ///< Invalid radix argument to ^R
    E_ISA,          ///< Invalid search argument
    E_ISS,          ///< Invalid search string
    E_IUC,          ///< Invalid character 'x' following ^
    E_KEY,          ///< Key 'foo' not found
    E_LOC,          ///< Invalid location for tag '!foo!'
    E_MAP,          ///< Missing apostrophe
    E_MAX,          ///< Internal program limit reached
    E_MEM,          ///< Memory overflow
    E_MLP,          ///< Missing left parenthesis
    E_MQN,          ///< Missing Q-register name
    E_MRA,          ///< Missing right angle bracket
    E_MRP,          ///< Missing right parenthesis
    E_MSC,          ///< Missing start of conditional
    E_NAB,          ///< No argument before 1's complement operator
    E_NAC,          ///< No argument before comma
    E_NAE,          ///< No argument before equals sign
    E_NAP,          ///< No argument before right parenthesis
    E_NAQ,          ///< No argument before quote
    E_NAS,          ///< No argument before semi-colon
    E_NAT,          ///< Cannot have n argument and text string
    E_NAU,          ///< No argument before U command
    E_NCA,          ///< Negative argument to comma
    E_NFI,          ///< No file for input
    E_NFO,          ///< No file for output
    E_NON,          ///< No n argument after m argument
    E_NOT,          ///< O command has no tag
    E_NYA,          ///< Numeric argument with Y
    E_NYI,          ///< Not yet implemented
    E_OFO,          ///< Output file already open
    E_PDO,          ///< Push-down list overflow
    E_POP,          ///< Attempt to move pointer off page with 'x'
    E_SNI,          ///< Semi-colon not in iteration
    E_SRH,          ///< Search failure: 'foo'
    E_TAG,          ///< Missing tag: '!foo!'
    E_TXT,          ///< Invalid text delimiter 'x'
    E_UTC,          ///< Unterminated command string
    E_UTM,          ///< Unterminated macro
    E_XAB,          ///< Execution aborted
    E_YCA,          ///< Y command aborted

    NERRORS         ///< Total number of errors
};

extern void print_command(void);

extern void print_verbose(int err_teco);

#if     defined(TEST)

//lint -save -e652

extern noreturn void (throw)(const char *func, unsigned int line, int err_teco, ...);

/// @def    throw
/// @brief  Debug version of throw(), used in order to add function and line number.

#define throw(err_teco, ...) (throw)(__func__, __LINE__, err_teco, ## __VA_ARGS__)

//lint -restore

#else

extern noreturn void throw(int err_teco, ...);

#endif

#endif  // !defined(_ERRCODES_H)
