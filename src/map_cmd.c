///
///  @file    map_cmd.c
///  @brief   Execute commands that map keys.
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "display.h"
#include "errcodes.h"
#include "exec.h"
#include "term.h"

///  @enum   mapkeys
///
///  @brief  Define the keys that can be mapped.

enum mapkeys
{
    KEY_F1       = (KEY_F(1)),
    KEY_F2       = (KEY_F(2)),
    KEY_F3       = (KEY_F(3)),
    KEY_F4       = (KEY_F(4)),
    KEY_F5       = (KEY_F(5)),
    KEY_F6       = (KEY_F(6)),
    KEY_F7       = (KEY_F(7)),
    KEY_F8       = (KEY_F(8)),
    KEY_F9       = (KEY_F(9)),

    KEY_S_F1     = (KEY_F(1)  + 12),
    KEY_S_F2     = (KEY_F(2)  + 12),
    KEY_S_F3     = (KEY_F(3)  + 12),
    KEY_S_F4     = (KEY_F(4)  + 12),
    KEY_S_F5     = (KEY_F(5)  + 12),
    KEY_S_F6     = (KEY_F(6)  + 12),
    KEY_S_F7     = (KEY_F(7)  + 12),
    KEY_S_F8     = (KEY_F(8)  + 12),
    KEY_S_F9     = (KEY_F(9)  + 12),
    
    KEY_S_LEFT   = KEY_SLEFT,
    KEY_S_RIGHT  = KEY_SRIGHT,
    KEY_S_UP     = KEY_SR,
    KEY_S_DOWN   = KEY_SF,
    KEY_S_HOME   = KEY_SHOME,
    KEY_S_END    = KEY_SEND,
    KEY_PGUP     = KEY_PPAGE,
    KEY_S_PGUP   = KEY_SPREVIOUS,
    KEY_PGDN     = KEY_NPAGE,
    KEY_S_PGDN   = KEY_SNEXT,
    KEY_DELETE   = KEY_DC,
    KEY_S_DELETE = KEY_SDC,
    KEY_INSERT   = KEY_IC
};


/// @def    _
///
/// @brief  Helper macro to set up table of keys.

#define _(key) [KEY_ ## key] = { .kname = #key, .qname = NUL, .qlocal = false, \
                                 .macro = NULL, .colon = false }

/// @struct  keys
///
/// @brief   Key-to-Q-register mapping.

struct keys
{
    const char *kname;              ///< Key name
    char qname;                     ///< Mapped Q-register
    bool qlocal;                    ///< true if local Q-register
    char *macro;                    ///< Command string
    bool colon;                     ///< Command was :FM or :FQ
};

static struct keys keys[] =         ///< List of mappable keys
{
    // Key      Shift + key

    _(LEFT),    _(S_LEFT),
    _(RIGHT),   _(S_RIGHT),
    _(UP),      _(S_UP),
    _(DOWN),    _(S_DOWN),
    _(HOME),    _(S_HOME),
    _(END),     _(S_END),
    _(PGUP),    _(S_PGUP),
    _(PGDN),    _(S_PGDN),
    _(DELETE),  _(S_DELETE),
    _(INSERT),
    _(F1),      _(S_F1),
    _(F2),      _(S_F2),
    _(F3),      _(S_F3),
    _(F4),      _(S_F4),
    _(F5),      _(S_F5),
    _(F6),      _(S_F6),
    _(F7),      _(S_F7),
    _(F8),      _(S_F8),
    _(F9),      _(S_F9),
};


// Local functions

static void unmap_key(uint key);


///
///  @brief    Execute FM command: map key to command string, or unmap key.
///
///            @FM/key/cmds/ - Map key to command string.
///            @FM/key//     - Unmap key.
///            @FM///        - Unmap all keys.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_FM(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    uint size = cmd->text1.len;

    if (size == 0)                      // Unmap all keys?
    {
        for (uint i = 0; i < countof(keys); ++i)
        {
            unmap_key(i);
        }

        return;
    }

    // Here to map a key to a command string.

    char key[size + 1];

    sprintf(key, "%.*s", (int)size, cmd->text1.data);

    for (uint i = 0; i < countof(keys); ++i)
    {
        if (keys[i].kname != NULL && !strcasecmp(key, keys[i].kname))
        {
            unmap_key(i);

            if (cmd->text2.len != 0)
            {
                size = cmd->text2.len;

                keys[i].colon = cmd->colon;
                keys[i].macro = alloc_mem(size + 1);

                memcpy(keys[i].macro, cmd->text2.data, (size_t)size);

                keys[i].macro[size] = NUL;
            }

            return;
        }
    }

    throw(E_KEY, key);                  // Key 'key' not found
}


///
///  @brief    Execute FQ command: map key to Q-register.
///
///             @FQq/key/ - Map key to Q-register.
///             @FQq//    - Ignored (does not unmap).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_FQ(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->text1.len == 0)
    {
        return;
    }

    char key[cmd->text1.len + 1];

    sprintf(key, "%.*s", (int)cmd->text1.len, cmd->text1.data);

    for (uint i = 0; i < countof(keys); ++i)
    {
        if (keys[i].kname != NULL && !strcasecmp(key, keys[i].kname))
        {
            unmap_key(i);

            keys[i].colon  = cmd->colon;
            keys[i].qname  = cmd->qname;
            keys[i].qlocal = cmd->qlocal;
            
            return;
        }
    }

    throw(E_KEY, key);                  // Key 'key' not found
}


///
///  @brief    Check input key and execute anything it's mapped to.
///
///  @returns  true if key was mapped, else false.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(TECO_DISPLAY)

bool exec_key(int key)
{
    struct keys *p = &keys[key];

    if ((uint)key < countof(keys) && p->kname != NULL)
    {
        if (p->macro != NULL)           // Mapped to command string?
        {
            struct buffer buf;

            buf.data = p->macro;
            buf.size = (uint)strlen(p->macro);
            buf.len  = buf.size;
            buf.pos  = 0;

            exec_macro(&buf, NULL);
        }
        else if (p->qname != NUL)       // Mapped to Q-register?
        {
            struct cmd cmd = null_cmd;

            cmd.c1     = 'M';
            cmd.qname  = p->qname;
            cmd.qlocal = p->qlocal;
            cmd.colon  = true;          // Keep local Q-registers

            exec_M(&cmd);
        }
        else
        {
            return false;
        }

        if (!p->colon)                  // No refresh if :FM or :FQ
        {
            refresh_dpy();
        }

        return true;
    }

    return false;
}

#endif


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
    keys[key].colon  = false;
}

