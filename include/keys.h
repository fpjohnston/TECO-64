///
///  @file    keys.h
///  @brief   Header file for user-definable keys.
///
///  @copyright 2020-2023 Franklin P. Johnston / Nowwith Treble Software
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

#if     !defined(_KEYS_H)

#define _KEYS_H

#include <ncurses.h>

// NOTE: some key codes changed when upgrading to Ubuntu 21.04, which may have
// involved consequent changes to the C library, ncurses, or other components.
// Bottom line: not sure what changed or why. All of the changes involved keypad
// characters combined with either Ctrl or Alt (e.g., Ctrl-Home, or Alt-Delete)
// which aren't explicitly defined in ncurses.h. All of the changed values are
// 1 higher than before. So the following offset is employed to facilitate
// reverting to the older values if needed with builds on other systems.

//lint -save -e835

#define OFFSET      1               ///< Modify as needed

//  @enum
//
//  @brief  Define the keys that can be mapped.

enum
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
    KEY_F10      = (KEY_F(10)),
    KEY_F11      = (KEY_F(11)),
    KEY_F12      = (KEY_F(12)),

    KEY_S_F1     = (KEY_F(1)  + 12),
    KEY_S_F2     = (KEY_F(2)  + 12),
    KEY_S_F3     = (KEY_F(3)  + 12),
    KEY_S_F4     = (KEY_F(4)  + 12),
    KEY_S_F5     = (KEY_F(5)  + 12),
    KEY_S_F6     = (KEY_F(6)  + 12),
    KEY_S_F7     = (KEY_F(7)  + 12),
    KEY_S_F8     = (KEY_F(8)  + 12),
    KEY_S_F9     = (KEY_F(9)  + 12),
    KEY_S_F10    = (KEY_F(10) + 12),
    KEY_S_F11    = (KEY_F(11) + 12),
    KEY_S_F12    = (KEY_F(12) + 12),

    KEY_C_F1     = (KEY_F(1)  + 24),
    KEY_C_F2     = (KEY_F(2)  + 24),
    KEY_C_F3     = (KEY_F(3)  + 24),
    KEY_C_F4     = (KEY_F(4)  + 24),
    KEY_C_F5     = (KEY_F(5)  + 24),
    KEY_C_F6     = (KEY_F(6)  + 24),
    KEY_C_F7     = (KEY_F(7)  + 24),
    KEY_C_F8     = (KEY_F(8)  + 24),
    KEY_C_F9     = (KEY_F(9)  + 24),
    KEY_C_F10    = (KEY_F(10) + 24),
    KEY_C_F11    = (KEY_F(11) + 24),
    KEY_C_F12    = (KEY_F(12) + 24),

    KEY_S_UP     = KEY_SR,
    KEY_S_DOWN   = KEY_SF,
    KEY_S_LEFT   = KEY_SLEFT,
    KEY_S_RIGHT  = KEY_SRIGHT,

    KEY_C_UP     = 566 + OFFSET,
    KEY_C_DOWN   = 525 + OFFSET,
    KEY_C_LEFT   = 545 + OFFSET,
    KEY_C_RIGHT  = 560 + OFFSET,

    KEY_A_UP     = 564 + OFFSET,
    KEY_A_DOWN   = 523 + OFFSET,
    KEY_A_LEFT   = 543 + OFFSET,
    KEY_A_RIGHT  = 558 + OFFSET,

    KEY_PGUP     = KEY_PPAGE,
    KEY_PGDN     = KEY_NPAGE,
    KEY_INSERT   = KEY_IC,
    KEY_DELETE   = KEY_DC,

    KEY_S_PGUP   = KEY_SPREVIOUS,
    KEY_S_PGDN   = KEY_SNEXT,
    KEY_S_HOME   = KEY_SHOME,
    KEY_S_END    = KEY_SEND,
    KEY_S_DELETE = KEY_SDC,

    KEY_C_HOME   = 535 + OFFSET,
    KEY_C_END    = 530 + OFFSET,
    KEY_C_DELETE = 519 + OFFSET,
    KEY_C_PGUP   = 555 + OFFSET,
    KEY_C_PGDN   = 550 + OFFSET,

    KEY_A_HOME   = 533 + OFFSET,
    KEY_A_END    = 528 + OFFSET,
    KEY_A_INSERT = 538 + OFFSET,
    KEY_A_DELETE = 517 + OFFSET,
    KEY_A_PGUP   = 553 + OFFSET,
    KEY_A_PGDN   = 548 + OFFSET
};

//lint -restore

#endif  // !defined(_KEYS_H)
