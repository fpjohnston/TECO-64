///
///  @file    color_cmd.c
///  @brief   Execute commands that set display colors (F1, F2, F3, F4).
///
///  @copyright 2019-2022 Franklin P. Johnston / Nowwith Treble Software
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
#include <ncurses.h>
#include <string.h>

#define DISPLAY_INTERNAL            ///< Enable internal definitions

#include "teco.h"
#include "display.h"
#include "exec.h"


#define FG              (bool)true      ///< Foreground flag
#define BG              (bool)false     ///< Background flag


static bool color_active = true;        ///< Switched on or off by CTRL/K

static const short color_base = 16;     ///< Start of user-defined color pairs

static const short satmax = 1000;       ///< Saturation maximum

///
///  @struct color_table
///
///  @brief  Table of colors
///

struct color_table
{
    const char *name;               ///< Name of color
    short red;                      ///< Red saturation (0-1000)
    short green;                    ///< Green saturation (0-1000)
    short blue;                     ///< Blue saturation (0-1000)
};

///
///  @var    system_colors
///
///  @brief  Table of pre-defined system colors
///

static const struct color_table system_colors[] =
{
    [COLOR_BLACK]   = { "BLACK",        0,      0,      0, },
    [COLOR_RED]     = { "RED",     satmax,      0,      0, },
    [COLOR_GREEN]   = { "GREEN",        0, satmax,      0, },
    [COLOR_YELLOW]  = { "YELLOW",  satmax, satmax,      0, },
    [COLOR_BLUE]    = { "BLUE",         0,      0, satmax, },
    [COLOR_MAGENTA] = { "MAGENTA", satmax,      0, satmax, },
    [COLOR_CYAN]    = { "CYAN",         0, satmax, satmax, },
    [COLOR_WHITE]   = { "WHITE",   satmax, satmax, satmax, },
};

// Local functions

static int find_color(const char *token);

static void make_pair(short pair);

static void set_color(const tstring *text, short sat, short color, bool type);

static void set_colors(const struct cmd *cmd, short pair);


///
///  @brief    Check that colors have been set (based on whether the foreground
///            and background colors for the command window are both black). If
///            not, we provide default colors for each of the windows (black on
///            white for the edit window, white on black for everything else).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void check_colors(void)
{
    short fg, bg;

    pair_content(CMD, &fg, &bg);

    if (fg == COLOR_BLACK && bg == COLOR_BLACK)
    {
        const short black = color_base;
        const short white = color_base + 1;

        init_color(white, 1000, 1000, 1000);
        init_color(black,    0,    0,    0);

        init_pair(CMD,    white, black);
        init_pair(STATUS, white, black);
        init_pair(LINE,   white, black);
        init_pair(EDIT,   black, white);
    }
}


///  @brief    Execute F1 command: set colors for command window. These colors
///            are also copied to the status and line windows, but the F3 and
///            F4 commands may be used to override those colors after F1 has
///            been executed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F1(struct cmd *cmd)
{
    set_colors(cmd, CMD);
    set_colors(cmd, STATUS);
    set_colors(cmd, LINE);

    if (f.e0.display)
    {
        // Use command window colors for stdscr

        bkgd(COLOR_PAIR(CMD));           //lint !e835

        reset_dpy((bool)true);
    }
}


///
///  @brief    Execute F2 command: set colors for edit window.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F2(struct cmd *cmd)
{
    set_colors(cmd, EDIT);

    if (f.e0.display)
    {
        reset_dpy((bool)true);
    }
}


///
///  @brief    Execute F3 command: set colors for status window.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F3(struct cmd *cmd)
{
    set_colors(cmd, STATUS);

    if (f.e0.display)
    {
        reset_dpy((bool)true);
    }
}


///
///  @brief    Execute F4 command: set colors for divider line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F4(struct cmd *cmd)
{
    set_colors(cmd, LINE);

    if (f.e0.display)
    {
        reset_dpy((bool)true);
    }
}


///
///  @brief    Find color specified by string.
///
///  @returns  Index into table, or -1 if no match.
///
////////////////////////////////////////////////////////////////////////////////

static int find_color(const char *token)
{
    if (token == NULL)
    {
        return -1;
    }

    for (int i = 0; i < (int)countof(system_colors); ++i)
    {
        if (!strcasecmp(token, system_colors[i].name))
        {
            return i;
        }
    }

    return -1;
}


///
///  @brief    Create color pair.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void make_pair(short pair)
{
    short color = (short)((pair - 1) * 2 + color_base);

    init_pair(pair, color, color + 1);
}


///
///  @brief    Switch window colors on or off. Called when user types CTRL/K.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_colors(void)
{
    if (f.e0.display)
    {
        if (color_active)
        {
            color_active = false;

            init_pair(CMD,    COLOR_WHITE, COLOR_BLACK);
            init_pair(EDIT,   COLOR_BLACK, COLOR_WHITE);
            init_pair(STATUS, COLOR_WHITE, COLOR_BLACK);
            init_pair(LINE,   COLOR_WHITE, COLOR_BLACK);
        }
        else
        {
            color_active = true;

            make_pair(CMD);
            make_pair(EDIT);
            make_pair(STATUS);
            make_pair(LINE);
        }
    }
}


///
///  @brief    Initialize saturation levels for a specified color.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void set_color(const tstring *text, short pair, short sat, bool fg)
{
    assert(text != NULL);               // Error if no input string
    assert(pair >= CMD && pair <= MAX_PAIR);

    // Adjust color saturation, which ncurses allows to range from 0 to 1000.
    // Colors are defined with separate levels for red, green, and blue. Note
    // that setting a level only makes sense for colors other than black,
    // since black has red, green, and blue all set to 0.

    if (sat < 0)
    {
        sat = 0;
    }
    else if (sat > 100)
    {
        sat = 100;
    }

    sat *= 10;

    const char *name = build_trimmed(text->data, text->len);
    int i = find_color(name);           // Get color index

    if (i == -1)
    {
        throw(E_KEY, name);             // Keyword 'name' not found
    }

    short red   = (short)(system_colors[i].red   * sat / satmax);
    short green = (short)(system_colors[i].green * sat / satmax);
    short blue  = (short)(system_colors[i].blue  * sat / satmax);
    short color = (short)((pair - 1) * 2 + color_base);

    if (fg && f.e0.display)
    {
        init_color(color, red, green, blue);
    }
    else
    {
        init_color(color + 1, red, green, blue);
    }
}


///
///  @brief    Set foreground and background colors for our four display
///            windows: command, edit, status, and the divider line).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void set_colors(const struct cmd *cmd, short pair)
{
    assert(cmd != NULL);
    assert(pair >= CMD && pair <= MAX_PAIR);

    // The following is used to set up new colors, whose saturation we can vary
    // without affecting the use of the same colors by other windows. That is,
    // the edit window could use a white background at 100% while the command
    // window could use one at 80%. If they both used the standard colors, then
    // changing the saturation for one window would change the other.
    //
    // The colors have the following values:
    //
    // Window  Foreground  Background
    // ------  ----------  ----------
    // CMD         16          17
    // EDIT        18          19
    // STATUS      20          21
    // LINE        22          23

    short fsat, bsat;

    if (!cmd->n_set)                    // Neither foreground nor background
    {
        fsat = bsat = 100;
    }
    else if (!cmd->m_set)               // Foreground, but no background
    {
        fsat = (short)cmd->n_arg;
        bsat = 100;
    }
    else                                // Both foreground and background
    {
        fsat = (short)cmd->m_arg;
        bsat = (short)cmd->n_arg;
    }

    set_color(&cmd->text1, pair, fsat, FG);
    set_color(&cmd->text2, pair, bsat, BG);

    // Here once we've set up a color pair, but we won't use it if colors have
    // been disabled through use of the CTRL/K command. This allows users to
    // turn off colors in order to change them before making them active (such
    // as might be desirable if a window was inadvertently set to the same color
    // for both foreground and background, thus making it unreadable).

    if (color_active && f.e0.display)
    {
        short color = (short)((pair - 1) * 2 + color_base);

        init_pair(pair, color, color + 1);
    }
}
