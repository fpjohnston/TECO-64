///
///  @file    term_in.c
///  @brief   System-independent functions to input from user's terminal.
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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "editbuf.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"
#include "qreg.h"
#include "term.h"
#include "window.h"


#define FF_LINES    40                  ///< No. of lines to print for FF

#define VT_LINES    4                   ///< No. of lines to print for VT

static int last_in = EOF;               ///< Last character read


// Local functions

static void read_bs(void);

static void read_bs_or_lf(int pos, int line);

static int read_chr(int c, bool accent);

static void read_cr(void);

static void read_ctrl_c(int last);

static void read_ctrl_g(void);

static void read_ctrl_u(void);

static void read_ctrl_z(void);

static void read_ff(void);

static int read_first(void);

static void read_lf(void);

static void read_qname(int c);

static void read_vt(void);



///
///  @brief    Process backspace.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_bs(void)
{
    int c = delete_tbuf();

    if (c != EOF)
    {
        if (f.et.rubout)
        {
            print_echo(BS);
            print_echo(SPACE);
            print_echo(BS);
        }
        else
        {
            print_echo(c);
        }
    }
}


///
///  @brief    Process immediate-mode BS or LF character.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_bs_or_lf(int pos, int line)
{
    if (clear_eol())                    // Can we clear to end of line?
    {
        ;                               // If so, we're done
    }
    else if (f.et.rubout)
    {
        int c;
        int nbytes = (int)strlen(prompt);

        while ((c = delete_tbuf()) != EOF)
        {
            if (isdelim(c))
            {
                store_tbuf(c);

                break;
            }

            ++nbytes;
        }

        for (int i = 0; i < nbytes; ++i)
        {
            print_echo(BS);
            print_echo(SPACE);
            print_echo(BS);
        }
    }
    else
    {
        print_echo(CR);
    }

    if (t.dot != pos)
    {
        int n = getdelta_ebuf(line);

        setpos_ebuf(n + t.dot);

        if (f.ev)
        {
            flag_print(f.ev);
        }
        else
        {
            flag_print(-1);
        }
    }
}


///
///  @brief    Read next character.
///
///  @returns  Character processed, or EOF if double escape seen.
///
////////////////////////////////////////////////////////////////////////////////

static int read_chr(int c, bool accent)
{
    switch (c)
    {
        case BS:
        case DEL:
            read_bs();

            break;

        case FF:
            read_ff();

            break;

        case CR:
            read_cr();

            break;

        case CTRL_C:
            read_ctrl_c(last_in);

            break;

        case CTRL_G:
            read_ctrl_g();

            break;

        case CTRL_U:
            read_ctrl_u();

            break;

        case CTRL_Z:
            read_ctrl_z();

            break;

        case ESC:
            echo_in(accent ? '`' : '$');
            store_tbuf(c);

            if (last_in == ESC)
            {
                last_in = EOF;

                print_echo(CRLF);

                return EOF;             // Done reading command
            }

            break;

        case LF:
            read_lf();

            break;

        case VT:
            read_vt();

            break;

        default:
            if (!f.et.lower)
            {
                c = toupper(c);
            }

            echo_in(c);
            store_tbuf(c);

            break;
    }

    return c;
}


///
///  @brief    Read next character from terminal.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void read_cmd(void)
{
    int c;

    if (current->pos != current->len)
    {
        return;                         // Just process current command
    }

    switch (read_indirect())
    {
        case -1:                        // Exit if we have a complete command
            return;

        case 1:
            c = getc_term((bool)WAIT);  // Partial command - get another chr.

            break;

        default:
        case 0:
            c = read_first();           // Start new command

            break;
    }

    // We don't reset the terminal buffer until we get here, so that we can
    // use the *x command to save the previous contents in Q-register 'x'.

    reset_tbuf();                       // Reset terminal buffer

    for (;;)
    {
        // If the character is an accent grave and and the et.accent bit is set,
        // or it matches a non-NUL EE flag, then treat it as an ESCape.

        bool accent = false;

        if ((f.et.accent && c == ACCENT) || f.ee == c)
        {
            accent = true;
            c = ESC;                    // Process it as ESCape.
        }

        if (read_chr(c, accent) == EOF)
        {
            break;
        }

        last_in = c;

        c = getc_term((bool)WAIT);
    }

    // Copy everything to command buffer.

    while ((c = fetch_tbuf()) != EOF)
    {
        store_cbuf(c);
    }
}



///
///  @brief    Process carriage return.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_cr(void)
{
    print_echo(CR);

    store_tbuf(CR);
}


///
///  @brief    Process CTRL/C.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_ctrl_c(int last)
{
    echo_in(CTRL_C);

    if (f.et.abort)
    {
        exit(EXIT_FAILURE);
    }

    store_tbuf(CTRL_C);
    print_echo(CRLF);

    if (last == CTRL_C)                 // Second CTRL/C?
    {
        exit(EXIT_SUCCESS);             // Yes: clean up, reset, and exit
    }

    print_prompt();
}


///
///  @brief    Process CTRL/G, looking for ^G^G, ^G{SPACE}, and ^G*.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_ctrl_g(void)
{
    echo_in(CTRL_G);

    int c = getc_term((bool)WAIT);      // Get next character

    echo_in(c);                         // Echo it

    if (c == CTRL_G)                    // ^G^G
    {
        reset_cbuf();
        print_echo(CRLF);               // Start new line
        put_bell();

        longjmp(jump_main, 1);
    }
    else if (c == SPACE)                // ^G<SPACE> - retype current line
    {
        if (start_tbuf() == 0)          // First line?
        {
            print_prompt();             // Yes, we need the prompt
        }

        echo_tbuf((int)start_tbuf());
    }
    else if (c == '*')                  // ^G* - retype all input lines
    {
        print_echo(CRLF);
        print_prompt();

        echo_tbuf(0);
    }
    else                                // Not special CTRL/G sequence
    {
        store_tbuf(CTRL_G);
        store_tbuf(c);                  // Regular character, so just store it
    }
}


///
///  @brief    Process CTRL/U: delete to start of current line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_ctrl_u(void)
{
    if (clear_eol())                    // Can we clear to end of line?
    {
        ;                               // If so, we're done
    }
    else if (f.et.rubout)
    {
        int c;
        int nbytes = (int)strlen(prompt);

        while ((c = delete_tbuf()) != EOF)
        {
            if (isdelim(c))
            {
                store_tbuf(c);

                break;
            }

            ++nbytes;
        }

        for (int i = 0; i < nbytes; ++i)
        {
            print_echo(BS);
            print_echo(SPACE);
            print_echo(BS);
        }
    }
    else
    {
        echo_in(CTRL_U);
        print_echo(CRLF);
    }

    print_prompt();
}


///
///  @brief    Process input CTRL/Z.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_ctrl_z(void)
{
    echo_in(CTRL_Z);
    store_tbuf(CTRL_Z);

    int c = getc_term((bool)WAIT);

    echo_in(CTRL_Z);

    if (c == CTRL_Z)                    // Two CTRL/Z's?
    {
        exit(EXIT_SUCCESS);             // Clean up, reset, and exit
    }

    store_tbuf(c);                      // Normal character
}


///
///  @brief    Process input form feed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_ff(void)
{
    print_echo(CR);

    for (int i = 0; i < FF_LINES; ++i)
    {
        print_echo(LF);
    }

    store_tbuf(FF);
}


///
///  @brief    Read first character from command string. This is required if
///            special handling is needed, especially for the following immedi-
///            ate-mode commands, which are processed without destroying the
///            previous command buffer:
///
///            ?    Display previous command string up to erroneous command
///            /    Display verbose explanation of last error
///            *{q} Copy last command string to q-register {q}.
///
///            Once we return, the previous command string and error are con-
///            sidered lost. Our caller then overwrites the old command string
///            with a new one.
///
///            Note that although LF and BS are immediate-mode commands, we do
///            not deal with them here to avoid a situation such as a LF or BS
///            being typed following a CTRL/U used to delete a command string.
///            For this reason, these commands must be handled by our caller.
///
///  @returns  Character to be processed.
///
////////////////////////////////////////////////////////////////////////////////

static int read_first(void)
{
    // Loop until we seen something other than an immediate-mode command.

    bool prompt_enabled = true;

    reset_cbuf();

    for (;;)
    {
        if (f.ev)                       // Is edit verify flag set?
        {
            flag_print(f.ev);
        }

        if (prompt_enabled)
        {
            print_prompt();
        }

        prompt_enabled = true;

        int c = getc_term((bool)WAIT);

        c = readkey_win(c);             // See if it's a window key

        switch (c)
        {
            case BS:
                read_bs_or_lf(t.B, -1);

                break;

            case LF:
                read_bs_or_lf(t.Z, 1);

                break;

            case CTRL_K:
                echo_in(CTRL_K);
                echo_in(CRLF);
                reset_colors();
                clear_win();

                break;

            case CTRL_W:
                echo_in(CTRL_W);
                echo_in(CRLF);
                clear_win();

                break;

            case '/':                   // Display verbose error message
                echo_in(c);

                if (last_error != E_NUL)
                {
                    echo_in(CRLF);
                    help_err(last_error);
                }

                break;

            case '?':                   // Display erroneous command string
                if (!f.e0.error)        // If no error,
                {
                    return c;           //  then just return '?'
                }
                else
                {
                    echo_in(c);
                    echo_tbuf(0);       // Echo command line
                    echo_in(c);
                    echo_in(CRLF);
                }

                break;

            case '*':                   // Store last command in Q-register
                echo_in(c);
                c = getc_term((bool)WAIT); // Get Q-register name

                if (f.e0.ctrl_c)
                {
                    f.e0.ctrl_c = false;

                    return CTRL_C;
                }

                read_qname(c);

                break;

            case DEL:
                echo_in(CRLF);

                break;

            case EOF:                   // Window key we already processed
                prompt_enabled = false;

                break;

            default:
                return c;
        }
    }
}


///
///  @brief    Process input line feed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_lf(void)
{
    print_echo(LF);
    store_tbuf(LF);
}


///
///  @brief    Get Q-register name and store command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_qname(int c)
{
    if (c == CR)
    {
        return;
    }
    else if (c == BS || c == DEL || c == CTRL_U)
    {
        print_echo(CR);

        return;
    }

    int qname = c;
    bool qdot = (qname == '.');

    if (qdot)                           // Local Q-register?
    {
        echo_in(c);                     // Yes, echo the dot

        qname = getc_term((bool)WAIT);  // And get next character
    }

    echo_in(qname);                     // Echo Q-register name

    print_echo(CRLF);

    assert(term_buf != NULL);

    struct buffer qbuf =
    {
        .len  = term_buf->len,
        .pos  = term_buf->pos,
        .size = term_buf->size,
    };

    qbuf.buf  = alloc_mem(term_buf->len);

    memcpy(qbuf.buf, term_buf->buf, (ulong)qbuf.len);

    store_qtext(qname, qdot, &qbuf);
}


///
///  @brief    Process input vertical tab.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void read_vt(void)
{
    print_echo(CR);

    for (int i = 0; i < VT_LINES; ++i)
    {
        print_echo(LF);
    }

    store_tbuf(VT);
}
