///
///  @file    qreg.c
///  @brief   Q-register functions for TECO.
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
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "qreg.h"


///  @var    qglobal
///  @brief  Global Q-registers.

struct qreg qglobal[QREG_SIZE];

////////////////////////////////////////////////////////////////////////////////
///
///  Definitions for macro stack for local Q-registers.
///
////////////////////////////////////////////////////////////////////////////////

///  @def    MSTACK_SIZE
///  @brief  Number of items that can be put on macro stack.

#define MSTACK_SIZE     64          // Macro stack for local Q-registers

///  @struct mstack
///  @brief  Definition of macro stack structure.

struct mstack
{
//    struct buffer cmd;
    struct qreg qlocal[QREG_SIZE];      ///< Local Q-register set.
};

///  @var    mstack
///  @brief  Macro stack.

struct mstack mstack[MSTACK_SIZE];

///  @var    mlevel
///  @brief  Current level in macro stack.

int mlevel;

///  @var    qlocal
///  @brief  Current set of local Q-registers.

struct qreg *qlocal = mstack[0].qlocal;

////////////////////////////////////////////////////////////////////////////////
///
///  Definitions for Q-register push-down list.
///
////////////////////////////////////////////////////////////////////////////////

///  @def    QSTACK_SIZE
///  @brief  Number of items that can be  put on push-down list.

#define QSTACK_SIZE     64          // Maximum depth for push-down list

///  @var    qstack
///  @brief  Q-register push-down list.

static struct qreg qstack[QSTACK_SIZE];

///  @var    qlevel
///  @brief  Current level in push-down list.

static uint qlevel;

// Local functions

static void free_qreg(void);


///
///  @brief    Append character to Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void append_qchr(int qname, bool qdot, int c)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    uint nbytes = qreg->text.put + 1;

    if (nbytes >= qreg->text.size)
    {
        nbytes += STR_SIZE_INIT;

        qreg->text.buf = expand_mem(qreg->text.buf, qreg->text.size, nbytes);
        qreg->text.size = nbytes;
    }

    qreg->text.buf[qreg->text.put++] = (char)c;
}


///
///  @brief    Append text to Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void append_qtext(int qname, bool qdot, struct tstring text)
{
    assert(text.buf != NULL);

    struct qreg *qreg = get_qreg(qname, qdot);
    uint nbytes = qreg->text.put + text.len;

    if (nbytes >= qreg->text.size)
    {
        uint nblocks = ((nbytes + (STR_SIZE_INIT - 1)) / STR_SIZE_INIT);
                        
        nbytes = nblocks * STR_SIZE_INIT;
        
        void *p = expand_mem(qreg->text.buf, qreg->text.size, nbytes);

        qreg->text.buf = p;
        qreg->text.size = nbytes;
    }

    memcpy(qreg->text.buf + qreg->text.put, text.buf, (size_t)text.len);

    text.buf = NULL;

    qreg->text.put += text.len;
}


///
///  @brief    Free Q-register storage.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void free_qreg(void)
{
    // Free the global Q-registers

    for (int i = 0; i < QREG_SIZE; ++i)
    {
        struct qreg *qreg = &qglobal[i];

        if (qreg->text.buf != NULL)
        {
            free_mem(&qreg->text.buf);
        }
    }

    // Free the local Q-registers

    while (mlevel > 0)
    {
        for (int i = 0; i < QREG_SIZE; ++i)
        {
            struct qreg *qreg = &mstack[mlevel].qlocal[i];

            if (qreg->text.buf != NULL)
            {
                free_mem(&qreg->text.buf);
            }
        }
        --mlevel;
    }
}


///
///  @brief    Get size of text in all Q-registers.
///
///  @returns  Size of text in bytes.
///
////////////////////////////////////////////////////////////////////////////////

uint get_qall(void)
{
    struct qreg *qreg;
    uint n = 0;
    
    for (int i = 0; i < QREG_SIZE; ++i)
    {
        qreg = &qglobal[i];
        n += qreg->text.put;

        qreg = &mstack[0].qlocal[i];
        n += qreg->text.put;
    }

    return n;
}


///
///  @brief    Get ASCII value of nth character in Q-register text string.
///
///  @returns  ASCII value of character, or -1 if not found.
///
////////////////////////////////////////////////////////////////////////////////

int get_qchr(int qname, bool qdot, int n)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    if (n < 0 || (uint)n >= qreg->text.put) // Out of range?
    {
        return EOF;                     // Yes
    }

    return qreg->text.buf[n];
}


///
///  @brief    Get number in Q-register.
///
///  @returns  Q-register number.
///
////////////////////////////////////////////////////////////////////////////////

int get_qnum(int qname, bool qdot)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    return qreg->n;
}


///
///  @brief    Get pointer to Q-register storage.
///
///  @returns  Q-register pointer (error if invalid Q-register).
///
////////////////////////////////////////////////////////////////////////////////

struct qreg *get_qreg(int qname, bool qdot)
{
    static const char *qchars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    const char *qchar = strchr(qchars, toupper(qname));

    if (qchar == NULL)
    {
        printc_err(E_IQN, qname);
    }

    uint qindex = (uint)(qchar - qchars);
    struct qreg *qreg;

    if (qdot)                           // Local Q-register?
    {
        qreg = &mstack[0].qlocal[qindex];
    }
    else
    {
        qreg = &qglobal[qindex];        // Set global Q-register
    }

    return qreg;
}


///
///  @brief    Get size of text in Q-register.
///
///  @returns  Size of text in bytes.
///
////////////////////////////////////////////////////////////////////////////////

uint get_qsize(int qname, bool qdot)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    return qreg->text.put;
}


///
///  @brief    Initialize Q-register storage.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_qreg(void)
{
    mlevel = 0;
    qlevel = 0;

    if (atexit(free_qreg) != 0)
    {
        exit(EXIT_FAILURE);
    }
}


///
///  @brief    Pop Q-register from push-down list.
///
///  @returns  true if success, false if push-down list is empty.
///
////////////////////////////////////////////////////////////////////////////////

bool pop_qreg(int qname, bool qdot)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    if (qlevel == 0)
    {
        return false;
    }

    struct qreg oldreg = qstack[--qlevel];

    if (qreg->text.buf != NULL && qreg->text.buf != oldreg.text.buf)
    {
        free_mem(&qreg->text.buf);
    }

    *qreg = oldreg;

    return true;
}


///
///  @brief    Print Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_qreg(int qname, bool qdot)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    for (uint i = 0; i < qreg->text.put; ++i)
    {
        int c = qreg->text.buf[i];

        if (c == LF)
        {
            putc_term(CRLF);
        }
        else
        {
            echo_chr(c);
        }
    }    
}


///
///  @brief    Push Q-register onto push-down list.
///
///  @returns  true if success, false if push-down list is full.
///
////////////////////////////////////////////////////////////////////////////////

bool push_qreg(int qname, bool qdot)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    if (qlevel == QSTACK_SIZE)
    {
        return false;
    }

    qstack[qlevel++] = *qreg;

    return true;
}


///
///  @brief    Store character in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_qchr(int qname, bool qdot, int c)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    if (qreg->text.buf != NULL)         // Existing buffer?
    {
        free_mem(&qreg->text.buf);      // Yes, discard it
    }

    qreg->text.get  = 0;
    qreg->text.put  = 0;
    qreg->text.size = STR_SIZE_INIT;
    qreg->text.buf  = alloc_mem(qreg->text.size);

    qreg->text.buf[qreg->text.put++] = (char)c;
}


///
///  @brief    Store number in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_qnum(int qname, bool qdot, int n)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    qreg->n = n;
}


///
///  @brief    Store text in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_qtext(int qname, bool qdot, struct tstring text)
{
    assert(text.buf != NULL);

    struct qreg *qreg = get_qreg(qname, qdot);

    if (qreg->text.buf != NULL)         // Existing buffer?
    {
        free_mem(&qreg->text.buf);      // Yes, discard it
    }

    uint nbytes = STR_SIZE_INIT;
        
    void *p = alloc_mem(nbytes);

    qreg->text.buf  = p;
    qreg->text.size = nbytes;
    qreg->text.get  = 0;
    qreg->text.put  = text.len;

    memcpy(qreg->text.buf, text.buf, (size_t)text.len);
}
