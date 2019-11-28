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

static void set_opts(struct cmd *cmd, const char *opts);


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
///  @brief    Scan command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

const struct cmd_table *scan_cmd(struct cmd *cmd, int c)
{
    // Here to start parsing a command string,  one character at a time. Note
    // that although some commands may contain only a single character, most of
    // them comprise multiple characters, so we have to keep looping until we
    // have found everything we need. As we continue, we store information in
    // the command block defined by 'cmd', for use when we're ready to actually
    // execute the command. This includes such things as m and n arguments,
    // modifiers such as : and @, and any text strings followed the command.

    struct cmd_table *table;

    cmd->c1 = (char)c;

    if (toupper(c) == 'E')          // E{x} command
    {
        cmd->c2 = (char)fetch_buf();

        table = scan_E(cmd);
    }
    else if (toupper(c) == 'F')     // F{x} command
    {
        cmd->c2 = (char)fetch_buf();

        table = scan_F(cmd);
    }
    else if (toupper(c) == '^')     // ^{x} command
    {
        cmd->c1 = (char)fetch_buf();
        cmd->c1 = scan_caret(cmd);

        table = &cmd_table[(int)cmd->c1];
    }
    else                            // Everything else
    {
        cmd->c2 = NUL;

        table = &cmd_table[toupper(c)];
    }

    if (table->scan == NULL)
    {
        return NULL;
    }

    set_opts(cmd, table->opts);

    return table;
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

    scan_state = SCAN_DONE;
}


///
///  @brief    Scan expression. Note that we don't do much here other than to
///            check for correct form.
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
            c = fetch_buf();            // Keep reading characters
        } while (valid_radix(c));       // As long as they're valid for our radix

        unfetch_buf(c);                 // Went too far, so return character

        cmd->n_set = true;
        cmd->n_arg = 1;

        push_expr(cmd->n_arg, EXPR_OPERAND); // Dummy value
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
             
    scan_state = SCAN_EXPR;
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
        if (f.ei.strict && cmd->atsign_set)
        {
            print_err(E_MOD);           // Two @'s are not allowed
        }

        cmd->atsign_set = true;
    }
    else if (cmd->c1 == ':')
    {
        if (f.ei.strict && cmd->dcolon_set)
        {
            print_err(E_MOD);           // More than two :'s are not allowed
        }

        if (cmd->colon_set)
        {
            cmd->colon_set = false;
            cmd->dcolon_set = true;
        }
        else if (!cmd->dcolon_set)
        {
            cmd->colon_set = true;
        }
    }

    scan_state = SCAN_MOD;
}


///
///  @brief    Scan the rest of the command string. We enter here after scanning
///            any expression, and any prefix modifiers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_tail(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->paren != 0)
    {
        print_err(E_MRP);               // Missing right parenthesis
    }
    else if (f.ei.strict)
    {
        if (   (cmd->m_set      && !cmd->m_opt     )
            || (cmd->n_set      && !cmd->n_opt     )
            || (cmd->colon_set  && !cmd->colon_opt )
            || (cmd->dcolon_set && !cmd->dcolon_opt)
            || (cmd->atsign_set && !cmd->atsign_opt))
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

    if (cmd->q_req)                     // Do we need a Q-register?
    {
        c = fetch_buf();                // Yes

        if (c == '.')                   // Is it a local Q-register?
        {
            cmd->qlocal = true;         // Yes, mark it

            c = fetch_buf();            // Get Q-register name
        }        

        if (!isalnum(c))
        {
            // The following allows use of G* and G_

            if (toupper(cmd->c1) != 'G' || (c != '*' && c != '_'))
            {
                printc_err(E_IQN, c);   // Illegal Q-register name
            }
        }

        cmd->qreg = (char)c;            // Save the name
    }

    // The P command can optionally be followed by a W. This doesn't really
    // change how the command works.

    if (cmd->w_opt)                     // Optional W following?
    {
        c = fetch_buf();                // Maybe

        if (toupper(c) == 'W')          // Is it?
        {
            cmd->c2 = (char)c;          // Yes
        }
        else
        {
            unfetch_buf(c);             // No. Put character back
        }
    }

    // The @ prefix modifier has already been scanned here, but if the user
    // specified it, the first character after the command is the alternate
    // delimiter to use for the subsequent text string.

    if (cmd->atsign_set)                // @ modifier?
    {
        cmd->delim = (char)fetch_buf(); // Yes, next character is delimiter
    }

    // Now get the text strings, if they're allowed for this command.

    if (cmd->t1_opt || cmd->t2_opt)
    {
        scan_text(cmd->delim, &cmd->text1);

        if (cmd->t2_opt)
        {
            scan_text(cmd->delim, &cmd->text2);
        }
    }
}


///
///  @brief    Scan the text string following the command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void scan_text(int delim, struct tstr *text)
{
    assert(text != NULL);

    text->len = 0;
    text->buf = next_buf();

    if (fetch_buf() == delim)
    {
        text->buf = NULL;

        return;
    }

    ++text->len;

    while (fetch_buf() != delim)
    {
        ++text->len;
    }
}


///
///  @brief    Scan a variable (which can be an operand as well as a command).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_var(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (operand_expr())                 // Is there an operand available?
    {
        (void)get_n_arg();

        cmd->n_set = true;
        scan_state = SCAN_DONE;
    }
    else
    {
        push_expr(1, EXPR_OPERAND);     // Dummy value

        scan_state = SCAN_EXPR;
    }
}


///
///  @brief    Set command options. The command tables in this file, e_cmd.c,
///            and f_cmd.c all include strings that define the options for each
///            command. The while() and switch() statements below parse these
///            strings to set the appropriate options, as follows:
///
///            n  - Command allows one argument          (e.g., nC).
///                 Note: implies n.
///            m  - Command allows two arguments         (e.g., m,nT).
///            H  - Command allows H to be special       (e.g., HP).
///                 Note: implies m and n
///            :  - Command allows colon modifier        (e.g., :ERfile`).
///            :: - Command allows double colon modifier (e.g., ::Stext`).
///            @  - Command allows atsign form           (e.g., @^A/hello/).
///            q  - Command requires Q-register          (e.g., Mq).
///            W  - Command allows W                     (e.g., PW).
///            1  - Command allows one text string       (e.g., Otag`).
///            2  - Command allows two text strings      (e.g., FNfoo`baz`).
///                 Note: implies 1.
///
///            These do not need to be in any particular order, and characters
///            not in the list above will be ignored. This allows the use of
///            spaces to improve readability.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void set_opts(struct cmd *cmd, const char *opts)
{
    assert(cmd != NULL);

    if (opts == NULL)
    {
        return;
    }

    int c;

    while ((c = *opts++) != NUL)
    {
        switch (c)
        {
            case 'H':
                cmd->h_opt = true;
                //lint -fallthrough 

            case 'm':
                cmd->m_opt = true;
                //lint -fallthrough

            case 'n':
                cmd->n_opt = true;

                break;

            case ':':
                if (*opts == ':')       // Double colon?
                {
                    cmd->dcolon_opt = true;

                    ++opts;
                }
                else                    // No, just single colon
                {
                    cmd->colon_opt  = true;
                }

                break;

            case '@':
                cmd->atsign_opt = true;

                break;

            case 'q':
                cmd->q_req = true;

                break;

            case 'W':
                cmd->w_opt  = true;

                break;

            case '2':
                cmd->t2_opt = true;
                //lint -fallthrough

            case '1':
                cmd->t1_opt = true;

                break;

            default:
                assert(isspace(c));

                break;
        }
    }
}
