///
///  @file    ascii.h
///  @brief   Definition of ASCII characters for TECO.
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

#if     !defined(_ASCII_H)

#define _ASCII_H

///  @enum   ascii
///  @brief  Definitions of special ASCII characters.

enum ascii
{
    NL = -1,                            ///< Newline (LF or CR/LF)
    NUL,
    CTRL_A,
    CTRL_B,
    CTRL_C,
    CTRL_D,
    CTRL_E,
    CTRL_F,
    CTRL_G,
    BEL     = CTRL_G,                   ///< Used for ringing bell on terminal
    CTRL_H,
    BS      = CTRL_H,
    CTRL_I,
    HT      = CTRL_I,
    TAB     = CTRL_I,
    CTRL_J,
    LF      = CTRL_J,
    CTRL_K,
    VT      = CTRL_K,
    CTRL_L,
    FF      = CTRL_L,
    CTRL_M,
    CR      = CTRL_M,
    CTRL_N,
    CTRL_O,
    CTRL_P,
    CTRL_Q,
    CTRL_R,
    CTRL_S,
    CTRL_T,
    CTRL_U,
    CTRL_V,
    CTRL_W,
    CTRL_X,
    CTRL_Y,
    CTRL_Z,
    ESC,
    FS,
    GS,
    RS,
    US,
    SPACE   = ' ',                      ///< Space character
    ACCENT  = '`',                      ///< Accent grave
    DEL     = 127                       ///< DELETE/RUBOUT
};

#endif  // !defined(_ASCII_H)
