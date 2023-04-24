///
///  @file    ei_cmd.c
///  @brief   Execute EI command.
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
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "cbuf.h"
#include "eflags.h"
#include "estack.h"
#include "exec.h"
#include "file.h"


// The following variables are only used for "classic" EI commands. See
// description for exec_EI() for details.

static tbuffer ei_primary;              ///< Primary EI command buffer

static tbuffer ei_secondary;            ///< Secondary EI command buffer

static tbuffer *ei_command = NULL;      ///< Current EI command buffer


///
///  @brief    Execute EI command: read TECO indirect command file. This can be
///            handled in one of two ways:
///
///            1. Execute indirect command file data after execution of current
///               command has completed. This is how DEC TECOs behaved.
///
///            2. Execute indirect command file as though it were a macro loaded
///               in a Q-register. This behavior is new to TECO-64, and allows
///               immediate execution, as well as the ability to nest EI com-
///               mands. Basically, it treats the EI command as a subroutine
///               call.
///
///            The second option described above is enabled by a bit in the E1
///            flag, which is enabled by default.
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
    struct ifile *ifile;

    close_input(stream);                // Close any open file

    if (f.e1.eimacro)
    {
        if (len == 0)
        {
            cbuf->pos = cbuf->len;

            if (cmd->colon)
            {
                store_val(SUCCESS);
            }

            return;
        }

        if ((name = init_filename(name, len, cmd->colon)) != NULL)
        {
            uint_t size;

            if ((ifile = open_command(name, stream, cmd->colon, &size)) != NULL)
            {
                char ei_data[size + 1];
                tbuffer ei_macro =
                    { .len = 0, .pos = 0, .size = size, .data = ei_data };

                read_command(ifile, stream, &ei_macro);

                if (cmd->colon)
                {
                    store_val(SUCCESS);
                }

                if (ei_macro.size != 0)
                {
                    exec_macro(&ei_macro, cmd);
                }

                return;
            }
        }
    }
    else                                // Classic EI
    {
        if (len == 0)                   // @EI//?
        {
            longjmp(jump_main, MAIN_CTRLC); // Yes, act like CTRL/C typed
        }

        if ((name = init_filename(name, len, cmd->colon)) != NULL)
        {
            ei_command = (ei_command == &ei_secondary) ? &ei_primary : &ei_secondary;

            free_mem(&ei_command->data); // Free up previous data

            if ((ifile = open_command(name, stream, cmd->colon, &ei_command->size)) != NULL)
            {
                read_command(ifile, stream, ei_command);

                if (cmd->colon)
                {
                    store_val(SUCCESS);
                }

                return;
            }
        }
    }

    if (cmd->colon)
    {
        store_val(FAILURE);
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
    if (ei_command == NULL)             // Executing "classic" EI command?
    {
        return false;                   // No, so we're done
    }

    if (ei_command->pos == ei_command->len) // Used up all data in current buffer?
    {
        free_mem(&ei_command->data);    // Yes, free it up

        ei_command = (ei_command == &ei_secondary) ? &ei_primary : &ei_secondary;

        if (ei_command->pos == ei_command->len)
        {
            ei_command = NULL;

            return false;
        }
    }

    cbuf = ei_command;                  // Set command string using our buffer

    return true;
}


///
///  @brief    Reset indirect command file buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_indirect(void)
{
    free_mem(&ei_primary.data);
    free_mem(&ei_secondary.data);

    ei_primary.len = 0;
    ei_primary.pos = 0;
    ei_secondary.len = 0;
    ei_secondary.pos = 0;
    ei_command = NULL;
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
