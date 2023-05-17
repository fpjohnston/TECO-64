///
///  @file    term_in.c
///  @brief   System-independent functions to input from user's terminal.
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

#include <ctype.h>
#include <errno.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

#if     defined(DEBUG)

#include <string.h>

#endif

#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "cmdbuf.h"
#include "display.h"
#include "editbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "exec.h"
#include "qreg.h"
#include "term.h"


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

static int read_wait(void);


///
///  @brief    Execute CTRL/U: delete to start of current line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_cancel(void)
{
    if (clear_eol())                    // Can we clear to end of line?
    {
        term_pos = 0;                   // Yes, so we're back at column 0
    }
    else if (f.et.rubout)
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

    int c = getc_term(WAIT);            // Get next character

    switch (c)
    {
        case CTRL_G:                    // ^G^G - cancel all input
            echo_in(c);
            reset_cbuf();
            echo_in(LF);

            longjmp(jump_first, FIRST_PROMPT); // Restart w/ prompt

        case CTRL_U:
            exec_cancel();

            break;

        case SPACE:                     // ^G<SPACE> - retype current line
            echo_in(LF);
            retype_line(start_tbuf());

            break;

        case '*':                       // ^G* - retype all input lines
            echo_in(c);
            echo_in(LF);
            retype_line((uint_t)0);

            break;

        case DEL:
            store_tbuf(CTRL_G);         // Store CTRL/G so it can be deleted
            exec_DEL();

            break;

        default:                        // Not special ^G command
            echo_in(c);
            store_tbuf(CTRL_G);
            store_tbuf(c);              // Regular character, so just store it

            break;
    }
}


///
///  @brief    Execute DEL: delete last character typed. We should normally
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
///  @brief    Execute BS or LF: immediate-action commands to go to next or
///            previous lines in file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_inspect(int_t pos, int_t line)
{
    if (clear_eol())                    // Can we clear to end of line?
    {
        ;                               // If so, we're done
    }
    else if (f.et.rubout)
    {
        rubout_line();
    }
    else
    {
        echo_in(CR);
    }

    if (t->dot != pos)
    {
        int_t n = len_edit(line);

        set_dot(n + t->dot);

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
///  @brief    Execute * (star) immediate-action command to store last command
///            string in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_star(void)
{
    echo_in('*');

    int qname = getc_term(WAIT);        // Get Q-register name
    bool qlocal = (qname == '.');
    int qindex;

    if (qlocal)                         // Local Q-register?
    {
        echo_in('.');                   // Yes, echo the dot

        qname = getc_term(WAIT);        // Get Q-register name for real
    }

    echo_in(qname);                     // Echo Q-register name

    if ((qindex = get_qindex(qname, qlocal)) == -1)
    {
        echo_in('?');                   //  alert the user to the error,

        longjmp(jump_first, FIRST_PROMPT); // and reset command string
    }

    echo_in(LF);

    tbuffer qbuf = copy_tbuf();

    store_qtext(qindex, &qbuf);
}


///
///  @brief    Get single character from terminal.
///
///  @returns  Character read. EOF can be returned, but only if the wait flag
///            was clear, indicating that the caller did not want to wait if
///            there were no input characters immediately available.
///
////////////////////////////////////////////////////////////////////////////////

int getc_term(bool wait)
{
    static bool LF_pending = false;

    if (LF_pending)
    {
        LF_pending = false;

        return LF;
    }

    int c;

    if (wait)
    {
        c = read_wait();
    }
    else if ((c = get_nowait()) == EOF)
    {
        return EOF;
    }

    // Here when we have a non-EOF character. See if it requires special
    // processing for display mode. If check_key() returns an EOF, that
    // means that a resize occurred, so re-print the prompt and then get
    // another character.

    if ((c = check_key(c)) == EOF)
    {
        term_pos = 0;

        print_prompt();

        return getc_term(wait);         // Recurse to get next character
    }

    f.e0.sigint = false;                // Normal character, not CTRL/C

    if (f.e3.CR_in && c == LF)
    {
        LF_pending = true;

        return CR;
    }
    else if (f.e0.display && c == CR)
    {
        if (f.e3.CR_in)                 // Allow CR/LF?
        {
            LF_pending = true;          // Yes, output LF next
        }
        else
        {
            c = LF;                     // No, so just change CR to LF
        }
    }

#if     defined(DEBUG)

    putc_key(c);                        // Write character to keystroke file

#endif

    return c;
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
        else if (c == ESC)
        {
            if (!f.e1.dollar && (f.et.accent || f.ee != NUL))
            {
                echo_in('`');           // Echo delimiter as accent grave
            }
            else
            {
                echo_in('$');           // Echo ESCape as dollar sign
            }
        }

        if (c == CTRL_C)
        {
            echo_in(CTRL_C);
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
        else if (c == ESC)
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
        else
        {
            echo_in(c);

            if (c == LF && check_help())
            {
                throw(E_NYI);           // No help available yet
            }
            else if (!f.et.lower)
            {
                c = toupper(c);
            }

            store_tbuf(c);
        }

        last_in = c;                    // Save last character
        c = getc_term(WAIT);            // And get new character
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
    static bool last_ctrl_w = false;    // Last character was CTRL/W

    bool found_ctrl_w = false;          // Assume no CTRL/W this time

    // Loop until we've seen something other than an immediate-action command.

    for (;;)
    {
        int c = getc_term(WAIT);

        if ((c = exec_key(c)) == EOF)
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
                if (t->dot != t->B)
                {
                    exec_inspect(t->B, (int_t)-1);
                }

                break;

            case CR:
            case LF:
                if (t->dot != t->Z)
                {
                    exec_inspect(t->Z, (int_t)1);

                    if (t->dot == t->Z)
                    {
                        print_prompt();
                    }
                }

                break;

            case CTRL_F:
                echo_in(CTRL_F);

                c = getc_term(WAIT);

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
                echo_in(c);
                echo_in(LF);
                reset_colors();
                reset_dpy((bool)true);

                break;

            case CTRL_W:
                echo_in(c);
                echo_in(LF);

                if (last_ctrl_w)        // Refresh all windows?
                {
                    last_ctrl_w = false;

                    reset_dpy((bool)true);
                }
                else                    // No, just refresh edit window
                {
                    reset_dpy((bool)false);

                    found_ctrl_w = true;
                }

                break;

#if     defined(DEBUG)

            case CTRL_RIGHT:
                if (f.e1.repeat)
                {
                    echo_in(c);         // Echo ^]

                    c = getc_term(WAIT);

                    if (c == CTRL_RIGHT)
                    {
                        echo_in(c);     // Echo ^] again
                        echo_in(LF);

                        size_t len = strlen(cbuf->data);

                        if (len != 0)
                        {
                            char temp[len + 1];

                            strcpy(temp, cbuf->data);

                            exec_str(temp); // Re-execute last command string
                            refresh_dpy();  // Do any necessary screen update
                        }

                        break;
                    }
                }

                echo_in('?');
                echo_in(LF);

                break;

#endif

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
                print_command();
                echo_in('?');
                echo_in(LF);

                break;

            case '*':                   // Store last command in Q-register
                exec_star();

                break;

            default:
                last_ctrl_w = false;

                return c;
        }

        last_ctrl_w = found_ctrl_w;

        print_prompt();
    }
}


///
///  @brief    Read with wait (blocking I/O).
///
///  @returns  Character read. Note the following special cases:
///
///            1. If a read is interrupted by a CTRL/C, we will either return
///               the CTRL/C, if so requested by the ET flag, or we will issue
///               an XAB error.
///
///            2. If any error other than EINTR occurs, we will issue an XAB
///               error.
///
////////////////////////////////////////////////////////////////////////////////

static int read_wait(void)
{
    bool ctrl_c = f.et.ctrl_c;

    if (f.e0.display)                   // Display mode active?
    {
        int c = get_wait();

        if (c != EOF)
        {
            return c;
        }
    }
    else
    {
        char chr;
        ssize_t nbytes = read(fileno(stdin), &chr, sizeof(chr));

        if (nbytes == 0)                // EOF reading redirected stdin
        {
            exit(EXIT_SUCCESS);         // So we're all done
        }
        else if (nbytes != -1)          // Error?
        {
            return chr;
        }
    }

    // Here if getch_wait() or read() returned an error

    if (errno != EINTR)                 // Interrupted by CTRL/C?
    {
        throw(E_ERR, NULL);             // General error
    }
    else if (ctrl_c)                    // Trapping CTRL/C?
    {
        f.e0.sigint = false;            // No, reset system flag

        return CTRL_C;                  // And return CTRL/C to caller
    }

    // Here if not trapping CTRL/C

    if (f.e0.ctrl_t)
    {
        throw(E_XAB);                   // Execution aborted
    }
    else
    {
        echo_in(CTRL_C);                // Print alert message

        longjmp(jump_main, MAIN_CTRLC);
    }
}


///
///  @brief    Re-type current line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void retype_line(uint_t pos)
{
    term_pos = 0;

    if (pos == 0)                       // At start of terminal buffer?
    {
        print_prompt();                 // Yes, output prompt first
    }

    echo_tbuf(pos);
}
