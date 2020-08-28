///
///  @file    qreg.c
///  @brief   Q-register functions for TECO.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include "ascii.h"
#include "errors.h"
#include "exec.h"
#include "qreg.h"
#include "term.h"


///  @var    qglobal
///  @brief  Global Q-registers.

static struct qreg qglobal[QREG_SIZE];

///  @struct qlocal
///  @brief  Local Q-register set.

struct qlocal
{
    struct qlocal *next;                ///< Next item in list
    struct qreg qreg[QREG_SIZE];        ///< Local Q-register set
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

void append_qchr(int qname, bool qdot, int c)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    if (qreg->text.data == NULL)
    {
        qreg->text.pos  = 0;
        qreg->text.len  = 0;
        qreg->text.size = STR_SIZE_INIT;
        qreg->text.data  = alloc_mem(qreg->text.size);
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

void delete_qtext(int qname, bool qdot)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    free_mem(&qreg->text.data);

    qreg->text.len = 0;
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

    for (uint i = 0; i < QREG_SIZE; ++i)
    {
        if (local_head->qreg[i].text.data != NULL)
        {
            free_mem(&local_head->qreg[i].text.data);
        }
    }

    free_mem(&local_head);

    // Free the global Q-registers

    for (uint i = 0; i < QREG_SIZE; ++i)
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

    for (uint i = 0; i < QREG_SIZE; ++i)
    {
        qreg = &qglobal[i];
        n += qreg->text.len;
    }

    struct qlocal *qnext = local_head;

    // Get count of all text in local Q-registers

    while (qnext != NULL)
    {
        for (uint i = 0; i < QREG_SIZE; ++i)
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

int get_qchr(int qname, bool qdot, int n)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    if (n < 0 || (uint)n >= qreg->text.len) // Out of range?
    {
        return EOF;                     // Yes
    }

    return qreg->text.data[n];
}


///
///  @brief    Get Q-register name for command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void get_qname(struct cmd *cmd, const char *extras)
{
    assert(cmd != NULL);                // Error if no command block

    check_end();

    int c = fetch_cbuf();               // Get Q-register (or dot)

    if (c == '.')                       // Is it local?
    {
        cmd->qlocal = true;             // Yes, mark it

        check_end();

        c = fetch_cbuf();               // Get Q-register name for real
    }

    if (!isalnum(c) && (extras == NULL || strchr(extras, c) == NULL))
    {
        throw(E_IQN, c);                // Illegal Q-register name
    }

    cmd->qname = (char)c;               // Save the name
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
    static const char *qchars = QCHARS;

    const char *qchar = strchr(qchars, toupper(qname));

    if (qchar == NULL)
    {
        throw(E_IQN, qname);            // Illegal Q-register name
    }

    uint qindex = (uint)(qchar - qchars);
    struct qreg *qreg;

    if (qdot)                           // Local Q-register?
    {
        qreg = &local_head->qreg[qindex];
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

    for (uint i = 0; i < QREG_SIZE; ++i)
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

bool pop_qreg(int qname, bool qdot)
{
    struct qreg *qreg = get_qreg(qname, qdot);

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

void print_qreg(int qname, bool qdot)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    for (uint i = 0; i < qreg->text.len; ++i)
    {
        int c = qreg->text.data[i];

        if (c == LF)
        {
            print_chr(CRLF);
        }
        else
        {
            echo_out(c);
        }
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

bool push_qreg(int qname, bool qdot)
{
    struct qreg *qreg = get_qreg(qname, qdot);
    struct qlist *savedq = alloc_mem((uint)sizeof(*savedq));

    savedq->qreg.n         = qreg->n;
    savedq->qreg.text.size = qreg->text.size;
    savedq->qreg.text.pos  = qreg->text.pos;
    savedq->qreg.text.len  = qreg->text.len;
    savedq->qreg.text.data  = alloc_mem(savedq->qreg.text.size);

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

        for (uint i = 0; i < QREG_SIZE; ++i)
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
///  @brief    Store character in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_qchr(int qname, bool qdot, int c)
{
    struct qreg *qreg = get_qreg(qname, qdot);

    free_mem(&qreg->text.data);

    qreg->text.pos  = 0;
    qreg->text.len  = 0;
    qreg->text.size = STR_SIZE_INIT;
    qreg->text.data  = alloc_mem(qreg->text.size);

    qreg->text.data[qreg->text.len++] = (char)c;
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

void store_qtext(int qname, bool qdot, struct buffer *text)
{
    assert(text != NULL);               // Error if no place to store text
    assert(text->size != 0);            // Error if no data for text

    struct qreg *qreg = get_qreg(qname, qdot);

    free_mem(&qreg->text.data);

    qreg->text = *text;
}
