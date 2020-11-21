///
///  @file    qreg.c
///  @brief   Q-register functions for TECO.
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "teco.h"
#include "errcodes.h"
#include "exec.h"
#include "qreg.h"
#include "term.h"

///  @def    QREGISTER
///  @brief  Get pointer to Q-register data structure.

#define QREGISTER(i) (i >= QCOUNT ? &local_head->qreg[i - QCOUNT] : &qglobal[i])

///  @var    qglobal
///  @brief  Global Q-registers.

static struct qreg qglobal[QCOUNT];

///  @struct qlocal
///  @brief  Local Q-register set.

struct qlocal
{
    struct qlocal *next;                ///< Next item in list
    struct qreg qreg[QCOUNT];           ///< Local Q-register set
};

///  @var    local_head
///  @brief  Head of local Q-register set linked list.

static struct qlocal *local_head = NULL;

////////////////////////////////////////////////////////////////////////////////
///
///  Definitions for Q-register push-down list. This is actually implemented as
///  linked list with a head pointer, which removes any need for an artificial
///  limit as to how many items can be pushed.
///
////////////////////////////////////////////////////////////////////////////////

///  @struct qlist
///  @brief  Linked list for pushing and popping Q-registers.

struct qlist
{
    struct qlist *next;                 ///< Next item in list
    struct qreg qreg;                   ///< Q-register value and text
};

///  @var    list_head
///  @brief  Q-register push-down list.

static struct qlist *list_head = NULL;


// Local functions

static void exit_qreg(void);


///
///  @brief    Append character to Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void append_qchr(int qindex, int c)
{
    struct qreg *qreg = QREGISTER(qindex);

    if (qreg->text.data == NULL)
    {
        qreg->text.pos  = 0;
        qreg->text.len  = 0;
        qreg->text.size = STR_SIZE_INIT;
        qreg->text.data = alloc_mem(qreg->text.size);
    }
    else
    {
        uint nbytes = qreg->text.len;

        if (nbytes == qreg->text.size)
        {
            nbytes += STR_SIZE_INIT;

            qreg->text.data = expand_mem(qreg->text.data, qreg->text.size,
                                         nbytes);
            qreg->text.size = nbytes;
        }
    }

    qreg->text.data[qreg->text.len++] = (char)c;
}


///
///  @brief    Delete text in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void delete_qtext(int qindex)
{
    struct qreg *qreg = QREGISTER(qindex);

    free_mem(&qreg->text.data);

    qreg->text.size = 0;
    qreg->text.len  = 0;
    qreg->text.pos  = 0;
}


///
///  @brief    Clean up memory before we exit from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exit_qreg(void)
{
    // Free local Q-registers and reset the push-down list.

    reset_qreg();

    // The prompt level set of local Q-registers is not freed by reset_qreg(),
    // so we have to do that ourselves.

    assert(local_head != NULL);         // Error if no local Q-registers
    assert(local_head->next == NULL);   // Error if more than one set

    for (uint i = 0; i < QCOUNT; ++i)
    {
        if (local_head->qreg[i].text.data != NULL)
        {
            free_mem(&local_head->qreg[i].text.data);
        }
    }

    free_mem(&local_head);

    // Free the global Q-registers

    for (uint i = 0; i < QCOUNT; ++i)
    {
        struct qreg *qreg = &qglobal[i];

        free_mem(&qreg->text.data);
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
    uint n = 0;
    struct qreg *qreg;

    // Get count of all text in global Q-registers

    for (uint i = 0; i < QCOUNT; ++i)
    {
        qreg = &qglobal[i];
        n += qreg->text.len;
    }

    struct qlocal *qnext = local_head;

    // Get count of all text in local Q-registers

    while (qnext != NULL)
    {
        for (uint i = 0; i < QCOUNT; ++i)
        {
            qreg = &qnext->qreg[i];
            n += qreg->text.len;
        }

        qnext = qnext->next;
    }

    // Get count of all text in push-down list

    struct qlist *qlist = list_head;

    while (qlist != NULL)
    {
        qreg = &qlist->qreg;
        n += qreg->text.len;
        qlist = qlist->next;
    }

    return n;
}


///
///  @brief    Get ASCII value of nth character in Q-register text string.
///
///  @returns  ASCII value of character, or -1 if not found.
///
////////////////////////////////////////////////////////////////////////////////

int get_qchr(int qindex, int n)
{
    struct qreg *qreg = QREGISTER(qindex);

    if (n < 0 || (uint)n >= qreg->text.len) // Out of range?
    {
        return EOF;                     // Yes
    }

    return qreg->text.data[n];
}


///
///  @brief    Get index into Q-register array, given its name.
///
///  @returns  Index, or -1 if not found.
///
////////////////////////////////////////////////////////////////////////////////

int get_qindex(int qname, bool qlocal)
{
    static const char *qchars = QNAMES "abcdefghijklmnopqrstuvwxyz";

    const char *qchar = strchr(qchars, qname);

    if (qchar == NULL)
    {
        return -1;
    }

    int qindex  = qchar - qchars;

    qindex -= (qindex >= QCOUNT) ? QCOUNT : 0;
    qindex += (qlocal)           ? QCOUNT : 0;

    return qindex;
}


///
///  @brief    Get number in Q-register.
///
///  @returns  Q-register number.
///
////////////////////////////////////////////////////////////////////////////////

int get_qnum(int qindex)
{
    struct qreg *qreg = QREGISTER(qindex);

    return qreg->n;
}


///
///  @brief    Get Q-register entry.
///
///  @returns  Q-register entry.
///
////////////////////////////////////////////////////////////////////////////////

struct qreg *get_qreg(int qindex)
{
    if (qindex >= QCOUNT)
    {
        return &local_head->qreg[qindex - QCOUNT];
    }
    else
    {
        return &qglobal[qindex];
    }
}


///
///  @brief    Get size of text in Q-register.
///
///  @returns  Size of text in bytes.
///
////////////////////////////////////////////////////////////////////////////////

uint get_qsize(int qindex)
{
    struct qreg *qreg = QREGISTER(qindex);

    return qreg->text.len;
}


///
///  @brief    Initialize Q-register storage.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_qreg(void)
{
    register_exit(exit_qreg);

    list_head = NULL;
    local_head = alloc_mem((uint)sizeof(*local_head));
}


///
///  @brief    Pop local Q-register set.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void pop_qlocal(void)
{
    struct qlocal *saved_set = local_head;

    local_head = saved_set->next;

    for (uint i = 0; i < QCOUNT; ++i)
    {
        if (saved_set->qreg[i].text.data != NULL)
        {
            free_mem(&saved_set->qreg[i].text.data);
        }
    }

    free_mem(&saved_set);
}


///
///  @brief    Pop Q-register from push-down list.
///
///  @returns  true if success, false if push-down list is empty.
///
////////////////////////////////////////////////////////////////////////////////

bool pop_qreg(int qindex)
{
    struct qreg *qreg = QREGISTER(qindex);

    if (list_head == NULL)
    {
        return false;
    }

    struct qlist *savedq = list_head;

    list_head = savedq->next;

    if (qreg->text.data != NULL && qreg->text.data != savedq->qreg.text.data)
    {
        free_mem(&qreg->text.data);
    }

    *qreg = savedq->qreg;

    free_mem(&savedq);

    return true;
}


///
///  @brief    Print Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_qreg(int qindex)
{
    struct qreg *qreg = QREGISTER(qindex);

    for (uint i = 0; i < qreg->text.len; ++i)
    {
        int c = qreg->text.data[i];

        type_out(c);
    }
}


///
///  @brief    Push local Q-register set.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void push_qlocal(void)
{
    struct qlocal *qlocal = alloc_mem((uint)sizeof(*qlocal));

    qlocal->next = local_head;

    local_head = qlocal;
}


///
///  @brief    Push (deep) copy of Q-register onto push-down list.
///
///  @returns  true if success, false if push-down list is full.
///
////////////////////////////////////////////////////////////////////////////////

bool push_qreg(int qindex)
{
    struct qreg *qreg    = QREGISTER(qindex);
    struct qlist *savedq = alloc_mem((uint)sizeof(*savedq));

    savedq->qreg.n         = qreg->n;
    savedq->qreg.text.size = qreg->text.size;
    savedq->qreg.text.pos  = qreg->text.pos;
    savedq->qreg.text.len  = qreg->text.len;
    savedq->qreg.text.data = alloc_mem(savedq->qreg.text.size);

    memcpy(savedq->qreg.text.data, qreg->text.data,
           (ulong)savedq->qreg.text.size);

    savedq->next = list_head;

    list_head = savedq;

    return true;
}


///
///  @brief    Free local Q-registers and reset the push-down list.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_qreg(void)
{
    // Free the local Q-registers

    while (local_head->next != NULL)
    {
        struct qlocal *saved_set = local_head;

        local_head = saved_set->next;

        for (uint i = 0; i < QCOUNT; ++i)
        {
            if (saved_set->qreg[i].text.data != NULL)
            {
                free_mem(&saved_set->qreg[i].text.data);
            }
        }

        free_mem(&saved_set);
    }

    // Free up what's on the Q-register push-down list.

    struct qlist *savedq;

    while ((savedq = list_head) != NULL)
    {
        list_head = savedq->next;

        free_mem(&savedq->qreg.text.data);
        free_mem(&savedq);
    }
}


///
///  @brief    Scan Q-register following command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_qreg(struct cmd *cmd)
{
    static const char *qnames = QNAMES "abcdefghijklmnopqrstuvwxyz";

    assert(cmd != NULL);

    const char *qname;
    int c = fetch_cbuf();

    if (c == '.')                       // Local Q-register?
    {
        cmd->qlocal = true;             // Yes, mark it

        c = fetch_cbuf();
    }

    cmd->qname = (char)c;               // Save the name

    if ((qname = strchr(qnames, cmd->qname)) != NULL)
    {
        cmd->qindex = qname - qnames;

        if (cmd->qindex >= QCOUNT)
        {
            cmd->qindex -= QCOUNT;
        }

        if (cmd->qlocal)
        {
            cmd->qindex += QCOUNT;
        }
    }
    else if ((cmd->c1 != 'G' && cmd->c1 != 'g') ||
             strchr("*_+", cmd->qname) == NULL)
    {
        throw(E_IQN, cmd->qname);       // Invalid Q-register name
    }
    else
    {
        cmd->qindex = -1;
    }
}


///
///  @brief    Store character in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_qchr(int qindex, int c)
{
    struct qreg *qreg = QREGISTER(qindex);

    free_mem(&qreg->text.data);

    qreg->text.pos  = 0;
    qreg->text.len  = 0;
    qreg->text.size = STR_SIZE_INIT;
    qreg->text.data = alloc_mem(qreg->text.size);

    qreg->text.data[qreg->text.len++] = (char)c;
}


///
///  @brief    Store number in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_qnum(int qindex, int n)
{
    struct qreg *qreg = QREGISTER(qindex);

    qreg->n = n;
}


///
///  @brief    Store (or free) text in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_qtext(int qindex, struct buffer *text)
{
    struct qreg *qreg = get_qreg(qindex);

    free_mem(&qreg->text.data);

    if (text == NULL)
    {
        qreg->text.size = 0;
        qreg->text.len  = 0;
        qreg->text.pos  = 0;
    }
    else
    {
        assert(text->size != 0);        // Error if no data for text

        qreg->text = *text;
    }
}
