///
///  @file    ei_cmd.c
///  @brief   Execute EI command.
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
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "cbuf.h"
#include "eflags.h"
#include "estack.h"
#include "exec.h"
#include "file.h"


#define EI_MAX      128                 ///< Max. depth for EI commands

///   @var    ei_depth
///   @brief  Nesting depth for new-style EI commands. Note that this can be no
///           greater than 1 if we're processing an old-style EI command.

static uint ei_depth = 0;

///   @var    ei_stack
///   @brief  EI command buffer array.

static tbuffer ei_stack[EI_MAX];

///   @var    ei_old
///   @brief  Flag to distinguish between old-style and new-style EI commands.
///           The ei_stack[] array is used for both, but if we're processing
///           an old-style EI command, then ei_old is set to true, and the
///           array can have at most one element (because old-style EI commands
///           cannot be nested).

static bool ei_old = false;


///
///  @brief    Set or clear old-style EI flag. Basically, we verify that we're
///            not already processing a old-style EI command when the user wants
///            to switch to old-style, or vice versa. This is because new-style
///            commands can be nested, whereas old-style commands are more like
///            chaining one indirect command file to another: once an EI command
///            is seen, execution of that file terminates and we open up a new
///            file and start execution of the commands found therein.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_EI(bool ei_new)
{
    if (ei_depth > 1)                   // Have we started nesting?
    {
        if (ei_old && ei_new)           // Yes. Changing horses in mid-stream?
        {
            throw(E_EIE);               // EI command error
        }
    }

    ei_old = !ei_new;                   // Change new to old and old to new
}


///
///  @brief    Execute EI command: read TECO command file. This is handled in
///            one of two ways: preset where input is read from after execution
///            of the command string has completed, as DEC TECOs have done, or
///            immediately execute the contents of the file as though it were a
///            as TECO C has done. Which behavior is used depends on the new_ei
///            bit in the E1 extended features flag. By default, this bit is
///            enabled.
///
///            N.B.: the new_ei bit should not be changed after TECO starts up,
///            or while executing EI commands, as this could have unpredictable
///            and undesirable results.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EI(struct cmd *cmd)
{
    assert(cmd != NULL);

    const char *name = cmd->text1.data;
    uint_t len       = cmd->text1.len;
    uint stream      = IFILE_INDIRECT;

    close_input(stream);                // Close any open file

    if (f.e1.new_ei)                    // New-style EI
    {
        if (len == 0)
        {
            if (ei_depth != 0)
            {
                cbuf->pos = cbuf->len;
            }

            if (cmd->colon)
            {
                push_x(SUCCESS, X_OPERAND);
            }

            return;
        }

        if ((name = init_filename(name, len, cmd->colon)) != NULL)
        {
            if (ei_depth == EI_MAX)
            {
                throw(E_EIE);           // EI command error (stack overflow)
            }

            if (open_command(name, stream, cmd->colon, &ei_stack[ei_depth]))
            {
                if (cmd->colon)
                {
                    push_x(SUCCESS, X_OPERAND);
                }

                if (ei_stack[ei_depth].size != 0)
                {
                    // Incrementing ei_depth is necessary here to deal with any
                    // nesting that will occur if the macro we are executing
                    // contains an EI command that will cause another macro to
                    // be loaded and executed.

                    exec_macro(&ei_stack[ei_depth++], cmd);

                    --ei_depth;
                }

                free_mem(&ei_stack[ei_depth].data);

                return;
            }
        }
    }
    else                                // Old-style EI
    {
        if (len == 0)                   // @EI//?
        {
            longjmp(jump_main, MAIN_CTRLC); // Yes, act like CTRL/C typed
        }

        if ((name = init_filename(name, len, cmd->colon)) != NULL)
        {
            if (ei_depth == 0)          // Trying to nest EI command?
            {
                ++ei_depth;
            }
            else
            {
                free_mem(&ei_stack[0].data);
            }

            ei_old = true;

            if (open_command(name, stream, cmd->colon, &ei_stack[0]))
            {
                if (cmd->colon)
                {
                    push_x(SUCCESS, X_OPERAND);
                }

                return;
            }
        }
    }

    if (cmd->colon)
    {
        push_x(FAILURE, X_OPERAND);
    }
}


///
///  @brief    Read input from indirect file if one is open.
///
///  @returns  true if have command to execute, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool read_EI(void)
{
    if (ei_old)
    {
        if (ei_stack[0].data == NULL || ei_stack[0].pos == ei_stack[0].len)
        {
            ei_old = false;

            return false;
        }
        else
        {
            cbuf = &ei_stack[0];        // Use our command string

            return true;
        }
    }
    else
    {
        return false;
    }
}


///
///  @brief    Reset indirect command file buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_indirect(void)
{
    ei_old = false;

    while (ei_depth != 0)
    {
        free_mem(&ei_stack[--ei_depth].data);

        ei_stack[ei_depth].size = 0;
        ei_stack[ei_depth].len  = 0;
        ei_stack[ei_depth].pos  = 0;
    }
}


///
///  @brief    Scan EI command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_EI(struct cmd *cmd)
{
    assert(cmd != NULL);

    require_n(cmd->m_set, cmd->n_set);
    reject_dcolon(cmd->dcolon);
    scan_texts(cmd, 1, ESC);

    return false;
}
