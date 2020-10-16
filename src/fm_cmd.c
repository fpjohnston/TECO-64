///
///  @file    fm_cmd.c
///  @brief   Execute FM command.
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

#include "teco.h"
#include "display.h"
#include "errors.h"
#include "exec.h"
#include "term.h"

#if     defined(TECO_DISPLAY)

#include <string.h>
#include <unistd.h>

#include "ascii.h"

#define KEY_F1          (KEY_F(1))      ///< Key definition
#define KEY_F2          (KEY_F(2))      ///< Key definition
#define KEY_F3          (KEY_F(3))      ///< Key definition
#define KEY_F4          (KEY_F(4))      ///< Key definition
#define KEY_F5          (KEY_F(5))      ///< Key definition
#define KEY_F6          (KEY_F(6))      ///< Key definition
#define KEY_F7          (KEY_F(7))      ///< Key definition
#define KEY_F8          (KEY_F(8))      ///< Key definition
#define KEY_F9          (KEY_F(9))      ///< Key definition

#define KEY_S_F1        (KEY_F(1)  + 12) ///< Key definition
#define KEY_S_F2        (KEY_F(2)  + 12) ///< Key definition
#define KEY_S_F3        (KEY_F(3)  + 12) ///< Key definition
#define KEY_S_F4        (KEY_F(4)  + 12) ///< Key definition
#define KEY_S_F5        (KEY_F(5)  + 12) ///< Key definition
#define KEY_S_F6        (KEY_F(6)  + 12) ///< Key definition
#define KEY_S_F7        (KEY_F(7)  + 12) ///< Key definition
#define KEY_S_F8        (KEY_F(8)  + 12) ///< Key definition
#define KEY_S_F9        (KEY_F(9)  + 12) ///< Key definition
    
#define KEY_C_F1        (KEY_F(1)  + 24) ///< Key definition
#define KEY_C_F2        (KEY_F(2)  + 24) ///< Key definition
#define KEY_C_F3        (KEY_F(3)  + 24) ///< Key definition
#define KEY_C_F4        (KEY_F(4)  + 24) ///< Key definition
#define KEY_C_F5        (KEY_F(5)  + 24) ///< Key definition
#define KEY_C_F6        (KEY_F(6)  + 24) ///< Key definition
#define KEY_C_F7        (KEY_F(7)  + 24) ///< Key definition
#define KEY_C_F8        (KEY_F(8)  + 24) ///< Key definition
#define KEY_C_F9        (KEY_F(9)  + 24) ///< Key definition

#define KEY_S_LEFT      KEY_SLEFT       ///< Key definition
#define KEY_C_LEFT      0x221           ///< Key definition
#define KEY_A_LEFT      0x21f           ///< Key definition

#define KEY_S_RIGHT     KEY_SRIGHT      ///< Key definition
#define KEY_C_RIGHT     0x230           ///< Key definition
#define KEY_A_RIGHT     0x22e           ///< Key definition

#define KEY_S_UP        KEY_SR          ///< Key definition
#define KEY_C_UP        0x236           ///< Key definition
#define KEY_A_UP        0x234           ///< Key definition

#define KEY_S_DOWN      KEY_SF          ///< Key definition
#define KEY_C_DOWN      0x20d           ///< Key definition
#define KEY_A_DOWN      0x20b           ///< Key definition

#define KEY_S_HOME      KEY_SHOME       ///< Key definition
#define KEY_C_HOME      0x217           ///< Key definition
#define KEY_A_HOME      0x215           ///< Key definition

#define KEY_S_END       KEY_SEND        ///< Key definition
#define KEY_C_END       0x212           ///< Key definition
#define KEY_A_END       0x210           ///< Key definition

#define KEY_PGUP        KEY_PPAGE       ///< Key definition
#define KEY_S_PGUP      KEY_SPREVIOUS   ///< Key definition
#define KEY_C_PGUP      0x22b           ///< Key definition
#define KEY_A_PGUP      0x229           ///< Key definition

#define KEY_PGDN        KEY_NPAGE       ///< Key definition
#define KEY_S_PGDN      KEY_SNEXT       ///< Key definition
#define KEY_C_PGDN      0x226           ///< Key definition
#define KEY_A_PGDN      0x224           ///< Key definition

#define KEY_DELETE      KEY_DC          ///< Key definition
#define KEY_S_DELETE    KEY_SDC         ///< Key definition
#define KEY_C_DELETE    0x205           ///< Key definition
#define KEY_A_DELETE    0x207           ///< Key definition

#define KEY_INSERT      KEY_IC          ///< Key definition
#define KEY_A_INSERT    0x21A           ///< Key definition

#define KEY_S_TAB       KEY_BTAB        ///< Key definition

/// @def    _
///
/// @brief  Helper macro to set up table of keys.

#define _(key) [KEY_ ## key] = { .kname = #key, .qname = NUL, .qlocal = false }

/// @struct  keys
///
/// @brief   Key-to-Q-register mapping.

struct keys
{
    const char *kname;              ///< Key name
    char qname;                     ///< Mapped Q-register
    bool qlocal;                    ///< true if local Q-register
};

static struct keys keys[] =         ///< List of mappable keys
{
    // Key      Shift + key   Ctrl + key    Alt + key

    _(LEFT),    _(S_LEFT),    _(C_LEFT),    _(A_LEFT),
    _(RIGHT),   _(S_RIGHT),   _(C_RIGHT),   _(A_RIGHT),
    _(UP),      _(S_UP),      _(C_UP),      _(A_UP),
    _(DOWN),    _(S_DOWN),    _(C_DOWN),    _(A_DOWN),
    _(HOME),    _(S_HOME),    _(C_HOME),    _(A_HOME),
    _(END),     _(S_END),     _(C_END),     _(A_END),
    _(PGUP),    _(S_PGUP),    _(C_PGUP),    _(A_PGUP),
    _(PGDN),    _(S_PGDN),    _(C_PGDN),    _(A_PGDN),
    _(DELETE),  _(S_DELETE),  _(C_DELETE),  _(A_DELETE),
    _(INSERT),                              _(A_INSERT),
                _(S_TAB),
    _(F1),      _(S_F1),      _(C_F1),
    _(F2),      _(S_F2),      _(C_F2),
    _(F3),      _(S_F3),      _(C_F3),
    _(F4),      _(S_F4),      _(C_F4),
    _(F5),      _(S_F5),      _(C_F5),
    _(F6),      _(S_F6),      _(C_F6),
    _(F7),      _(S_F7),      _(C_F7),
    _(F8),      _(S_F8),      _(C_F8),
    _(F9),      _(S_F9),      _(C_F9),
};

#endif


///
///  @brief    Execute FM command: map key to Q-register.
///
///             @FMq/key/ - Map key to Q-register.
///            :@FMq/key/ - Unmap key.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(TECO_DISPLAY)

void exec_FM(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    char key[cmd->text1.len + 1];

    sprintf(key, "%.*s", (int)cmd->text1.len, cmd->text1.data);

    for (uint i = 0; i < countof(keys); ++i)
    {
        if (keys[i].kname != NULL && !strcasecmp(key, keys[i].kname))
        {
            if (cmd->colon)
            {
                keys[i].qname  = NUL;
                keys[i].qlocal = false;
            }
            else
            {
                keys[i].qname  = cmd->qname;
                keys[i].qlocal = cmd->qlocal;
            }

            return;
        }
    }

    throw(E_KEY, key);                  // Bad key
}

#else

void exec_FM(struct cmd *unused1)
{
    throw(E_NOD);                       // Display mode support not enabled
}

#endif


///
///  @brief    Check input key and execute any macro it's mapped to.
///
///  @returns  true if key was mapped, else false.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(TECO_DISPLAY)

bool exec_key(int key)
{
    const struct keys *p = &keys[key];

    if ((uint)key < countof(keys) && p->kname != NULL)
    {
        if (p->qname != NUL)
        {
            struct cmd cmd = null_cmd;

            cmd.c1     = 'M';
            cmd.qname  = p->qname;
            cmd.qlocal = p->qlocal;
            cmd.colon  = true;          // Keep local Q-registers

            exec_M(&cmd);

            return true;
        }
    }

    return false;
}

#endif
