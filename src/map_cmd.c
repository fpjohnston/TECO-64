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
#include "keys.h"
#include "term.h"


static bool exit_set = false;           ///< Flag for setting exit function

// Local functions

static void exit_map(void);

static void reset_map(void);

static void unmap_key(uint key);


///
///  @brief    Execute "FM" command: map key to command string, or unmap key.
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

    if (!exit_set)
    {
        exit_set = true;

        register_exit(exit_map);
    }

    uint size = cmd->text1.len;

    if (size == 0)                      // Unmap all keys?
    {
        reset_map();

        return;
    }

    // Here to map a key to a command string.

    char *temp;
    uint len = build_string(&temp, cmd->text1.data, size);
    char key[len + 1];

    strcpy(key, temp);
    free_mem(&temp);

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
///  @brief    Execute "FQ" command: map key to Q-register.
///
///            @FQq/key/ - Map key to Q-register.
///            @FQq//    - Ignored (does not unmap).
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

    char *temp;
    uint len = build_string(&temp, cmd->text1.data, cmd->text1.len);
    char key[len + 1];

    strcpy(key, temp);
    free_mem(&temp);

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
///  @brief    Reset everything on exit.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exit_map(void)
{
    reset_map();
}


///
///  @brief    Reset all mapped keys.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void reset_map(void)
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
    keys[key].colon  = false;
}

