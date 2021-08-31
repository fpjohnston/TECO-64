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


///  @var    ei_old
///  @brief  Buffer for old-style EI commands

static tbuffer ei_old;

///   @var    ei_depth
///   @brief  Nesting depth for new-style EI commands.

static uint ei_depth = 0;


///
///  @brief    Check to see if any EI commands are active.
///
///  @returns  +1 -> New-style EI command is active.
///            -1 -> Old-style EI command is active.
///             0 -> No EI command is active.
///
////////////////////////////////////////////////////////////////////////////////

int check_EI(void)
{
    if (ei_depth != 0)
    {
        return 1;
    }
    else if (cbuf == &ei_old)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}


///
///  @brief    Execute "EI" command: read TECO command file. This is handled in
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

    if (f.e0.init || f.e1.new_ei)       // New-style EI
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
            tbuffer ei_new;

            if (open_command(name, stream, cmd->colon, &ei_new))
            {
                if (cmd->colon)
                {
                    push_x(SUCCESS, X_OPERAND);
                }

                if (ei_new.size != 0)
                {
                    ++ei_depth;

                    exec_macro(&ei_new, cmd);

                    free_mem(&ei_new.data);

                    --ei_depth;
                }

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
            if (open_command(name, stream, cmd->colon, &ei_old))
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
    if (ei_old.data == NULL || ei_old.pos == ei_old.len)
    {
        return false;
    }
    else
    {
        cbuf = &ei_old;                 // Use our command string

        return true;
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
    ei_old.size = 0;
    ei_old.len  = 0;
    ei_old.pos  = 0;

    free_mem(&ei_old.data);

    ei_depth = 0;
}


///
///  @brief    Scan "EI" command.
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
