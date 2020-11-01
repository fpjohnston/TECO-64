///
///  @file    term_in.c
///  @brief   System-independent functions to input from user's terminal.
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include <ncurses.h>
#include <setjmp.h>
#include <stdbool.h>                    //lint !e537
#include <stdio.h>                      //lint !e537
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "display.h"
#include "editbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "exec.h"
#include "qreg.h"
#include "term.h"


static jmp_buf jump_first;              ///< longjmp() to reset terminal input

// Local functions

static void exec_BS(void);

static void exec_cancel(void);

static void exec_ctrl_G(void);

static void exec_inspect(int pos, int line);

static void exec_star(void);

static int read_first(void);

static void rubout_chr(int c);

static void rubout_chrs(uint n);

static void rubout_line(void);


///
///  @brief    Execute "BS": delete last character typed. We should normally
///            only get here if there is actually something to delete; if the
///            terminal buffer is empty, then BS is executed as an immediate-
///            action command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_BS(void)
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
        print_echo(c);
    }

    if (getlen_tbuf() == 0)             // Is terminal buffer empty now?
    {
        longjmp(jump_first, 2);         // Yes, restart w/o prompt
    }
}


///
///  @brief    Execute CTRL/U: delete to start of current line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_cancel(void)
{

#if     defined(TECO_DISPLAY)

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
        print_echo(CRLF);
    }

    longjmp(jump_first, 1);             // Restart w/ prompt
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

    echo_in(c);                         // Echo next character

    switch (c)
    {
        case CTRL_G:                    // ^G^G - cancel all input
            reset_cbuf((bool)true);
            print_echo(CRLF);

            longjmp(jump_first, 1);     // Restart w/ prompt

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
///  @brief    Execute "BS" or "LF": immediate-action commands to go to next or
///            previous lines in file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_inspect(int pos, int line)
{

#if     defined(TECO_DISPLAY)

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
        print_echo(CR);
    }

    if (t.dot != pos)
    {
        int n = getdelta_ebuf(line);

        setpos_ebuf(n + t.dot);

        if (f.e0.display)
        {
            ;
        }
        else if (f.ev)
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
    bool qdot = (qname == '.');

    if (qdot)                           // Local Q-register?
    {
        echo_in('.');                   // Yes, echo the dot

        qname = getc_term((bool)WAIT);  // Get Q-register name for real
    }

    echo_in(qname);                     // Echo Q-register name

    if (!isalnum(qname))                // If invalid Q-register,
    {
        echo_in('?');                   //  alert the user to the error,

        longjmp(jump_first, 1);         //   and reset command string
    }

    print_echo(CRLF);

    struct buffer qbuf = copy_tbuf();

    store_qtext(qname, qdot, &qbuf);
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
        flag_print(f.ev);
    }

    // This allows commands, such as ^C, ^U, and ^G^G, that clear the terminal
    // buffer, to restart input and put us back in immediate-action mode.

    switch (setjmp(jump_first))
    {
        case 0:
        case 1:
            print_prompt();
            break;

        default:
            break;
    }

    int c = read_first();               // Get the first non-immediate command

    last_in = EOF;

    // We don't reset the terminal buffer until we get here, because the user
    // might enter a "*q" immediate-action command to save the contents of the
    // terminal buffer in a Q-register.

    reset_tbuf();                       // Reset terminal buffer

    // Read characters until we have a complete command string.

    for (;;)
    {
        if ((c == ACCENT && f.et.accent) || (c == f.ee) ||
            (c == ESC && (f.et.accent || f.ee != -1)))
        {
            echo_in('`');               // Echo delimiter as accent grave
            c = ESC;                    //  but treat it as ESCape
        }
        else if (c == ESC)
        {
            echo_in('$');               // Echo ESCape as dollar sign
        }

        if (c == CTRL_C)
        {
            echo_in(CTRL_C);
            store_tbuf(CTRL_C);
            print_echo(CRLF);

            if (f.et.abort)             // Should we abort?
            {
                exit(EXIT_FAILURE);     // Yes (exit with error)
            }
            else if (last_in == CTRL_C) // Second CTRL/C?
            {
                exit(EXIT_SUCCESS);     // Yes, user wants out
            }

            longjmp(jump_first, 1);     // Restart w/ prompt
        }
        else if (c == BS || c == DEL)
        {
            exec_BS();
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
                print_echo(CRLF);

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

        if (c == ESC || (c == ACCENT && f.et.accent) || c == f.ee)
        {
            c = LF;
        }

        switch (c)
        {
            case BS:
            case DEL:
                if (t.dot != t.B)
                {
                    exec_inspect(t.B, -1);
                }

                break;

            case CR:
            case LF:
                if (t.dot != t.Z)
                {
                    exec_inspect(t.Z, 1);

                    if (t.dot == t.Z)
                    {
                        print_prompt();
                    }
                }

                break;

            case CTRL_K:
                reset_colors();
                //lint -fallthrough

            case CTRL_W:
                echo_in(c);
                echo_in(CRLF);
                clear_dpy();

                break;

            case '/':                   // Display verbose error message
                echo_in(c);

                if (last_error != E_NUL)
                {
                    echo_in(CRLF);
                    print_help(last_error);
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
    // Echoed input is normally only a single character, but if we're not
    // in image mode, then control characters require an extra RUBOUT.

    uint n;

    if (f.e0.display)
    {
        n = (uint)strlen(unctrl((uint)c));
    }
    else
    {
        n = iscntrl(c) && !f.et.image ? 2 : 1;
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
        print_echo(BS);
        print_echo(SPACE);
        print_echo(BS);
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

