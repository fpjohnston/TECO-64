///
///  @file    cmd_scan.c
///  @brief   Functions to scan TECO command string.
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
///  @mainpage teco - TECO text editor
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


// Local functions

static void scan_text(int delim, struct tstr *tstr);


///
///  @brief    We've scanned an illegal character, so return to main loop.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_bad(struct cmd *cmd)
{
    assert(cmd != NULL);

    printc_err(E_ILL, cmd->c1);
}


///
///  @brief    Scan the rest of the command string. We enter here after scanning
///            any expression, and any prefix modifiers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->paren != 0)
    {
        print_err(E_MRP);               // Missing right parenthesis
    }
    else if (f.ei.strict)
    {
        if (   (cmd->got_m      && !cmd->opt_m     )
            || (cmd->got_n      && !cmd->opt_n     )
            || (cmd->got_colon  && !cmd->opt_colon )
            || (cmd->got_dcolon && !cmd->opt_dcolon)
            || (cmd->got_atsign && !cmd->opt_atsign))
        {
            print_err(E_MOD);           // Invalid modifier for command
        }
    }

    cmd->delim = ESC;                   // Assume we'll use the std. delimiter

    if (cmd->c1 == CTRL_A || cmd->c1 == '!')
    {
        cmd->delim = cmd->c1;           // Switch delimiter for CTRL/A and !
    }

    // Some commands have a postfix Q-register, which is an alphanumeric name
    // optionally preceded by a . (to flag that it's a local and not a global
    // Q-register.

    int c;

    if (cmd->opt_qreg)                  // Do we need a Q-register?
    {
        c = fetch_cmd();                // Yes

        if (c == '.')                   // Is it a local Q-register?
        {
            cmd->qlocal = true;         // Yes, mark it

            c = fetch_cmd();            // Get Q-register name
        }        

        if (!isalnum(c))
        {
            printc_err(E_IQN, c);       // Illegal Q-register name
        }

        cmd->qreg = c;                  // Save the name
    }

    // The P command can optionally be followed by a W. This doesn't really
    // change how the command works.

    if (cmd->opt_w)                     // Optional W following?
    {
        c = fetch_cmd();                // Maybe

        if (toupper(c) == 'W')          // Is it?
        {
            cmd->c2 = c;                // Yes
        }
        else
        {
            unfetch_cmd(c);             // No. Put character back
        }
    }

    // The @ prefix modifier has already been scanned here, but if the user
    // specified it, the first character after the command is the alternate
    // delimiter to use for the subsequent text string.

    if (cmd->got_atsign)                // @ modifier?
    {
        cmd->delim = fetch_cmd();       // Yes, next character is delimiter
    }

    // Now get the text strings, if they're allowed for this command.

    if (cmd->opt_t1 || cmd->opt_t2)
    {
        scan_text(cmd->delim, &cmd->text1);

        if (cmd->opt_t2)
        {
            scan_text(cmd->delim, &cmd->text2);
        }
    }
}


///
///  @brief    Flag that we've scanned the actual command character.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_done(struct cmd *cmd)
{
    assert(cmd != NULL);

    cmd->state = CMD_DONE;
}


///
///  @brief    Scan expression. Note that we don't do much here other than to
///            check for correctc form.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_expr(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c = cmd->c1;

    if (isdigit(c))
    {
        do
        {
            c = fetch_cmd();            // Keep reading characters
        } while (valid_radix(c));       // As long as they're valid for our radix

        unfetch_cmd(c);                 // Went too far, so return character

        push_expr(1, EXPR_OPERAND);     // Dummy value
    }
    else if (c == ',')
    {
        exec_comma(cmd);
    }
    else
    {
        if (c == '(')
        {
            ++cmd->paren;
        }
        else if (c == ')')
        {
            if (cmd->paren == 0)
            {
                print_err(E_MLP);
            }

            --cmd->paren;
        }

        push_expr(c, EXPR_OPERATOR);
    }
             
    cmd->state = CMD_EXPR;
}


///
///  @brief    Scan a flag value (which can be an operand as well as a command).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_flag(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (operand_expr())                 // Is there an operand available?
    {
        (void)get_n_arg();

        cmd->got_n = true;
        cmd->state = CMD_DONE;
    }
    else
    {
        push_expr(1, EXPR_OPERAND);     // Dummy value

        cmd->state = CMD_EXPR;
    }
}


///
///  @brief    Scan a command modifier (i.e., @ or :).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_mod(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->c1 == '@')
    {
        if (f.ei.strict && cmd->got_atsign)
        {
            print_err(E_MOD);           // Two @'s are not allowed
        }

        cmd->got_atsign = true;
    }
    else if (cmd->c1 == ':')
    {
        if (f.ei.strict && cmd->got_dcolon)
        {
            print_err(E_MOD);           // More than two :'s are not allowed
        }

        if (cmd->got_colon)
        {
            cmd->got_colon = false;
            cmd->got_dcolon = true;
        }
        else if (!cmd->got_dcolon)
        {
            cmd->got_colon = true;
        }
    }

    cmd->state = CMD_MOD;
}


///
///  @brief    Scan the text string following the command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void scan_text(int delim, struct tstr *text)
{
    text->len = 0;
    text->buf = next_cmd();

    int c = fetch_cmd();
    
    if (c == delim)
    {
        text->buf = NULL;

        return;
    }

    ++text->len;

    while ((c = fetch_cmd()) != delim)
    {
        ++text->len;
    }
}
