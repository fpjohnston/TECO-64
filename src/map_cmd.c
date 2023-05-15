///
///  @file    map_cmd.c
///  @brief   Execute FM and FQ commands.
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
#include "display.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "keys.h"


/// @def    _
///
/// @brief  Helper macro to set up table of keys.

#define _(key) [KEY_ ## key - KEY_MIN] = \
    { \
        .kname  = #key,  \
        .qname  = NUL,   \
        .qlocal = false, \
        .macro  = NULL,  \
    }

/// @struct  keys
///
/// @brief   Key-to-Q-register mapping.

struct keys
{
    const char *kname;              ///< Key name
    char qname;                     ///< Mapped Q-register
    bool qlocal;                    ///< true if local Q-register
    char *macro;                    ///< Command string
};

static struct keys keys[] =         ///< List of mappable keys
{
    // Key      Shift + key   Ctrl + key    Alt + key

    _(UP),      _(S_UP),      _(C_UP),      _(A_UP),
    _(DOWN),    _(S_DOWN),    _(C_DOWN),    _(A_DOWN),
    _(LEFT),    _(S_LEFT),    _(C_LEFT),    _(A_LEFT),
    _(RIGHT),   _(S_RIGHT),   _(C_RIGHT),   _(A_RIGHT),

    _(HOME),    _(S_HOME),    _(C_HOME),    _(A_HOME),
    _(END),     _(S_END),     _(C_END),     _(A_END),
    _(PGUP),    _(S_PGUP),    _(C_PGUP),    _(A_PGUP),
    _(PGDN),    _(S_PGDN),    _(C_PGDN),    _(A_PGDN),
    _(DELETE),  _(S_DELETE),  _(C_DELETE),  _(A_DELETE),
    _(INSERT),                              _(A_INSERT),

    _(F1),      _(S_F1),      _(C_F1),
    _(F2),      _(S_F2),      _(C_F2),
    _(F3),      _(S_F3),      _(C_F3),
    _(F4),      _(S_F4),      _(C_F4),
    _(F5),      _(S_F5),      _(C_F5),
    _(F6),      _(S_F6),      _(C_F6),
    _(F7),      _(S_F7),      _(C_F7),
    _(F8),      _(S_F8),      _(C_F8),
    _(F9),      _(S_F9),      _(C_F9),
    _(F10),     _(S_F10),     _(C_F10),
    _(F11),     _(S_F11),     _(C_F11),
    _(F12),     _(S_F12),     _(C_F12),
};

#undef _


// Local functions

static struct keys *find_key(const char *key);

static void unmap_key(uint key);


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

void exec_FM(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len== 0)             // Unmap all keys?
    {
        reset_map();

        if (cmd->colon)
        {
            store_val(SUCCESS); // Command succeeded
        }

        return;
    }

    // Here to map a key to a command string.

    const char *name = build_trimmed(cmd->text1.data, cmd->text1.len);
    struct keys *key = find_key(name);

    if (key == NULL)
    {
        if (!cmd->colon)
        {
            throw(E_KEY, name);         // Keyword 'name' not found
        }

        store_val(FAILURE);     // Command failed

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
        store_val(SUCCESS);     // Command succeeded
    }
}


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

void exec_FQ(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len == 0)
    {
        if (cmd->colon)
        {
            store_val(SUCCESS); // Command succeeded
        }

        return;
    }

    const char *name = build_trimmed(cmd->text1.data, cmd->text1.len);
    struct keys *key = find_key(name);

    if (key == NULL)
    {
        if (!cmd->colon)
        {
            throw(E_KEY, name);         // Keyword 'name' not found
        }

        store_val(FAILURE);     // Command failed

        return;
    }

    key->qname  = cmd->qname;
    key->qlocal = cmd->qlocal;

    if (cmd->colon)
    {
        store_val(SUCCESS);     // Command succeeded
    }
}


///
///  @brief    Check soft key and execute anything it's mapped to.
///
///            NOTE THAT THIS FUNCTION ONLY WORKS IF DISPLAY MODE IS CURRENTLY
///            ACTIVE. IF NOT, IT WILL NOT SEE THE CORRECT KEY CODES.
///
///  @returns  true if key was mapped, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool exec_soft(int key)
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

    if (p->macro != NULL)               // Mapped to command string?
    {
        exec_str(p->macro);

        return true;
    }
    else if (p->qname != NUL)           // Mapped to Q-register?
    {
        char cmd[4];                    // Room for "M.A" (or similar command)

        snprintf(cmd, sizeof(cmd), "M%s%c", p->qlocal ? "." : "", p->qname);

        exec_str(cmd);

        return true;
    }
    else
    {
        return false;
    }
}


///
///  @brief    Find mapped key (if already mapped, unmap it so we can remap it).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

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


///
///  @brief    Reset all mapped keys.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_map(void)
{
    for (uint i = 0; i < countof(keys); ++i)
    {
        unmap_key(i);
    }
}


///
///  @brief    Unmap key.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void unmap_key(uint key)
{
    free_mem(&keys[key].macro);

    keys[key].qname  = false;
    keys[key].qlocal = false;
}
