///
///  @file    exec_f.c
///  @brief   General dispatcher for TECO F commands (e.g., FR, FS).
///
///           nFB    Search, bounded by n lines
///           m,nFB  Search between locations m and n
///           nFC    Search and replace over n lines
///           m,nFC  Search and replace between locations m and n
///           nFD    Search and delete string
///           nFK    Search and delete intervening text
///           nFN    Global string replace
///           FR     Replace last string
///           nFS    Local string replace
///           F'     Flow to end of conditional
///           F<     Flow to start of iteration
///           F>     Flow to end of iteration
///           nF_    Destructive search and replace
///           F|     Flow to ELSE part of conditional
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"

struct f_cmds
{
    int c2;
    void (*exec)(void);
    int nargs;
    int mflags;
};
    
static struct f_cmds f_cmds[] =
{
    { 'B',  exec_FB,       1, MOD_ACMN },
    { 'C',  exec_FC,       0, MOD_ACN  },
    { 'D',  exec_FD,       0, MOD_AN   },
    { 'K',  exec_FK,       0, MOD_AN   },
    { 'N',  exec_FN,       1, MOD_AN   },
    { 'R',  exec_FR,       1, MOD_AMN  },
    { 'S',  exec_FS,       0, MOD_MN   },
    { '\'', exec_F_apos,   1, MOD_NONE },
    { '<',  exec_F_langle, 1, MOD_NONE },
    { '>',  exec_F_rangle, 1, MOD_NONE },
    { '_',  exec_F_ubar,   1, MOD_AN   },
    { '|',  exec_F_vbar,   1, MOD_NONE },
};


///
///  @brief    Execute F command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_F(void)
{
    int c = fetch_cmd();                // Get character following F

    for (uint i = 0; i < countof(f_cmds); ++i)
    {
        if (f_cmds[i].c2 == toupper(c))
        {
            cmd.c2 = c;

            check_mod(f_cmds[i].mflags);

            (*f_cmds[i].exec)();

            return;
        }
    }

    printc_err(E_IFC, c);               // Illegal F character
}


void exec_FB(void)
{
    check_mod(MOD_AC);

    get_cmd(ESC, 1, &cmd);
}


void exec_F_apos(void)
{
    printf("F%% command\r\n");
}


void exec_F_langle(void)
{
    printf("F< command\r\n");
}


void exec_F_rangle(void)
{
    printf("F> command\r\n");
}


void exec_F_vbar(void)
{
    printf("F| command\r\n");
}

