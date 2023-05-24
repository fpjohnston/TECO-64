///
///  @file    em_cmd.c
///  @brief   Execute EM command.
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
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "cmdbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "qreg.h"
#include "term.h"


#if     defined(NOTRACE)

#include "errors.h"

// EM command does not work if tracing is disabled in this build.

void exec_EM(struct cmd *unused)
{
    throw(E_NYI);                       // Not yet implemented
}

bool scan_EM(struct cmd *unused)
{
    return false;
}

#else


///  @var     strip
///
///  @brief   Flags that are used to determine whether to ignore the echoing
///           of commands that don't actually affect execution (such as spaces
///           and comments).

static struct
{
    uint space   : 1;           ///< Strip spaces
    uint blank   : 1;           ///< Strip blank lines (ending w/ LF)
    uint white   : 1;           ///< Strip all whitespace
    uint comment : 1;           ///< Strip tags starting w/ ! and space
    uint bang    : 1;           ///< Strip comments (starting w/ !!)
} strip;

// Local functions

static bool scan_blank(void);

static void squish_cmd(int comment);


///
///  @brief    Execute EM command: echo macro in Q-register according to bits
///            set in specified Q-register. Used to "squish" macros in a manner
///            similar to the squ.tec file used in classic TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EM(struct cmd *cmd)
{
    assert(cmd != NULL);

    struct qreg *qreg = get_qreg(cmd->qindex);

    assert(qreg != NULL);               // Error if no Q-register

    if (qreg->text.len == 0)            // Nothing to do if no text
    {
        return;
    }

    if (cmd->n_set)
    {
        strip.space   = (cmd->n_arg & 1)  ? true : false;
        strip.blank   = (cmd->n_arg & 2)  ? true : false;
        strip.white   = (cmd->n_arg & 4)  ? true : false;
        strip.comment = (cmd->n_arg & 8)  ? true : false;
        strip.bang    = (cmd->n_arg & 16) ? true : false;
    }
    else
    {
        strip.space   = false;
        strip.blank   = false;
        strip.white   = false;
        strip.comment = false;
        strip.bang    = false;
    }

    if (strip.comment)
    {
        if (!cmd->m_set || cmd->m_arg == NUL || cmd->m_arg == '!' || cmd->m_arg >= DEL)
        {
            cmd->m_arg = SPACE;
        }
    }

    tbuffer *saved_cbuf = cbuf;
    uint_t saved_pos = qreg->text.pos;

    cbuf = &qreg->text;                 // Switch command strings
    cbuf->pos = 0;

    squish_cmd(cmd->m_arg);             // Squish the command string

    qreg->text.pos = saved_pos;
    cbuf = saved_cbuf;                  // Restore previous command string
}


///
///  @brief    Scan line to see if it's blank.
///
///  @returns  true if line is blank, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool scan_blank(void)
{
    uint_t pos = cbuf->pos;             // Save current position
    int c;

    while ((c = peek_cbuf()) != EOF)
    {
        if (!isspace(c) || c == TAB)
        {
            cbuf->pos = pos;            // Restore original position

            return false;               // Don't strip non-blank line
        }

        next_cbuf();                    // Skip whitespace character

        if (c == LF || c == FF || c == CR) // At end of blank line?
        {
            break;
        }
    }

    return true;
}


///
///  @brief    Scan EM command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_EM(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_ATSIGN);

    if (!scan_qreg(cmd))
    {
        throw(E_IQN, cmd->qname);       // Invalid Q-register name
    }

    return false;
}


///
///  @brief    Squish the current command string and echo the results.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void squish_cmd(int comment)
{
    struct cmd newcmd = null_cmd;       // Initialize new command
    int c;

    f.e0.skip = true;                   // Just skip over commands

    new_x();                            // Make new expression stack

    // Loop for all commands in command string.

    while ((c = fetch_cbuf()) != EOF)
    {
        bool echo = true;

        if (isgraph(c))
        {
            if (c == '!' && cbuf->pos < cbuf->len)
            {
                uint next = cbuf->pos;

                if (strip.comment && cbuf->data[next] == comment)
                {
                    echo = false;
                }
                else if (f.e1.bang && cbuf->data[next] == '!')
                {
                    if (strip.bang || strip.white)
                    {
                        echo = false;
                    }
                }
            }
        }
        else if (strip.white && isspace(c) && c != TAB)
        {
            echo = false;
        }
        else if (c == LF && strip.blank && term_pos == 0)
        {
            echo = false;
        }
        else if (c == SPACE)
        {
            if (strip.space)
            {
                echo = false;
            }
            else if (strip.blank && scan_blank())
            {
                echo = false;
            }
        }

        uint_t start = cbuf->pos - 1;   // Start of current command

        if (finish_cmd(&newcmd, c))     // Parse the rest of the command
        {
            newcmd = null_cmd;          // If done with command, start next one
        }

        if (echo)
        {
            unsigned long nbytes = cbuf->pos - start;

            if (write(STDOUT_FILENO, cbuf->data + start, nbytes) == -1)
            {
                throw(E_ERR, NULL);
            }
        }
    }

    delete_x();                         // Restore previous expression stack

    f.e0.skip = false;
}

#endif
