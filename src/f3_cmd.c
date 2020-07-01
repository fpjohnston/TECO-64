///
///  @file    f3_cmd.c
///  @brief   Execute F3 command.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
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
#include <string.h>

#if     defined(SCOPE)

#include <ncurses.h>

#endif

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "exec.h"
#include "window.h"


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

static void set_colors(const char *keyword, char *value);

    
///
///  @brief    Execute F3 command: set window colors.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F3(struct cmd *cmd)
{

#if     defined(SCOPE)

    assert(cmd != NULL);

    char keyword[cmd->text1.len + 1];

    sprintf(keyword, "%.*s", (int)cmd->text1.len, cmd->text1.buf);

    if (cmd->n_set)
    {
        // Set the saturation for a specified color. ncurses allows these
        // levels to range from 0 to 1000. Colors are defined with separate
        // levels for red, green, and blue. Note that setting a level only
        // makes sense for colors other than black, since black is defined
        // as having red, green, and blue all 0.

        int color = find_color(keyword);

        if (color == -1)
        {
            throw(E_WIN);
        }

        int n = cmd->n_arg;              // Color saturation

        // Make sure it's in the range [0, SATMAX].

        if (n < 0)
        {
            n = 0;
        }
        else if (n > SATMAX)
        {
            n = SATMAX;
        }

        // Adjust color saturation

        short red   = (short)(color_table[color].red   * (uint)n / SATMAX);
        short green = (short)(color_table[color].green * (uint)n / SATMAX);
        short blue  = (short)(color_table[color].blue  * (uint)n / SATMAX);

        (void)init_color((short)color, red, green, blue);
    }
    else
    {
        char value[cmd->text2.len + 1];

        sprintf(value, "%.*s", (int)cmd->text2.len, cmd->text2.buf);

        if (*keyword != NUL)                // Anything to parse?
        {
            set_colors(keyword, value);
        }
    }

#else

    throw(E_WIN);

#endif

}


///
///  @brief    Find color specified by string.
///
///  @returns  Index into table, or -1 if no match.
///
////////////////////////////////////////////////////////////////////////////////

static int find_color(const char *token)
{

#if     defined(SCOPE)

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

#endif

    return -1;
}


///
///  @brief    Set foreground and background colors for our three window
///            regions: command, text, and status line).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void set_colors(const char *keyword, char *value)
{

#if     defined(SCOPE)

    assert(keyword != NULL);
    assert(value != NULL);

    struct region *region;

    if (!strcasecmp(keyword, "command"))
    {
        region = &d.cmd;
    }
    else if (!strcasecmp(keyword, "text"))
    {
        region = &d.text;
    }
    else if (!strcasecmp(keyword, "status"))
    {
        region = &d.status;
    }
    else
    {
        throw(E_WIN);
    }

    char *saveptr;
    int fg = find_color(strtok_r(value, " ,", &saveptr));
    int bg = find_color(strtok_r(NULL,  " ,", &saveptr));

    if (fg != -1)
    {
        region->fg = (short)fg;
    }

    if (bg != -1)
    {
        region->bg = (short)bg;
    }

    if (region == &d.cmd)
    {
        (void)assume_default_colors(d.cmd.fg, d.cmd.bg);
    }
    else if (region == &d.text)
    {
        (void)init_pair(TEXT, d.text.fg, d.text.bg);
    }
    else
    {
        (void)init_pair(STATUS, d.status.fg, d.status.bg);
    }

#endif

}
