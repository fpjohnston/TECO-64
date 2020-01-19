///
///  @file    s_cmd.c
///  @brief   Execute S command.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"
#include "qreg.h"
#include "textbuf.h"


struct tstring last_search = { .len = 0 };

// Local functions

static int match_chr(int c, const char *start, uint len);

static void reset_search(void);

int issymbol(int c);

int isdelim(int c);

int isqreg(int c, const char *start, uint len);

bool search_forward(void);

int issymbol(int c)
{
    if (isalnum(c)   || c == '.' ||
        (f.e2.dollar && c == '$') ||
        (f.e2.ubar   && c == '_'))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int isdelim(int c)
{
    if (c == LF || c == VT || c == FF)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int isqreg(int c, const char *start, uint len)
{
    assert(start != NULL);

    const char *buf = start;
    int qname;
    bool qlocal = false;            
    
    if (len-- == 0)
    {
        print_err(E_IQN);
    }

    if ((qname = *buf++) == '.')
    {
        qlocal = true;

        if (len-- == 0)
        {
            print_err(E_IQN);
        }

        qname = *buf++;
    }

    int nbytes = buf - start;
    struct qreg *qreg = get_qreg(qname, qlocal);

    for (uint i = 0; i < qreg->text.len; ++i)
    {
        if (c == qreg->text.buf[i])
        {
            return nbytes;
        }
    }

    return -nbytes;
}

static int match_chr(int c, const char *start, uint len)
{
    assert(start != NULL);

    const char *buf = start;

    if (len-- == 0)
    {
        print_err(E_ISS);
    }

    int s = *buf++;                     // Next character from search string

    if (s == CTRL_E)
    {
        if (len-- == 0)
        {
            print_err(E_ISS);
        }

        s = toupper(*buf++);

        if ((s == 'A' && isalpha(c))  ||
            (s == 'C' && issymbol(c)) ||
            (s == 'D' && isdigit(c))  ||
            (s == 'L' && isdelim(c))  ||
            (s == 'R' && isalnum(c))  ||
            (s == 'V' && islower(c))  ||
            (s == 'W' && isupper(c))  ||
            (s == 'X'))
        {
            return buf - start;
        }
        else if (s == 'G')
        {
            int n = isqreg(c, buf, len);
            buf += abs(n);

            if (n > 0)
            {
                return buf - start;
            }
        }
            
        // TODO: process ^EB, ^EM, ^ES, ^E<nnn>, ^E[a,b,c...]

        return start - buf;
    }
    else if (s == CTRL_N)
    {
        int nbytes = match_chr(c, buf, len);

        buf += abs(nbytes);

        if (nbytes < 0)
        {
            return buf - start;
        }
        else
        {
            return start - buf;
        }
    }
    else if ((s == CTRL_S && !isalnum(c)) || s == CTRL_X)
    {
        return buf - start;
    }
    else if (c == s)
    {
        return buf - start;
    }
    else
    {
        return start - buf;
    }
}

static uint match_str(uint start)
{
    const char *p = last_search.buf;
    uint len = last_search.len;
    uint pos = start;

    while (len > 0)
    {
        int c = (int)getchar_tbuf((int)pos++);
        int n = match_chr(c, p, len);

        if (n < 0)
        {
            return 0;
        }

        p += n;
        len -= (uint)n;
    }

    return pos - start;
}

bool search_forward(void)
{
    uint dot = getpos_tbuf();
    uint Z   = getsize_tbuf();
    uint n;

    for (uint pos = 0; pos < Z - dot; ++pos)
    {
        if ((n = match_str(pos)) > 0)
        {
            setpos_tbuf(dot + pos + n);

            return true;
        }
    }

    return false;
}


///
///  @brief    Execute S command (local search).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_S(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->n_set && cmd->n_arg == 0)  // 0Stext` isn't allowed
    {
        print_err(E_ISA);               // Illegal search argument
    }

    if (cmd->dcolon_set)                // ::Stext` => 1,1:Stext`
    {
        cmd->m_arg = cmd->n_arg = 1;
        cmd->m_set = cmd->n_set = true;
    }
    else if (!cmd->n_set)               // Stext` => 1Stext`
    {
        cmd->n_arg = 1;
        cmd->n_set = true;
    }

    print_str("%s search for occurrence #%u\r\n",
              cmd->n_arg < 0 ? "backward" : "forward", (uint)abs(cmd->n_arg));

    if (cmd->text1.len != 0)
    {
        free_mem(&last_search.buf);

        last_search.len = build_string(&last_search.buf, cmd->text1.buf,
                                       cmd->text1.len);
    }

    if (cmd->m_set)
    {
        print_str("    limit search to %d characters\r\n", abs(cmd->m_arg) - 1);
    }

    if (cmd->colon_set)
    {
        print_str("    return success or failure\r\n");
    }

    printf("built string, %u characters:\r\n", last_search.len);

    if (!search_forward())
    {
        printf("string not found\r\n");
    }
}


///
///  @brief    Initialize search string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_search(void)
{
    if (atexit(reset_search) != 0)
    {
        fatal_err(errno, E_UIT, NULL);
    }
}


///
///  @brief    Reset search string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void reset_search(void)
{
    free_mem(&last_search.buf);
}
