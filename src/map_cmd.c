///
///  @file    map_cmd.c
///  @brief   Execute commands that map keys.
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
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "display.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "qreg.h"

#if     defined(DISPLAY_MODE)

#include "keys.h"

#endif

#include "term.h"

#define MAX_CTRL_F      (('9' - '0') + 1) ///< Maximum CTRL/F commands

static char *ctrl_f_cmd[MAX_CTRL_F];    ///< Command strings for CTRL/F

// Local functions

#if     defined(DISPLAY_MODE)

static struct keys *find_key(const char *key);

static void reset_map(void);

static void unmap_key(uint key);

#endif


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
    static int i = 0;                   ///< Last index used

    if (isdigit(c))
    {
        i = c - '0';                    // Set index and default
    }
    else
    {
        assert(c == CTRL_F);
    }

    if (ctrl_f_cmd[i] == NULL || *ctrl_f_cmd[i] == NUL)
    {
        return false;
    }

    tbuffer buf;

    buf.data = ctrl_f_cmd[i];
    buf.size = (uint_t)strlen(ctrl_f_cmd[i]);
    buf.len  = buf.size;
    buf.pos  = 0;

    bool saved_exec = f.e0.exec;

    f.e0.exec = true;                   // Force execution

    exec_macro(&buf, NULL);

    f.e0.exec = saved_exec;

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
            push_x(FAILURE, X_OPERAND); // Command failed

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
        push_x(SUCCESS, X_OPERAND);     // Command succeeded
    }
}


///
///  @brief    Execute FM command: map key to command string, or unmap key.
///
///            @FM/key/cmds/ - Map key to command string.
///            @FM/key//     - Unmap key.
///            @FM///        - Unmap all keys.
///
///            NOTE THAT THIS FUNCTION DOES NOTHING UNLESS DISPLAY MODE WAS
///            INCLUDED IN THE BUILD. ALSO, EXECUTING A COMMAND STRING WITH A
///            KEYCODE WILL NOT WORK UNLESS DISPLAY MODE IS CURRENTLY ACTIVE.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

void exec_FM(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len== 0)             // Unmap all keys?
    {
        reset_map();

        if (cmd->colon)
        {
            push_x(SUCCESS, X_OPERAND); // Command succeeded
        }

        return;
    }

    // Here to map a key to a command string.

    tstring name = build_string(cmd->text1.data, cmd->text1.len);
    struct keys *key = find_key(name.data);

    if (key == NULL)
    {
        if (!cmd->colon)
        {
            throw(E_KEY, name.data);    // Key 'key' not found
        }

        push_x(FAILURE, X_OPERAND);     // Command failed

        return;
    }

    if (cmd->text2.len != 0)
    {
        key->macro = alloc_mem(cmd->text2.len + 1);

        memcpy(key->macro, cmd->text2.data, (size_t)cmd->text2.len);

        key->macro[cmd->text2.len] = NUL;
    }

    if (cmd->colon)
    {
        push_x(SUCCESS, X_OPERAND);     // Command succeeded
    }
}

#else

void exec_FM(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->colon)
    {
        push_x(FAILURE, X_OPERAND);     // Command failed
    }
}

#endif


///
///  @brief    Execute FQ command: map key to Q-register.
///
///            @FQq/key/ - Map key to Q-register.
///            @FQq//    - Ignored (does not unmap).
///
///            NOTE THAT THIS FUNCTION DOES NOTHING UNLESS DISPLAY MODE WAS
///            INCLUDED IN THE BUILD. ALSO, EXECUTING A Q-REGISTER WITH A
///            KEYCODE WILL NOT WORK UNLESS DISPLAY MODE IS CURRENTLY ACTIVE.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

void exec_FQ(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len == 0)
    {
        if (cmd->colon)
        {
            push_x(SUCCESS, X_OPERAND); // Command succeeded
        }

        return;
    }

    tstring name = build_string(cmd->text1.data, cmd->text1.len);
    struct keys *key = find_key(name.data);

    if (key == NULL)
    {
        if (!cmd->colon)
        {
            throw(E_KEY, name.data);    // Key 'key' not found
        }

        push_x(FAILURE, X_OPERAND);     // Command failed

        return;
    }

    key->qname  = cmd->qname;
    key->qlocal = cmd->qlocal;

    if (cmd->colon)
    {
        push_x(SUCCESS, X_OPERAND);     // Command succeeded
    }
}

#else

void exec_FQ(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->colon)
    {
        push_x(FAILURE, X_OPERAND);     // Command failed
    }
}

#endif


///
///  @brief    Check input key and execute anything it's mapped to.
///
///            NOTE THAT THIS FUNCTION ONLY WORKS IF DISPLAY MODE IS CURRENTLY
///            ACTIVE. IF NOT, IT WILL NOT SEE THE CORRECT KEY CODES.
///
///  @returns  true if key was mapped, else false.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

bool exec_key(int key)
{
    struct keys *p;

    if ((uint)key - KEY_MIN < countof(keys))
    {
        p = &keys[key - KEY_MIN];
    }
    else
    {
        return false;
    }

    bool saved_exec = f.e0.exec;

    if (p->macro != NULL)               // Mapped to command string?
    {
        tbuffer buf;

        buf.data = p->macro;
        buf.size = (uint_t)strlen(p->macro);
        buf.len  = buf.size;
        buf.pos  = 0;

        f.e0.exec = true;               // Force execution

        exec_macro(&buf, NULL);
    }
    else if (p->qname != NUL)           // Mapped to Q-register?
    {
        struct cmd cmd = null_cmd;

        cmd.c1     = 'M';
        cmd.qname  = p->qname;
        cmd.qlocal = p->qlocal;
        cmd.colon  = true;              // Keep local Q-registers

        f.e0.exec = true;               // Force execution

        exec_M(&cmd);
    }
    else
    {
        return false;
    }

    f.e0.exec  = saved_exec;            // Restore previous flag

    refresh_dpy();                      // Update display

    return true;
}

#endif


///
///  @brief    Reset everything on exit.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exit_map(void)
{

#if     defined(DISPLAY_MODE)

    reset_map();

#endif

    for (uint i = 0; i < countof(ctrl_f_cmd); ++i)
    {
        free_mem(&ctrl_f_cmd[i]);
    }
}


///
///  @brief    Find mapped key (if already mapped, unmap it so we can remap it).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static struct keys *find_key(const char *key)
{
    for (uint i = 0; i < countof(keys); ++i)
    {
        if (keys[i].kname != NULL && !strcasecmp(key, keys[i].kname))
        {
            unmap_key(i);

            return &keys[i];
        }
    }

    return NULL;
}

#endif


///
///  @brief    Reset all mapped keys.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static void reset_map(void)
{
    for (uint i = 0; i < countof(keys); ++i)
    {
        unmap_key(i);
    }
}

#endif


///
///  @brief    Scan FF command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_FF(struct cmd *cmd)
{
    assert(cmd != NULL);
    reject_m(cmd->m_set);
    require_n(cmd->m_set, cmd->n_set);
    reject_dcolon(cmd->dcolon);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Scan FM command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_FM(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd->m_set);
    reject_n(cmd->n_set);
    reject_dcolon(cmd->dcolon);
    scan_texts(cmd, 2, ESC);

    return false;
}


///
///  @brief    Unmap key.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static void unmap_key(uint key)
{
    free_mem(&keys[key].macro);

    keys[key].qname  = false;
    keys[key].qlocal = false;
}

#endif

