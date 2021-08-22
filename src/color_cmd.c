///
///  @file    color_cmd.c
///  @brief   Execute commands that set display colors (F1, F2, F3).
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
#include <stdio.h>
#include <stdlib.h>

#if     defined(DISPLAY_MODE)

#include <ncurses.h>
#include <string.h>

#endif

#include "teco.h"
#include "display.h"
#include "exec.h"


#if     defined(DISPLAY_MODE)


#define COLOR_BASE  16              ///< Starting base for new colors

///
///  @struct color_table
///
///  @brief  Table of colors
///

struct color_table
{
    const char *name;               ///< Name of color
    uint red;                       ///< Red saturation (0-1000)
    uint green;                     ///< Green saturation (0-1000)
    uint blue;                      ///< Blue saturation (0-1000)
};

///
///  @var    color_table
///
///  @brief  Table of colors
///

static const struct color_table color_table[] =
{
    [COLOR_BLACK]   = { "BLACK",        0,      0,      0, },
    [COLOR_RED]     = { "RED",     SATMAX,      0,      0, },
    [COLOR_GREEN]   = { "GREEN",        0, SATMAX,      0, },
    [COLOR_YELLOW]  = { "YELLOW",  SATMAX, SATMAX,      0, },
    [COLOR_BLUE]    = { "BLUE",         0,      0, SATMAX, },
    [COLOR_MAGENTA] = { "MAGENTA", SATMAX,      0, SATMAX, },
    [COLOR_CYAN]    = { "CYAN",         0, SATMAX, SATMAX, },
    [COLOR_WHITE]   = { "WHITE",   SATMAX, SATMAX, SATMAX, },
};


// Local functions

static int find_color(const char *token);

static void set_color(const char *buf, uint_t len, int_t sat, short color);

static void set_colors(const struct cmd *cmd, enum region_pair pair);

#else

static void set_colors(void *unused1, int unused2);

#endif


///
///  @brief    Execute "F1" command: set colors for command region.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F1(struct cmd *cmd)
{
    set_colors(cmd, CMD);
}


///
///  @brief    Execute "F2" command: set colors for edit region.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F2(struct cmd *cmd)
{
    set_colors(cmd, EDIT);
}


///
///  @brief    Execute "F3" command: set colors for status line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F3(struct cmd *cmd)
{
    set_colors(cmd, STATUS);
}


///
///  @brief    Find color specified by string.
///
///  @returns  Index into table, or -1 if no match.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static int find_color(const char *token)
{
    if (token == NULL)
    {
        return -1;
    }

    for (int i = 0; i < (int)countof(color_table); ++i)
    {
        if (!strcasecmp(token, color_table[i].name))
        {
            return i;
        }
    }

    return -1;
}

#endif


///
///  @brief    Initialize saturation levels for a specified color.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static void set_color(const char *buf, uint_t len, int_t sat, short color)
{
    assert(buf != NULL);                // Error if no input string

    char keyword[len + 1];

    snprintf(keyword, sizeof(keyword), "%s", buf);

    // Adjust color saturation, which ncurses allows to range from 0 to 1000.
    // Colors are defined with separate levels for red, green, and blue. Note
    // that setting a level only makes sense for colors other than black, since
    // black is defined as having red, green, and blue all 0.

    if (sat < 0)
    {
        sat = 0;
    }
    else if (sat > 100)
    {
        sat = 100;
    }

    sat *= 10;

    int i = find_color(keyword);        // Get color index

    if (i == -1)
    {
        throw(E_DPY);
    }

    short red   = (short)(color_table[i].red   * (uint_t)sat / SATMAX);
    short green = (short)(color_table[i].green * (uint_t)sat / SATMAX);
    short blue  = (short)(color_table[i].blue  * (uint_t)sat / SATMAX);

    (void)init_color((short)color, red, green, blue);
}

#endif


///
///  @brief    Set foreground and background colors for our three display
///            regions: command, edit, and status line).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static void set_colors(const struct cmd *cmd, enum region_pair pair)
{
    assert(cmd != NULL);

    // The following is used to set up new colors, whose saturation we can vary
    // without affecting the use of the same colors by other regions. That is,
    // the edit region could use a white background at 100% while the command
    // region could use one at 80%. If they both used the standard colors, then
    // changing the saturation for one region would change the other.
    //
    // The colors have the following values:
    //
    // Region  Foreground  Background
    // ------  ----------  ----------
    // CMD         16          17
    // EDIT        18          19
    // STATUS      20          21

    short color = (short)(COLOR_BASE + ((pair - 1) * 2));

    int_t fg_sat, bg_sat;

    if (!cmd->n_set)                    // Neither foreground nor background
    {
        fg_sat = bg_sat = 100;
    }
    else if (!cmd->m_set)               // Foreground, but no background
    {
        fg_sat = cmd->m_arg;
        bg_sat = 100;
    }
    else                                // Both foreground and background
    {
        fg_sat = cmd->m_arg;
        bg_sat = cmd->n_arg;
    }

    set_color(cmd->text1.data, cmd->text1.len, fg_sat, color);
    set_color(cmd->text2.data, cmd->text2.len, bg_sat, color + 1);

    (void)init_pair((short)pair, color, color + 1);
}

#else

static void set_colors(void *unused1, int unused2)
{
}

#endif
