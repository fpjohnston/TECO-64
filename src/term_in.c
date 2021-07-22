///
///  @file    term_in.c
///  @brief   System-independent functions to input from user's terminal.
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

#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if     defined(DISPLAY_MODE)

#include <ncurses.h>

#endif

#include "teco.h"
#include "ascii.h"
#include "display.h"
#include "editbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "exec.h"
#include "qreg.h"
#include "term.h"

#include "cbuf.h"


enum
{
    FIRST_NORMAL,                   ///< Normal entry for reading input
    FIRST_PROMPT,                   ///< Restart input w/ prompt
    FIRST_NOPROMPT                  ///< Restart input w/o prompt
};


static jmp_buf jump_first;              ///< longjmp() to reset terminal input

// Local functions

static void exec_cancel(void);

static void exec_ctrl_G(void);

static void exec_DEL(void);

static void exec_inspect(int_t pos, int_t line);

static void exec_star(void);

static int read_first(void);

static void rubout_chr(int c);

static void rubout_chrs(uint n);

static void rubout_line(void);


///
///  @brief    Execute CTRL/U: delete to start of current line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_cancel(void)
{

#if     defined(DISPLAY_MODE)

    if (clear_eol())                    // Can we clear to end of line?
    {
        ;                               // If so, we're done
    }
    else

#endif

    if (f.et.rubout)
    {
        rubout_line();
    }
    else
    {
        echo_in(CTRL_U);
        echo_in(LF);
    }

    longjmp(jump_first, FIRST_PROMPT);  // Restart w/ prompt
}


///
///  @brief    Execute CTRL/G: ^G^G, ^G{SPACE}, and ^G*.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_ctrl_G(void)
{
    echo_in(CTRL_G);                    // Echo ^G

    int c = getc_term((bool)WAIT);      // Get next character
    int pos = (c == SPACE) ? (int)start_tbuf() : 0;

    if (c == DEL)
    {
        store_tbuf(CTRL_G);             // Store CTRL/G so it can be deleted
        exec_DEL();

        return;
    }
    else if (c == CTRL_U)
    {
        exec_cancel();

        return;
    }

    echo_in(c);                         // Echo next character

    switch (c)
    {
        case CTRL_G:                    // ^G^G - cancel all input
            reset_cbuf();
            echo_in(LF);

            longjmp(jump_first, FIRST_PROMPT); // Restart w/ prompt

        case SPACE:                     // ^G<SPACE> - retype current line
        case '*':                       // ^G* - retype all input lines
            if (pos == 0)
            {
                print_prompt();
            }

            echo_tbuf(pos);

            break;

        default:                        // Not special ^G command
            store_tbuf(CTRL_G);
            store_tbuf(c);              // Regular character, so just store it

            break;
    }
}


///
///  @brief    Execute "DEL": delete last character typed. We should normally
///            only get here if there is actually something to delete; if the
///            terminal buffer is empty, then BS is executed as an immediate-
///            action command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_DEL(void)
{
    int c = delete_tbuf();

    if (c == EOF)                       // This should never happen,
    {
        return;                         //  but ignore it if it does
    }

    if (f.et.rubout)
    {
        rubout_chr(c);
    }
    else
    {
        echo_in(c);
    }

    if (getlen_tbuf() == 0)             // Is terminal buffer empty now?
    {
        longjmp(jump_first, FIRST_NOPROMPT); // Yes, restart w/o prompt
    }
}


///
///  @brief    Execute "BS" or "LF": immediate-action commands to go to next or
///            previous lines in file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_inspect(int_t pos, int_t line)
{

#if     defined(DISPLAY_MODE)

    if (clear_eol())                    // Can we clear to end of line?
    {
        ;                               // If so, we're done
    }
    else

#endif

    if (f.et.rubout)
    {
        rubout_line();
    }
    else
    {
        echo_in(CR);
    }

    if (t.dot != pos)
    {
        int_t n = getdelta_ebuf(line);

        setpos_ebuf(n + t.dot);

        if (f.e0.display)
        {
            ;
        }
        else if (f.ev)
        {
            print_flag(f.ev);
        }
        else
        {
            print_flag((int_t)-1);
        }
    }
}


///
///  @brief    Execute "*": immediate-action command to store last command
///            string in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_star(void)
{
    echo_in('*');

    int qname = getc_term((bool)WAIT);  // Get Q-register name
    bool qlocal = (qname == '.');
    int qindex;

    if (qlocal)                         // Local Q-register?
    {
        echo_in('.');                   // Yes, echo the dot

        qname = getc_term((bool)WAIT);  // Get Q-register name for real
    }

    echo_in(qname);                     // Echo Q-register name

    if ((qindex = get_qindex(qname, qlocal)) == -1)
    {
        echo_in('?');                   //  alert the user to the error,

        longjmp(jump_first, FIRST_PROMPT); // and reset command string
    }

    echo_in(LF);

    struct buffer qbuf = copy_tbuf();

    store_qtext(qindex, &qbuf);
}


///
///  @brief    Read command string from terminal or indirect command file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void read_cmd(void)
{
    static int last_in;                 // Last character read

    if (f.ev)                           // Is edit verify flag set?
    {
        print_flag(f.ev);
    }

    // This allows commands, such as ^C, ^U, and ^G^G, that clear the terminal
    // buffer, to restart input and put us back in immediate-action mode.

    switch (setjmp(jump_first))
    {
        default:
        case FIRST_NORMAL:
        case FIRST_PROMPT:
            print_prompt();

            break;

        case FIRST_NOPROMPT:
            break;
    }

    int c = read_first();               // Get the first non-immediate command

    last_in = EOF;

    reset_tbuf();                       // Reset terminal buffer

    // Read characters until we have a complete command string.

    for (;;)
    {
        if ((c == ACCENT && f.et.accent && f.ee == NUL) ||
            (c == f.ee && f.ee != NUL))
        {
            echo_in('`');               // Echo delimiter as accent grave
            c = ESC;                    //  but treat it as ESCape
        }

#if     !defined(DOLLAR_ESC)

        else if (c == ESC && (f.et.accent || f.ee != NUL))
        {
            echo_in('`');               // Echo delimiter as accent grave
        }

#endif

        else if (c == ESC)
        {
            echo_in('$');               // Echo ESCape as dollar sign
        }

        if (c == CTRL_C)
        {
            echo_in(CTRL_C);
            echo_in(LF);
            store_tbuf(CTRL_C);

            if (f.et.abort)             // Should we abort?
            {
                exit(EXIT_FAILURE);     // Yes (exit with error)
            }
            else if (last_in == CTRL_C) // Second CTRL/C?
            {
                exit(EXIT_SUCCESS);     // Yes, user wants out
            }

            longjmp(jump_first, FIRST_PROMPT); // Restart w/ prompt
        }
        else if (c == BS || c == DEL)
        {
            exec_DEL();
        }
        else if (c == CTRL_G)
        {
            exec_ctrl_G();
        }
        else if (c == CTRL_U)
        {
            exec_cancel();
        }
        else if (c != ESC)
        {
            if (!f.et.lower)
            {
                c = toupper(c);
            }

            echo_in(c);
            store_tbuf(c);
        }
        else
        {
            store_tbuf(ESC);

            if (last_in == ESC)
            {
                echo_in(LF);

                while ((c = fetch_tbuf()) != EOF)
                {
                    store_cbuf(c);      // Copy command string
                }

                return;                 // Return to execute it
            }
        }

        last_in = c;                    // Save last character
        c = getc_term((bool)WAIT);      // And get new character
    }
}


///
///  @brief    Read and execute immediate-action characters, which must be
///            input immediately a prompt.
///
///            Once we return, the previous terminal buffer and error are lost.
///
///  @returns  Character to be processed.
///
////////////////////////////////////////////////////////////////////////////////

static int read_first(void)
{
    // Loop until we've seen something other than an immediate-action command.

    for (;;)
    {
        int c = getc_term((bool)WAIT);

        if ((c = readkey_dpy(c)) == EOF)
        {
            continue;
        }

        // If first character is an ESCape, or ESCape surrogate, then
        // treat it like LF.

        if (c == ESC || (c == ACCENT && f.et.accent && f.ee == NUL) ||
            (c == f.ee && c != NUL))
        {
            c = LF;
        }

        switch (c)
        {
            case BS:
            case DEL:
                if (t.dot != t.B)
                {
                    exec_inspect(t.B, (int_t)-1);
                }

                break;

            case CR:
            case LF:
                if (t.dot != t.Z)
                {
                    exec_inspect(t.Z, (int_t)1);

                    if (t.dot == t.Z)
                    {
                        print_prompt();
                    }
                }

                break;

            case CTRL_F:
                echo_in(CTRL_F);

                c = getc_term((bool)WAIT);

                echo_in(c);

                // Current format is: CTRL/F followed by a digit, or another
                // CTRL/F. Anything else is an error.

                if (c == CTRL_F || isdigit(c))
                {
                    echo_in(LF);

                    (void)exec_ctrl_F(c);
                }
                else
                {
                    echo_in('?');
                    echo_in(LF);
                }

                break;

            case CTRL_K:
                reset_colors();
                //lint -fallthrough

            case CTRL_W:
                echo_in(c);
                echo_in(LF);
                clear_dpy();

                break;

            case '/':                   // Display verbose error message
                echo_in(c);

                if (last_error != E_NUL)
                {
                    echo_in(LF);
                    print_verbose(last_error);
                }

                break;

            case '?':                   // Display erroneous command string
                if (!f.e0.error)        // But if no error message to print,
                {
                    return c;           //  then treat it as a normal character
                }

                echo_in('?');
                print_error();
                echo_in('?');
                echo_in(LF);

                break;

            case '*':                   // Store last command in Q-register
                exec_star();

                break;

            default:
                return c;
        }

        print_prompt();
    }
}


///
///  @brief    Rubout single echoed character.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void rubout_chr(int c)
{
    // Echoed input is normally only a single character, but control characters
    // may require more (or fewer) RUBOUTs.

    uint n = 1;

#if     defined(DISPLAY_MODE)

    if (f.e0.display)
    {
        n = (uint)strlen(unctrl((uint)c));
    }
    else

#endif

    if (iscntrl(c))
    {
        switch (c)
        {
            case LF:
                ++n;                    // Add extra for CR

                break;

            case BS:
            case TAB:
            case VT:
            case FF:
            case CR:
            case ESC:
                break;

            case DEL:                   // DEL doesn't echo
                return;

            default:                    // Generic control sequence
                ++n;

                break;
        }
    }
    else if (!isascii(c))
    {
        n += 3;                         // 8-bit chrs. are printed as [xx]
    }

    rubout_chrs(n);
}


///
///  @brief    Rubout multiple echoed characters.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void rubout_chrs(uint n)
{
    while (n-- > 0)
    {
        echo_in(BS);
        echo_in(SPACE);
        echo_in(BS);
    }
}


///
///  @brief    Rubout entire line (including the prompt).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void rubout_line(void)
{
    int c;

    while ((c = delete_tbuf()) != EOF)
    {
        if (isdelim(c))                 // Delimiter for previous line?
        {
            store_tbuf(c);              // Yes, put it back

            break;
        }

        rubout_chr(c);
    }

    uint n = (uint)strlen(teco_prompt);

    rubout_chrs(n);
}

