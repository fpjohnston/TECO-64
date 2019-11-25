///
///  @file    skip_cmd.c
///
///  @brief   Skip past current command.
///
///           The functions in this file are used to skip TECO commands, which
//            is needed to:
///
///           1. Flow to the end of a conditional.
///           2. Flow to the else clause of a conditional.
///           3. Flow to the end of a loop.
///           4. Flow while searching for a tag.
///
///           When skip_cmd() is called, the command buffer points to the start
///           of a command. When we return, the command buffer points to the
///           last character of the command. In the case of a command such as T,
///           the command buffer is left unchanged. In the case of a command
///           such as Stext<ESC>, the command buffer points to the <ESC>.
///
///  @author  Nowwith Treble Software
///
///  @bug     No known bugs.
///
///  @copyright  tbd
///
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files (the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"

int MstTop = -1;                        // TODO: fix this

// Local functions

static int next_chr(void);

//
// Dispatch table used to skip past TECO commands. The top-level function for
// this is skip_cmd(), but several of the functions it calls have to determine
// their own sub-functions (e.g., to process E or F commands).
//
// N.B.: This table presumes that we can specify a range for array members; we
// may need to devise alternative versions for compilers without that feature.
//

static void (*skip_table[])(void) =
{
    [NUL]               = NULL,
    [CTRL_A]            = skip_ctrl_a,
    [CTRL_B]            = NULL,
    [CTRL_C]            = NULL,
    [CTRL_D]            = NULL,
    [CTRL_E]            = NULL,
    [CTRL_F]            = NULL,
    [CTRL_G]            = NULL,
    [BS]                = NULL,
    [TAB]               = skip_esc,
    [LF]                = NULL,
    [VT]                = NULL,
    [FF]                = NULL,
    [CR]                = NULL,
    [CTRL_N]            = NULL,
    [CTRL_O]            = NULL,
    [CTRL_P]            = NULL,
    [CTRL_Q]            = NULL,
    [CTRL_R]            = NULL,
    [CTRL_S]            = NULL,
    [CTRL_T]            = NULL,
    [CTRL_U]            = skip_ctrl_u,
    [CTRL_W]            = NULL,
    [CTRL_X]            = NULL,
    [CTRL_Y]            = NULL,
    [CTRL_Z]            = NULL,
    [ESC]               = NULL,
    [FS]                = NULL,
    [GS]                = NULL,
    [RS]                = skip_one,
    [US]                = NULL,
    [SPACE]             = NULL,
    ['!']               = skip_tag,
    ['"']               = skip_quote,
    ['#']               = NULL,
    ['$']               = NULL,
    ['%']               = skip_one,
    ['&']               = NULL,
    ['\'']              = NULL,
    ['(']               = NULL,
    [')']               = NULL,
    ['*']               = NULL,
    ['+']               = NULL,
    [',']               = NULL,
    ['-']               = NULL,
    ['.']               = NULL,
    ['0']               = NULL,
    ['1']               = NULL,
    ['2']               = NULL,
    ['3']               = NULL,
    ['4']               = NULL,
    ['5']               = NULL,
    ['6']               = NULL,
    ['7']               = NULL,
    ['8']               = NULL,
    ['9']               = NULL,
    [':']               = NULL,
    [';']               = NULL,
    ['<']               = NULL,
    ['=']               = NULL,
    ['>']               = NULL,
    ['?']               = NULL,
    ['@']               = NULL,
    ['A']               = NULL,
    ['B']               = NULL,
    ['C']               = NULL,
    ['D']               = NULL,
    ['E']               = skip_e,
    ['F']               = skip_f,
    ['G']               = skip_one,
    ['H']               = NULL,
    ['I']               = skip_esc,
    ['J']               = NULL,
    ['K']               = NULL,
    ['L']               = NULL,
    ['M']               = skip_one,
    ['N']               = skip_esc,
    ['O']               = skip_esc,
    ['P']               = NULL,
    ['Q']               = skip_one,
    ['R']               = NULL,
    ['S']               = skip_esc,
    ['T']               = NULL,
    ['U']               = skip_one,
    ['V']               = NULL, 
    ['W']               = NULL,
    ['X']               = skip_one,
    ['Y']               = NULL,
    ['Z']               = NULL,
    ['[']               = skip_one,
    ['\\']              = NULL,
    [']']               = skip_one,
    ['^']               = skip_caret,
    ['_']               = skip_esc,
    ['`']               = NULL,
    ['a']               = NULL,
    ['b']               = NULL,
    ['c']               = NULL,
    ['d']               = NULL,
    ['e']               = skip_e,
    ['f']               = skip_f,
    ['g']               = skip_one,
    ['h']               = NULL,
    ['i']               = skip_esc,
    ['j']               = NULL,
    ['k']               = NULL,
    ['l']               = NULL,
    ['m']               = skip_one,
    ['n']               = skip_esc,
    ['o']               = skip_esc,
    ['p']               = NULL,
    ['q']               = skip_one,
    ['r']               = NULL,
    ['s']               = skip_esc,
    ['t']               = NULL,
    ['u']               = skip_one,
    ['v']               = NULL,
    ['w']               = NULL,
    ['x']               = skip_one,
    ['y']               = NULL,
    ['z']               = NULL,
    ['{']               = NULL,
    ['|']               = NULL,
    ['}']               = NULL,
    ['~']               = NULL,
    [DEL]               = NULL,
};


///
///  @brief    Get next character from command buffer, and issue error if we
///            can't get one.
///
///  @returns  Character read, or EOF.
///
////////////////////////////////////////////////////////////////////////////////

static int next_chr(void)
{
    int c;
    
    if ((c = fetch_buf()) == EOF)       // Yes, get the real delimiter
    {
        if (MstTop < 0)                 // If not in a macro
        {
            print_err(E_UTC);           // Unterminated command
        }
    }

    return c;                           // Return character or EOF
}


///
///  @brief    Skip command with one argument bracketed by a delimiter (e.g.,
///            !tag! or ^Ahello^A). Note that we allow for the use of the @
///            form (e.g., @!/tag/ or @^A/hello/).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void skip_arg1(int delim)
{
    int c;

    if (f.ei.atsign)                    // At-sign modifier?
    {
        if ((delim = next_chr()) == EOF)
        {
            return;
        }
    }

    while ((c = next_chr()) != delim)
    {
        if (c == EOF)
        {
            return;
        }
    }
}


///
///  @brief    Skip command with two arguments (e.g., FSabc$def, or @FS/abc/def/).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void skip_arg2(void)
{
    int delim = ESC;                    // Assume no at-sign modifier
    int c;
    
    if (f.ei.atsign)                    // At-sign modifier?
    {
        if ((delim = next_chr()) == EOF)
        {
            return;
        }
    }

    while ((c = next_chr()) != delim)   // Skip first argument
    {
        if (c == EOF)
        {
            return;
        }
    }

    (void)fetch_buf();                  // Skip delimiter between args

    while ((c = next_chr()) != delim)   // Skip second argument
    {
        if (c == EOF)
        {
            return;
        }
    }
}


///
///  @brief    Skip command of the form ^X.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void skip_caret(void)
{
    int c;

    if ((c = next_chr()) == EOF)
    {
        return;
    }

    int ctrl = toupper(c) - 'A' + 1;    // Convert caret-X to CTRL/X

    if (ctrl == CTRL_A)
    {
        skip_ctrl_a();
    }
    else if (ctrl == TAB)
    {
        skip_esc();
    }
    else if (ctrl == LF || ctrl == CR)
    {
        exec_nul();
    }
    else if (ctrl == CTRL_U)
    {
        skip_ctrl_u();
    }
    else if (ctrl == RS)
    {
        skip_one();
    }
    else if (ctrl <= NUL || ctrl > US)
    {
        print_err(E_IUC);               // Illegal character after ^
    }
}


///
///  @brief    Skip past command to specified delimiter.
///
///  @returns  EXIT_SUCCESS or EXIT_FAILURE.
///
///            On exit, the command buffer will either point to the terminating
///            ESCape, if that was the delimiter, else it points *past* the
///            delimiter that the user specified.
///
///            Example      Results
///
///            ^Atext$      Command buffer points to the ESCape.
///            @^A/text/    Command buffer points past the second /.
///
////////////////////////////////////////////////////////////////////////////////

void skip_cmd(void)
{
    int c;
    
    if ((c = next_chr()) == EOF)
    {
        return;
    }

    if (c == NUL || (c >= LF && c <= CR) || c == SPACE)
    {
        (void)exec_nul();               // TODO: temporary!
    }
    else if (c == '@')
    {
        exec_atsign();
    }
    else if ((uint)c < countof(skip_table) && skip_table[c] != NULL)
    {
        (*skip_table[c])();

        f.ei.atsign = false;
    }
}


///
///  @brief    Skip CTRL/A command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void skip_ctrl_a(void)
{
    skip_arg1(CTRL_A);
}


///
///  @brief    Skip CTRL/U command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void skip_ctrl_u(void)
{
    int c;

    if ((c = next_chr()) == EOF)        // Get name of Q-register
    {
        return;
    }

    skip_arg1(ESC);
}


///
///  @brief    Skip E commands (e.g., EBfile$, or EQq).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void skip_e(void)
{
    int c;
    
    if ((c = next_chr()) == EOF)
    {
        return;
    }

    switch (toupper(c))
    {
        case '%':                       // E%q
        case 'Q':                       // EQ$
            skip_one();

            break;

        case '_':                       // E_<search>
        case 'B':                       // EB<file>
        case 'I':                       // EI<file>
        case 'G':                       // EG<command>
        case 'L':                       // EL<file>
        case 'N':                       // EN<file>
        case 'R':                       // ER<file>
        case 'W':                       // EW<file>
            skip_esc();

            break;

        default:
            if (!isalpha(c))
            {
                printc_err(E_IEC, c);
            }

            break;
    }
}


///
///  @brief    Store string delimited by ESCape.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void skip_esc(void)               // Skip argument terminated by ESC
{
    skip_arg1(ESC);                      // Skip past the string
}


///
///  @brief    Skip F command (e.g., FCabc$def$, or FBabc$).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void skip_f(void)                 // Skip an Fx command
{
    int c;

    if ((c = next_chr()) == EOF)
    {
        return;
    }

    switch (toupper(c))
    {
        case 'C':                       // FC<search>$<replace>
        case 'N':                       // FN<search>$<replace>
        case 'S':                       // FS<search>$<replace>
        case '_':                       // F_<search>$<replace>
            skip_arg2();

            break;

        case 'B':                       // FB<search>
        case 'D':                       // FD<search>
        case 'K':                       // FK<search>
        case 'R':                       // FR<search>
            skip_esc();

            break;

        case '<':                       // F>
        case '>':                       // F<
        case '\'':                      // F'
        case '|':                       // F|
            break;

        default:
            printc_err(E_IFC, c);
    }
}


///
///  @brief    Skip one character in command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void skip_one(void)
{
    (void)next_buf();                   // Read and discard one character
}


///
///  @brief    Skip TECO conditional command (e.g., "A, or "=).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void skip_quote(void)
{
    int c;

    if ((c = next_chr()) == EOF)
    {
        return;
    }

    if (strchr("<=>ACDEFGLNRSTUVW", toupper(c)) == NULL)
    {
        print_err(E_IQC);               // Illegal character after "
    }
}


///
///  @brief    Skip GOTO tag (!tag!).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void skip_tag(void)
{
    skip_arg1('!');
}
