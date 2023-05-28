///
///  @file    equals_cmd.c
///  @brief   Execute = (equals) command.
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

#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "cmdbuf.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "term.h"


#if     INT_T == 64

#define FORMAT_DEC      "%ld"       ///< Decimal format
#define FORMAT_HEX      "%lx"       ///< Hexadecimal format
#define FORMAT_OCT      "%lo"       ///< Octal format

#else

#define FORMAT_DEC      "%d"        ///< Decimal format
#define FORMAT_HEX      "%x"        ///< Hexadecimal format
#define FORMAT_OCT      "%o"        ///< Octal format

#endif

// Local functions

static bool check_format(const char *format);


///
///  @brief    Check format string, making sure that one and only one numeric
///            format is specified (we do allow for %%). We do this to ensure
///            that the user does not include something like %s in the format
///            string which would cause unexpected (and potentially fatal)
///            things to happen.
///
///  @returns  true if exactly one numeric format found, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool check_format(const char *format)
{
    assert(format != NULL);             // Error if format is NULL

    int c;
    bool is_numeric = false;            // No numeric format seen yet

    while ((c = *format++) != NUL)
    {
        if (c != '%')
        {
            continue;
        }
        else if ((c = *format++) == NUL)
        {
            return false;
        }
        else if (c == '%')              // %% is okay
        {
            continue;
        }
        else if (is_numeric)            // No more than one numeric format
        {
            return false;
        }

        // Here if we have something following % besides %.

        while (!isalpha(c))
        {
            if ((c = *format++) == NUL || !isprint(c))
            {
                return false;
            }
        }

        // Here when we have an alphabetic character.

        if (strchr("hlqjz", c) != NULL)
        {
            if ((c = *format++) == NUL ||
                ((c == 'h' || c == 'l') && (c = *format++) == NUL))
            {
                return false;
            }
        }

        // Here to finally check for a numeric format

        if (strchr("diouxX", c) == NULL)
        {
            return false;
        }

        is_numeric = true;
    }

    return is_numeric;
}


///
///  @brief    Execute = (equals) command.
///
///             n=     Display n in decimal, + CRLF
///             n==    Display n in octal, + CRLF
///             n===   Display n in hexadecimal, + CRLF
///             n:=    Display n in decimal, no CRLF
///             n:==   Display n in octal, no CRLF
///             n:===  Display n in hexadecimal, no CRLF
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_equals(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!cmd->n_set)                    // If no operand, then complain
    {
        throw(E_NAE);                   // No argument before =
    }

    const char *format = FORMAT_DEC;    // Assume we're printing decimal

    if (cmd->c3 == '=')                 // Print hexadecimal if ===
    {
        format = FORMAT_HEX;
    }
    else if (cmd->c2 == '=')            // Print octal if ==
    {
        format = FORMAT_OCT;
    }

    // The following is an extension that allows the use of a complex format
    // string. This is only available when using the at-sign form of this
    // command. This format allows use of the string building characters, such
    // as "^EQq", and also allows a numeric output format such as "%06u". The
    // check_format() function ensures that there is at most one such numeric
    // format, and no others that would require additional arguments, such as
    // "%s", that would cause problems during printing.

    tstring result = { .data = NULL };

    if (cmd->atsign)
    {
        cmd->atsign = false;

        if (cmd->text1.len != 0)
        {
            result = build_string(cmd->text1.data, cmd->text1.len);

            if (check_format(result.data))
            {
                format = result.data;
            }
        }
    }

    tprint(format, cmd->n_arg);

    if (!cmd->colon)                    // Suppress CR/LF?
    {
        type_newline();
    }
}


///
///  @brief    Scan = (equals) command.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_equals(struct cmd *cmd)
{
    assert(cmd != NULL);

    assert(cmd != NULL);

    // If extended operators are not enabled, or we're not inside any
    // parentheses, then the equals sign is not part of an operator, but
    // is instead a command.

    if (f.e1.xoper && check_parens())
    {
        confirm(cmd, NO_M, NO_COLON, NO_DCOLON, NO_ATSIGN);

        if (require_cbuf() != '=')      // If we have one '=', we must have two
        {
            throw(E_ARG);
        }

        if (cmd->n_set)
        {
            cmd->n_set = false;

            store_val(cmd->n_arg);
        }

        store_oper(X_EQ);

        return true;
    }

    // Here if equals sign is a command and not part of a relational operator.

    int c;

    confirm(cmd, NO_M, NO_DCOLON);

    if ((c = peek_cbuf()) == '=')
    {
        next_cbuf();

        cmd->c2 = (char)c;

        if ((c = peek_cbuf()) == '=')
        {
            next_cbuf();

            cmd->c3 = (char)c;
        }
    }

    // We only allow text arguments if the at sign modifier is used. This
    // is to maintain backward compatibility with macros for other TECOs
    // that don't expect text arguments after the command.

    if (cmd->atsign)
    {
        if (!f.e1.equals)
        {
            throw(E_EXT);               // Extended feature not enabled
        }

        scan_texts(cmd, 1, ESC);

        cmd->atsign = true;             // Re-set after scan_texts() clears flag
    }

    return false;
}
