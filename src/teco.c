///
///  @file    teco.c
///  @brief   Main program for TECO text editor.
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

#define TECO_VERSION    200

const char *prompt = "*";

struct flags f;

int radix = 10;

int teco_version = TECO_VERSION;

int ctrl_x = 0;

bool trace_mode = false;

int last_c = NUL;

jmp_buf jump_main;

int form_feed = -1;

// Local functions

static void cleanup(void);


///
///  @brief    Main program entry for TECO text editor.
///
///  @returns  The usual exit codes.
///
////////////////////////////////////////////////////////////////////////////////

int main(int unused1, const char * const argv[])
{
    assert(argv != NULL);               // (This assertion should NEVER fire)

    f.ei.error = false;
    f.ei.strict = true;

    init_env();                         // Initialize environment
    init_term();                        // Initialize terminal
    init_cmd();                         // Initialize command buffer
    init_qreg();                        // Initialize Q-registers

    if (atexit(cleanup) != 0)           // Clean up when we exit
    {
        exit(EXIT_FAILURE);
    }

    for (;;)                            // Loop forever
    {
        if (setjmp(jump_main) == 0)
        {
            read_cmd();                 // Read one or more commands

#if     defined(TEST_SKIP)

            int command = 0;

            while (!empty_cmd())
            {
                const char *buf = next_cmd();
                int len = strlen(buf);
            
                printf("command #%d: ", ++command); fflush(stdout);
                type_cmd(buf, len);
                putc_term(CRLF);

                skip_cmd();
            }
#else
            exec_cmd();                 // Then execute them
#endif
        }
    }
}


///
///  @brief    General cleanup, initiated when exit() is called.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void cleanup(void)
{
    // TODO: finish these

    print_term("<Close primary input stream>");
    print_term("<Close secondary input stream>");
    print_term("<Close primary output stream>");
    print_term("<Close secondary output stream>");

    print_term("<Free the digit buffer>");
    print_term("<Free the edit buffer>");
    print_term("<Free the filename buffer>");
    print_term("<Free the search buffer>");

    (void)fflush(stdout);
}


///
///  @brief    Print the TECO prompt (this may be the standard asterisk, or
///            something else specified by the user).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_prompt(void)
{
    f.et.abort = false;                 // Don't abort on error

    puts_term(prompt, strlen(prompt));
}


void get_arg(int delim, struct tstr *tstr)
{
    tstr->count = 0;
    tstr->str = next_cmd();

    int c = fetch_cmd();
    
    if (c == EOF)
    {
        print_err(E_UTC);
    }
    else if (c == delim)
    {
        tstr->str = NULL;

        return;
    }

    ++tstr->count;

    while ((c = fetch_cmd()) != delim)
    {
        if (c == EOF)
        {
            print_err(E_UTC);
        }

        ++tstr->count;
    }
}

static void print_cmd(int delim, struct cmd *cmd);

void get_cmd(int delim, int ncmds, struct cmd *cmd)
{
    int c;

    if (cmd->qreg)
    {
        c = fetch_cmd();

        if (c == '.')                   // Local Q-register?
        {
            cmd->qlocal = true;         // Yes, mark it
            c = fetch_cmd();            // Get Q-register name
        }        

        if (c == EOF)
        {
            print_err(E_UTC);           // Unterminated command
        }

        if (!isalnum(c))
        {
            printc_err(E_IQN, c);       // Illegal Q-register name
        }

        cmd->qreg = c;                  // Save the name
    }

    if (f.ei.atsign)
    {
        c = fetch_cmd();

        if (c == EOF)
        {
            print_err(E_UTC);
        }

        delim = c;
    }

    get_arg(delim, &cmd->arg1);

    if (ncmds > 1)
    {
        get_arg(delim, &cmd->arg2);
    }

    print_cmd(delim, cmd);              // Print the command we just parsed

    f.ei.atsign = false;
    f.ei.colon  = false;
    f.ei.dcolon = false;
    f.ei.comma  = false;
}

static void print_cmd(int delim, struct cmd *cmd)
{
    // Here when we've parsed the entire command - now type it out.

    printf("cmd: ");
    fflush(stdout);
    
    if (f.ei.colon)
    {
        echo_chr(':');
    }
    else if (f.ei.dcolon)
    {
        echo_chr(':');
        echo_chr(':');
    }        

    if (f.ei.atsign)
    {
        echo_chr('@');
    }

    echo_chr(cmd->c1);

    if (cmd->c2 != NUL)
    {
        echo_chr(cmd->c2);
    }

    if (cmd->qreg)                      // Do we have a Q-register name?
    {
        if (cmd->qlocal)                // Yes, is it local?
        {
            echo_chr('.');
        }

        echo_chr(cmd->qreg);
    }

    if (cmd->arg1.count != 0)
    {
        if (f.ei.atsign)                // Conditionally echo delimiter before 1st arg.
        {
            echo_chr(delim);
        }

        for (const char *p = cmd->arg1.str; p < cmd->arg1.str + cmd->arg1.count; ++p)
        {
            echo_chr(*p);
        }
    }

    if (cmd->arg2.count != 0)
    {
        echo_chr(delim);                // Always echo delimiter before 2nd arg.

        for (const char *p = cmd->arg2.str; p < cmd->arg2.str + cmd->arg2.count; ++p)
        {
            echo_chr(*p);
        }
    }

    echo_chr(delim);                    // And always echo delimiter at end
    putc_term(CRLF);
}
