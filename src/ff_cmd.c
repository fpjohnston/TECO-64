///
///  @file    ff_cmd.c
///  @brief   Execute FF and CTRL/F commands.
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
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "estack.h"
#include "exec.h"


#define MAX_CTRL_F      (('9' - '0') + 1) ///< Maximum CTRL/F commands

static char *ctrl_f_cmd[MAX_CTRL_F];    ///< Command strings for CTRL/F


///
///  @brief    Execute CTRL/F command. This may take one of two forms:
///
///            <CTRL_F>x        - Execute command string for command string 'x';
///                               'x' may range from '0' to '9'.
///            <CTRL_F><CTRL_F> - Repeats last CTRL/F<digit>.
///
///  @returns  true if executed command, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool exec_ctrl_F(int c)
{
    static int saved_index = 0;         ///< Last index used

    if (isdigit(c))
    {
        saved_index = c - '0';          // Set index and default
    }
    else
    {
        assert(c == CTRL_F);
    }

#if     defined(DEBUG)

    if (saved_index == 0 && ctrl_f_cmd[0] == NULL)
    {
        const char *p = "EK HK EX";

        ctrl_f_cmd[0] = alloc_mem((uint_t)strlen(p) + 1);

        strcpy(ctrl_f_cmd[0], p);
    }

#endif

    if (ctrl_f_cmd[saved_index] == NULL || *ctrl_f_cmd[saved_index] == NUL)
    {
        return false;
    }

    tbuffer buf;

    buf.data = ctrl_f_cmd[saved_index];
    buf.size = (uint_t)strlen(ctrl_f_cmd[saved_index]);
    buf.len  = buf.size;
    buf.pos  = 0;

    bool exec = f.e0.exec;

    f.e0.exec = true;

    exec_macro(&buf, NULL);

    f.e0.exec = exec;

    return true;
}


///
///  @brief    Execute FF command: map or unmap Ctrl/F to command string.
///
///            @FF/cmds/ - Map CTRL/F to command string.
///            @FF//     - Unmap key.
///
///            THIS COMMAND IS EXPERIMENTAL, AND IS INTENDED FOR TESTING AND
///            DEBUGGING PURPOSES. ITS USE IS NOT DESCRIBED IN THE MARKDOWN
///            DOCUMENTATION, AS IT MAY BE DELETED OR CHANGED AT ANY TIME, AND
///            NO ASSUMPTION SHOULD BE MADE ABOUT ITS FORMAT OR FUNCTIONALITY.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_FF(struct cmd *cmd)
{
    assert(cmd != NULL);

    int_t i = 0;                        // Index into CTRL/F array

    if (cmd->n_set && ((i = cmd->n_arg - '0') < 0 || i > 9))
    {
        if (cmd->colon)
        {
            store_val(FAILURE); // Command failed

            return;
        }

        throw(E_INA);
    }

    free_mem(&ctrl_f_cmd[i]);           // Free existing command string

    if (cmd->text1.len != 0)
    {
        // Here to map CTRL/F to a command string.

        tstring key = build_string(cmd->text1.data, cmd->text1.len);

        ctrl_f_cmd[i] = alloc_mem(key.len + 1);

        strcpy(ctrl_f_cmd[i], key.data);
    }

    if (cmd->colon)
    {
        store_val(SUCCESS);     // Command succeeded
    }
}


///
///  @brief    Free up memory allocated for FF, FM, and FQ commands.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exit_map(void)
{
    reset_map();

    for (uint i = 0; i < countof(ctrl_f_cmd); ++i)
    {
        free_mem(&ctrl_f_cmd[i]);
    }
}


///
///  @brief    Scan FF command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_FF(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_M, NO_M_ONLY, NO_DCOLON);

    scan_texts(cmd, 1, ESC);

    return false;
}
