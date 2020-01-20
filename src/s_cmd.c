///
///  @file    s_cmd.c
///  @brief   Execute S command.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
///
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files (the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
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
#include "qreg.h"
#include "textbuf.h"


struct tstring last_search = { .len = 0 };

static int start_pos;                   ///< Start search at this position

static int end_pos;                     ///< End search at this position

static int text_pos;                    ///< Position of string relative to dot

static int text_len;                    ///< No. of bytes of text buffer matched

static uint match_len;                  ///< No. of characters left to match

static const char *match_buf;           ///< Next character to match


// Local functions

static bool match_chr(int c);

static void reset_search(void);

static int isblankx(int c);

static int isdelim(int c);

static int isqreg(int c);

static int issymbol(int c);

static bool match_str(void);

static bool search_loop(int count);

static bool search_backward(void);

static bool search_forward(void);

static void search_print(void);


///
///  @brief    Execute S command (local search).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_S(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->n_set && cmd->n_arg == 0)  // 0Stext` isn't allowed
    {
        print_err(E_ISA);               // Illegal search argument
    }

    if (cmd->dcolon_set)                // ::Stext` => 1,1:Stext`
    {
        cmd->m_arg = cmd->n_arg = 1;
        cmd->m_set = cmd->n_set = true;
    }
    else if (!cmd->n_set)               // Stext` => 1Stext`
    {
        cmd->n_arg = 1;
        cmd->n_set = true;
    }

    if (cmd->text1.len != 0)
    {
        free_mem(&last_search.buf);

        last_search.len = build_string(&last_search.buf, cmd->text1.buf,
                                       cmd->text1.len);
    }

    int dot = (int)getpos_tbuf();
    
    if (cmd->dcolon_set)
    {
        start_pos = end_pos = 0;
    }
    else if (cmd->n_arg < 0)
    {
        start_pos = -1;
        end_pos = -dot;

        if (cmd->m_set && cmd->m_arg != 0)
        {
            int nbytes = -cmd->m_arg + 1;

            if (end_pos < start_pos + nbytes)
            {
                end_pos = start_pos + nbytes;
            }
        }
    }
    else
    {
        start_pos = 0;
        end_pos = (int)getsize_tbuf() - dot;

        if (cmd->m_set && cmd->m_arg != 0)
        {
            int nbytes = cmd->m_arg - 1;

            if (end_pos > start_pos + nbytes)
            {
                end_pos = start_pos + nbytes;
            }
        }
    }

    if (search_loop(cmd->n_arg))
    {
        search_print();

        if (cmd->colon_set || cmd->dcolon_set)
        {
            push_expr(-1, EXPR_VALUE);
        }
    }
    else
    {
        if (cmd->colon_set || cmd->dcolon_set)
        {
            push_expr(0, EXPR_VALUE);
        }
        else
        {
            if (!f.ed.keepdot)
            {
                setpos_tbuf(0);
            }

            last_search.buf[last_search.len] = NUL;

            prints_err(E_SRH, last_search.buf);
        }
    }
}


///
///  @brief    Initialize search string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_search(void)
{
    if (atexit(reset_search) != 0)
    {
        fatal_err(errno, E_UIT, NULL);
    }
}


///
///  @brief    Check for multiple blanks (spaces or tabs) at current position.
///
///            Note that we return 1/0 instead of true/false for compatibility
///            with the ANSI isxxx() functions.
///
///  @returns  1 if one or more blanks found, else 0.
///
////////////////////////////////////////////////////////////////////////////////

static int isblankx(int c)
{
    if (!isblank(c))
    {
        return 0;
    }

    while (text_pos < end_pos)
    {
        c = getchar_tbuf(text_pos++);

        if (!isblank(c))
        {
            --text_pos;

            break;
        }
    }

    return 1;
}


///
///  @brief    Check for delimiter (LF, VT, or FF) at current position.
///
///            Note that we return 1/0 instead of true/false for compatibility
///            with the ANSI isxxx() functions.
///
///  @returns  1 if a delimiter found, else 0.
///
////////////////////////////////////////////////////////////////////////////////

static int isdelim(int c)
{
    if (c == LF || c == VT || c == FF)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


///
///  @brief    Check for a match with one of the characters in a Q-register.
///
///            Note that we return 1/0 instead of true/false for compatibility
///            with the ANSI isxxx() functions.
///
///  @returns  1 if a match found, else 0.
///
////////////////////////////////////////////////////////////////////////////////

static int isqreg(int c)
{
    int qname;
    bool qlocal = false;            
    
    if (match_len-- == 0)
    {
        print_err(E_IQN);
    }

    if ((qname = *match_buf++) == '.')
    {
        qlocal = true;

        if (match_len-- == 0)
        {
            print_err(E_IQN);
        }

        qname = *match_buf++;
    }

    struct qreg *qreg = get_qreg(qname, qlocal);

    for (uint i = 0; i < qreg->text.len; ++i)
    {
        if (c == qreg->text.buf[i])
        {
            return 1;
        }
    }

    return 0;
}


///
///  @brief    Check for a match on a symbol constituent: alphanumeric, period,
///            maybe dollar sign or underscore, depending on operating environ-
///            ment.
///
///            Note that we return 1/0 instead of true/false for compatibility
///            with the ANSI isxxx() functions.
///
///  @returns  1 if a match found, else 0.
///
////////////////////////////////////////////////////////////////////////////////

static int issymbol(int c)
{
    if (isalnum(c)   || c == '.' ||
        (f.e2.dollar && c == '$') ||
        (f.e2.ubar   && c == '_'))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


///
///  @brief    Check for a match on the current character in the text buffer,
///            allowing for the use of match control constructs in the search
///            string. Note that we can be called recursively.
///
///  @returns  true if a match found, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool match_chr(int c)
{
    if (match_len-- == 0)
    {
        print_err(E_ISS);
    }

    int match = *match_buf++;

    if (match == CTRL_E)
    {
        if (match_len-- == 0)
        {
            print_err(E_ISS);
        }

        match = toupper(*match_buf++);

        if ((match == 'A' && isalpha(c))  ||
            (match == 'B' && isblankx(c)) ||
            (match == 'C' && issymbol(c)) ||
            (match == 'D' && isdigit(c))  ||
            (match == 'G' && isqreg(c))   ||
            (match == 'L' && isdelim(c))  ||
            (match == 'R' && isalnum(c))  ||
            (match == 'S' && isblankx(c)) ||
            (match == 'V' && islower(c))  ||
            (match == 'W' && isupper(c))  ||
            (match == 'X'))
        {
            return true;
        }

        // TODO: maybe process ^EM, ^E<nnn>, and ^E[a,b,c...]?

        return false;
    }
    else if (match == CTRL_N)
    {
        return !match_chr(match);
    }
    else if ((match == CTRL_S && !isalnum(c)) || match == CTRL_X || c == match)
    {
        return true;
    }

    return false;
}


///
///  @brief    Check for delimiter (LF, VT, or FF) at current position.
///
///  @returns  true if a delimiter found, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool match_str(void)
{
    while (match_len > 0)
    {
        int c = getchar_tbuf(text_pos++);

        if (!match_chr(c))
        {
            return false;
        }
    }

    return true;
}


///
///  @brief    Reset search string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void reset_search(void)
{
    free_mem(&last_search.buf);
}


///
///  @brief    Search backward through text buffer to find next instance of
///            string in search buffer.
///
///  @returns  true if string found (text_pos will contain the buffer position
///            relative to dot, and text_len will contain the number of charac-
///            ters that were matched), and false if not found.
///
////////////////////////////////////////////////////////////////////////////////

static bool search_backward(void)
{
    // Start search at current position and see if we can get a match. If not,
    // decrement position by one, and try again. If we reach the end of the
    // text buffer without a match, then return failure, otherwise update our
    // position and return success.

    while (start_pos > end_pos)         // Search to beginning of buffer
    {
        text_pos  = start_pos--;        // Start at current position
        text_len  = 0;                  // No characters matched yet
        match_len = last_search.len;    // No. of characters left to match
        match_buf = last_search.buf;    // Start of match characters

        if (match_str())
        {
            return true;
        }
    }

    return false;
}


///
///  @brief    Search forward through text buffer to find next instance of
///            string in search buffer.
///
///  @returns  true if string found (text_pos will contain the buffer position
///            relative to dot, and text_len will contain the number of charac-
///            ters that were matched), and false if not found.
///
////////////////////////////////////////////////////////////////////////////////

static bool search_forward(void)
{
    // Start search at current position and see if we can get a match. If not,
    // increment position by one, and try again. If we reach the end of the
    // text buffer without a match, then return failure, otherwise update our
    // position and return success.

    while (start_pos < end_pos)         // Search to end of buffer
    {
        text_pos  = start_pos++;        // Start at current position
        text_len  = 0;                  // No characters matched yet
        match_len = last_search.len;    // No. of characters left to match
        match_buf = last_search.buf;    // Start of match characters

        if (match_str())
        {
            return true;
        }
    }

    return false;
}


///
///  @brief    Search forward through text buffer to find next instance of
///            string in search buffer.
///
///  @returns  true if string found (text_pos will contain the buffer position
///            relative to dot, and text_len will contain the number of charac-
///            ters that were matched), and false if not found.
///
////////////////////////////////////////////////////////////////////////////////

static bool search_loop(int count)
{
    // Start search at current position and see if we can get a match. If not,
    // increment position by one, and try again. If we reach the end of the
    // text buffer without a match, then return failure, otherwise update our
    // position and return success.

    bool backward = false;

    if (count < 0)
    {
        backward = true;
        count = -count;
    }

    while (count-- > 0)
    {
        if (backward)
        {
            if (!search_backward())
            {
                return false;
            }
        }
        else if (!search_forward())
        {
            return false;
        }
        else
        {
            // The following affects how much we move dot on multiple occurrence
            // searches. Normally we skip over the whole matched string when
            // proceeding to the nth search match. But if movedot is set, then
            // only increment by one character. This is only significant if the
            // first character of the string occurs again in the middle of it.

            if (f.ed.movedot)
            {
                ++start_pos;
            }
            else
            {
                start_pos = text_pos + text_len;
            }
        }
    }

    int dot = (int)getpos_tbuf() + text_pos + text_len;

    setpos_tbuf((uint)dot);

    return true;
}


///
///  @brief    Check to see if we need to print anything after a successful
///            search.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void search_print(void)
{
    int m    = 0;
    int n    = f.es;
    int flag = 0;         

    // Never print anything if we're in a loop or a macro.

    if (check_loop() || check_macro())
    {
        return;
    }

    if (n == 0)
    {
        return;
    }
    else
    {
        m = f.es / 256;
        n = f.es % 256;
    }

    if (n > NUL && n < SPACE)
    {
        flag = LF;
    }
    else if (n >= SPACE && n < DEL)
    {
        flag = n;
    }
    else if (n == -1)
    {
        flag = -1;
    }
    else
    {
        return;
    }

    if (m == 0)
    {
        n = getdelta_tbuf(1);
        m = getdelta_tbuf(0);
    }
    else
    {
        n = getdelta_tbuf(m);
        m = getdelta_tbuf(1 - m);
    }

    for (int i = m; i < n; ++i)
    {
        if (i == 0 && flag != -1)
        {
            echo_out(flag);
        }

        int c = getchar_tbuf(i);

        if (c == LF)
        {
            echo_out(CR);
        }

        echo_out(c);
    }
}
