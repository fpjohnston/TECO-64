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

#include "teco.h"
#include "ascii.h"
#include "cbuf.h"
#include "estack.h"
#include "exec.h"
#include "qreg.h"


///  @struct  strip
///
///  @brief   Flags that are used to determine whether to ignore the echoing
///           of commands that don't actually affect execution (such as spaces
///           and comments).

struct strip
{
    uint space : 1;                 ///< Strip spaces
    uint blank : 1;                 ///< Strip blank lines (ending w/ LF)
    uint white : 1;                 ///< Strip VT, FF, or CR
    uint bang  : 1;                 ///< Strip comments
    uint bang2 : 1;                 ///< Strip EOL comments
};


// Local functions

static bool strip_cmd(int c, const struct strip strip);


///
///  @brief    Execute EM command: echo macro in Q-register according to bits
///            set in specified Q-register. Used to "squish" macros.
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

    struct strip strip;

    if (cmd->n_set)
    {
        strip.space = (cmd->n_arg & 1)  ? true : false;
        strip.blank = (cmd->n_arg & 2)  ? true : false;
        strip.white = (cmd->n_arg & 4)  ? true : false;
        strip.bang  = (cmd->n_arg & 8)  ? true : false;
        strip.bang2 = (cmd->n_arg & 16) ? true : false;
    }
    else
    {
        strip.space = false;
        strip.blank = false;
        strip.white = false;
        strip.bang  = false;
        strip.bang2 = false;
    }

    tbuffer *saved_cbuf = cbuf;
    bool saved_trace = f.trace;
    bool saved_exec = f.e0.exec;
    uint_t saved_pos = qreg->text.pos;

    cbuf = &qreg->text;                 // Switch command strings
    cbuf->pos = 0;

    f.e0.exec = false;                  // Don't actually execute commands

    push_x();                           // Save expression stack

    // Loop for all commands in command string.

    struct cmd newcmd = null_cmd;       // Initialize new command
    int c;

    while ((c = peek_cbuf()) != EOF)
    {
        f.trace = strip_cmd(c, strip);

        next_cbuf();                    // Accept character and maybe trace it

        if (finish_cmd(&newcmd, c))
        {
            newcmd = null_cmd;
        }
    }

    pop_x();                            // Restore expression stack

    cbuf = saved_cbuf;                  // Restore previous command string
    qreg->text.pos = saved_pos;
    f.e0.exec = saved_exec;
    f.trace = saved_trace;
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

    reject_m(cmd->m_set);
    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);
    scan_qreg(cmd);

    return false;
}


///
///  @brief    See if we should echo the current command.
///
///  @returns  true if should echo command, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool strip_cmd(int c, const struct strip strip)
{
    switch (c)
    {
        case SPACE:
            if (strip.space)            // Stripping spaces?
            {
                return false;           // Don't trace space
            }
            else if (strip.blank)       // Stripping blank lines?
            {
                uint_t pos = cbuf->pos; // Save current position

                f.trace = false;

                while ((c = peek_cbuf()) != EOF)
                {
                    if (!isspace(c) || c == TAB)
                    {
                        cbuf->pos = pos; // Restore original position

                        return true;    // Don't strip non-blank line
                    }

                    next_cbuf();        // Skip whitespace character

                    if (isdelim(c))     // At end of blank line?
                    {
                        break;
                    }
                }

                return false;
            }

            break;

        case LF:
            if (strip.white)            // Stripping line delimiters?
            {
                return false;           // Don't trace line feed
            }
            else if (strip.blank && term_pos == 0)
            {
                return false;           // Don't trace blank line
            }

            break;

        case CR:
        case VT:
        case FF:
            if (strip.white)            // Stripping line delimiters?
            {
                return false;           // Don't trace CR, VT, or FF
            }

            break;

        case '!':                       // Check for tracing comments
        {
            uint next = cbuf->pos + 1;

            if (strip.bang && cbuf->data[next] == ' ')
            {
                return false;           // Don't trace comment
            }
            else if (f.e1.bang && cbuf->data[next] == '!')
            {
                if (strip.bang2 || strip.white)
                {
                    return false;       // Don't trace comment
                }
            }

            break;
        }

        default:
            break;
    }

    return true; 
}
