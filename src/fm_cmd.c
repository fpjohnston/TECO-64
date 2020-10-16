///
///  @file    fq_cmd.c
///  @brief   Execute FQ command.
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
#define KEY_F10         (KEY_F(10))     ///< Key definition
#define KEY_F11         (KEY_F(11))     ///< Key definition
#define KEY_F12         (KEY_F(12))     ///< Key definition

#define KEY_SF1         (KEY_F(1)  + 12) ///< Key definition
#define KEY_SF2         (KEY_F(2)  + 12) ///< Key definition
#define KEY_SF3         (KEY_F(3)  + 12) ///< Key definition
#define KEY_SF4         (KEY_F(4)  + 12) ///< Key definition
#define KEY_SF5         (KEY_F(5)  + 12) ///< Key definition
#define KEY_SF6         (KEY_F(6)  + 12) ///< Key definition
#define KEY_SF7         (KEY_F(7)  + 12) ///< Key definition
#define KEY_SF8         (KEY_F(8)  + 12) ///< Key definition
#define KEY_SF9         (KEY_F(9)  + 12) ///< Key definition
#define KEY_SF10        (KEY_F(10) + 12) ///< Key definition
#define KEY_SF11        (KEY_F(11) + 12) ///< Key definition
#define KEY_SF12        (KEY_F(12) + 12) ///< Key definition
    
#define KEY_CF1         (KEY_F(1)  + 24) ///< Key definition
#define KEY_CF2         (KEY_F(2)  + 24) ///< Key definition
#define KEY_CF3         (KEY_F(3)  + 24) ///< Key definition
#define KEY_CF4         (KEY_F(4)  + 24) ///< Key definition
#define KEY_CF5         (KEY_F(5)  + 24) ///< Key definition
#define KEY_CF6         (KEY_F(6)  + 24) ///< Key definition
#define KEY_CF7         (KEY_F(7)  + 24) ///< Key definition
#define KEY_CF8         (KEY_F(8)  + 24) ///< Key definition
#define KEY_CF9         (KEY_F(9)  + 24) ///< Key definition
#define KEY_CF10        (KEY_F(10) + 24) ///< Key definition
#define KEY_CF11        (KEY_F(11) + 24) ///< Key definition
#define KEY_CF12        (KEY_F(12) + 24) ///< Key definition

#define KEY_CSF1        (KEY_F(1)  + 36) ///< Key definition
#define KEY_CSF2        (KEY_F(2)  + 36) ///< Key definition
#define KEY_CSF3        (KEY_F(3)  + 36) ///< Key definition
#define KEY_CSF4        (KEY_F(4)  + 36) ///< Key definition
#define KEY_CSF5        (KEY_F(5)  + 36) ///< Key definition
#define KEY_CSF6        (KEY_F(6)  + 36) ///< Key definition
#define KEY_CSF7        (KEY_F(7)  + 36) ///< Key definition
#define KEY_CSF8        (KEY_F(8)  + 36) ///< Key definition
#define KEY_CSF9        (KEY_F(9)  + 36) ///< Key definition
#define KEY_CSF10       (KEY_F(10) + 36) ///< Key definition
#define KEY_CSF11       (KEY_F(11) + 36) ///< Key definition
#define KEY_CSF12       (KEY_F(12) + 36) ///< Key definition

/// @def    _
///
/// @brief  Helper macro to set up table of keys.

#define _(key) [KEY_ ## key] = { .kname = #key, .qname = NUL }

/// @struct  keys
///
/// @brief   Key-to-Q-register mapping.

struct keys
{
    const char *kname;                  ///< Key name
    char qname;                         ///< Mapped Q-register
};

static struct keys keys[] =             ///< List of mappable keys
{
    _(BREAK),
    _(SRESET),
    _(RESET),
    _(DOWN),
    _(UP),
    _(LEFT),
    _(RIGHT),
    _(HOME),
    _(BACKSPACE),
    _(F0),
    _(F1),
    _(F2),
    _(F3),
    _(F4),
    _(F5),
    _(F6),
    _(F7),
    _(F8),
    _(F9),
    _(F10),
    _(F11),
    _(F12),
    _(SF1),
    _(SF2),
    _(SF3),
    _(SF4),
    _(SF5),
    _(SF6),
    _(SF7),
    _(SF8),
    _(SF9),
    _(SF10),
    _(SF11),
    _(SF12),
    _(CF1),
    _(CF2),
    _(CF3),
    _(CF4),
    _(CF5),
    _(CF6),
    _(CF7),
    _(CF8),
    _(CF9),
    _(CF10),
    _(CF11),
    _(CF12),
    _(CSF1),
    _(CSF2),
    _(CSF3),
    _(CSF4),
    _(CSF5),
    _(CSF6),
    _(CSF7),
    _(CSF8),
    _(CSF9),
    _(CSF10),
    _(CSF11),
    _(CSF12),
    _(DL),
    _(IL),
    _(DC),
    _(IC),
    _(EIC),
    _(CLEAR),
    _(EOS),
    _(EOL),
    _(SF),
    _(SR),
    _(NPAGE),
    _(PPAGE),
    _(STAB),
    _(CTAB),
    _(CATAB),
    _(ENTER),
    _(PRINT),
    _(LL),
    _(A1),
    _(A3),
    _(B2),
    _(C1),
    _(C3),
    _(BTAB),
    _(BEG),
    _(CANCEL),
    _(CLOSE),
    _(COMMAND),
    _(COPY),
    _(CREATE),
    _(END),
    _(EXIT),
    _(FIND),
    _(HELP),
    _(MARK),
    _(MESSAGE),
    _(MOVE),
    _(NEXT),
    _(OPEN),
    _(OPTIONS),
    _(PREVIOUS),
    _(REDO),
    _(REFERENCE),
    _(REFRESH),
    _(REPLACE),
    _(RESTART),
    _(RESUME),
    _(SAVE),
    _(SBEG),
    _(SCANCEL),
    _(SCOMMAND),
    _(SCOPY),
    _(SCREATE),
    _(SDC),
    _(SDL),
    _(SELECT),
    _(SEND),
    _(SEOL),
    _(SEXIT),
    _(SFIND),
    _(SHELP),
    _(SHOME),
    _(SIC),
    _(SLEFT),
    _(SMESSAGE),
    _(SMOVE),
    _(SNEXT),
    _(SOPTIONS),
    _(SPREVIOUS),
    _(SPRINT),
    _(SREDO),
    _(SREPLACE),
    _(SRIGHT),
    _(SRSUME),
    _(SSAVE),
    _(SSUSPEND),
    _(SUNDO),
    _(SUSPEND),
    _(UNDO),
    _(MOUSE),
    _(RESIZE),
    _(EVENT),
};

#endif


///
///  @brief    Execute FQ command: map key to Q-register (global only).
///
///             @FQq/key/ - Map key to Q-register.
///            :@FQq/key/ - Unmap key.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(TECO_DISPLAY)

void exec_FQ(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->qlocal)                    // Local Q-register?
    {
        throw(E_IQN, '.');              // Yes, that's an error
    }

    char key[cmd->text1.len + 1];

    sprintf(key, "%.*s", (int)cmd->text1.len, cmd->text1.data);

    for (uint i = 0; i < countof(keys); ++i)
    {
        if (keys[i].kname != NULL && !strcasecmp(key, keys[i].kname))
        {
            if (cmd->colon)
            {
                keys[i].qname  = NUL;
            }
            else
            {
                keys[i].qname  = cmd->qname;
            }

            return;
        }
    }

    throw(E_KEY, key);                  // Bad key
}

#else

void exec_FQ(struct cmd *unused1)
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
            struct cmd cmd;

            memset(&cmd, NUL, sizeof(cmd));

            cmd.c1 = 'M';
            cmd.qname = p->qname;

            exec_M(&cmd);

            return true;
        }
    }

    return false;
}

#endif
