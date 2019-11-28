///
///  @file    qreg.c
///  @brief   Q-register functions for TECO.
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "teco.h"
#include "errors.h"

#define MSTACK_SIZE     64          // Macro stack for local Q-registers

#define QSTACK_SIZE     64          // Maximum depth for push-down list

struct qreg qglobal[QREG_SIZE];

struct mstack
{
//    struct buffer cmd;
    struct qreg qlocal[QREG_SIZE];
};

struct mstack mstack[MSTACK_SIZE];

int mlevel;

struct qreg *qlocal = mstack[0].qlocal;

//struct qreg *qreg = NULL;

static struct qreg qstack[QSTACK_SIZE];

static uint qlevel;

// Local functions

static void free_qreg(void);

static struct qreg *get_qreg(int qname, bool qdot);


///
///  @brief    Append character to Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void append_qchr(int qname, bool qdot, int c)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    uint nbytes = qreg->text.put;

    if (nbytes >= qreg->text.size)
    {
        nbytes += STR_SIZE_INIT;

        qreg->text.buf = alloc_more(qreg->text.buf, qreg->text.size, nbytes);
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

void append_qtext(int qname, bool qdot, struct tstr text)
{
    assert(text.buf != NULL);

    struct qreg *qreg = get_qreg(qname, qdot);
    uint nbytes = qreg->text.put + text.len;

    if (nbytes >= qreg->text.size)
    {
        uint nblocks = ((nbytes + (STR_SIZE_INIT - 1)) / STR_SIZE_INIT);
                        
        nbytes = nblocks * STR_SIZE_INIT;
        
        void *p = alloc_more(qreg->text.buf, qreg->text.size, nbytes);

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
            free(qreg->text.buf);

            qreg->text.buf = NULL;
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
                free(qreg->text.buf);

                qreg->text.buf = NULL;
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

static struct qreg *get_qreg(int qname, bool qdot)
{
    static const char *qchars = "0123456789abcdefghijklmnopqrstuvwxyz";

    const char *qchar = strchr(qchars, tolower(qname));

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
        free(qreg->text.buf);
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

    printf("%.*s", (int)qreg->text.put, qreg->text.buf);
    (void)fflush(stdout);
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
        free(qreg->text.buf);           // Yes, discard it
    }

    qreg->text.get  = 0;
    qreg->text.put  = 0;
    qreg->text.size = STR_SIZE_INIT;
    qreg->text.buf  = alloc_new(qreg->text.size);

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

void store_qtext(int qname, bool qdot, struct tstr text)
{
    assert(text.buf != NULL);

    struct qreg *qreg = get_qreg(qname, qdot);

    if (qreg->text.buf != NULL)         // Existing buffer?
    {
        free(qreg->text.buf);           // Yes, discard it

        qreg->text.buf = NULL;
    }

    uint nbytes = STR_SIZE_INIT;
        
    void *p = alloc_new(nbytes);

    qreg->text.buf  = p;
    qreg->text.size = nbytes;
    qreg->text.get  = 0;
    qreg->text.put  = text.len;

    memcpy(qreg->text.buf, text.buf, (size_t)text.len);
}
