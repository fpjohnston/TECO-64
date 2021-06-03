///
///  @file    keys.h
///  @brief   Header file for user-definable keys.
///
///  @copyright 2020-2021 Franklin P. Johnston / Nowwith Treble Software
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

#include <stdbool.h>                //lint !e537


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

    KEY_C_UP     = 566,
    KEY_C_DOWN   = 525,
    KEY_C_LEFT   = 545,
    KEY_C_RIGHT  = 560,

    KEY_A_UP     = 564,
    KEY_A_DOWN   = 523,
    KEY_A_LEFT   = 543,
    KEY_A_RIGHT  = 558,

    KEY_PGUP     = KEY_PPAGE,
    KEY_PGDN     = KEY_NPAGE,
    KEY_INSERT   = KEY_IC,
    KEY_DELETE   = KEY_DC,

    KEY_S_PGUP   = KEY_SPREVIOUS,
    KEY_S_PGDN   = KEY_SNEXT,
    KEY_S_HOME   = KEY_SHOME,
    KEY_S_END    = KEY_SEND,
    KEY_S_DELETE = KEY_SDC,

    KEY_C_HOME   = 535,
    KEY_C_END    = 530,
    KEY_C_DELETE = 519,

    KEY_A_HOME   = 533,
    KEY_A_END    = 528,
    KEY_A_INSERT = 538,
    KEY_A_DELETE = 517,
    KEY_A_PGUP   = 553,
    KEY_A_PGDN   = 548
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
    // Key      Shift + key   Ctrl + key    Alt + key

    _(UP),      _(S_UP),      _(C_UP),      _(A_UP),
    _(DOWN),    _(S_DOWN),    _(C_DOWN),    _(A_DOWN),
    _(LEFT),    _(S_LEFT),    _(C_LEFT),    _(A_LEFT),
    _(RIGHT),   _(S_RIGHT),   _(C_RIGHT),   _(A_RIGHT),

    _(HOME),    _(S_HOME),    _(C_HOME),    _(A_HOME),
    _(END),     _(S_END),     _(C_END),     _(A_END),
    _(PGUP),    _(S_PGUP),                  _(A_PGUP),
    _(PGDN),    _(S_PGDN),                  _(A_PGDN),
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
                              _(C_F10),
                _(S_F11),     _(C_F11),
    _(F12),     _(S_F12),     _(C_F12),
};

#undef _

#endif  // !defined(_KEYS_H)
