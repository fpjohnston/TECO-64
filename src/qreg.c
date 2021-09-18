///
///  @file    qreg.c
///  @brief   Q-register functions for TECO.
///
///  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errcodes.h"
#include "exec.h"
#include "qreg.h"
#include "term.h"

#include "cbuf.h"

///  @var    qtable
///  @brief  Table to convert a Q-register name to the corresponding index.

static char qtable[] =
{
    ['0'] = 1 + 0,                  //lint !e835
    ['1'] = 1 + 1,
    ['2'] = 1 + 2,
    ['3'] = 1 + 3,
    ['4'] = 1 + 4,
    ['5'] = 1 + 5,
    ['6'] = 1 + 6,
    ['7'] = 1 + 7,
    ['8'] = 1 + 8,
    ['9'] = 1 + 9,
    ['A'] = 1 + 10,
    ['a'] = 1 + 10,
    ['B'] = 1 + 11,
    ['b'] = 1 + 11,
    ['C'] = 1 + 12,
    ['c'] = 1 + 12,
    ['D'] = 1 + 13,
    ['d'] = 1 + 13,
    ['E'] = 1 + 14,
    ['e'] = 1 + 14,
    ['F'] = 1 + 15,
    ['f'] = 1 + 15,
    ['G'] = 1 + 16,
    ['g'] = 1 + 16,
    ['H'] = 1 + 17,
    ['h'] = 1 + 17,
    ['I'] = 1 + 18,
    ['i'] = 1 + 18,
    ['J'] = 1 + 19,
    ['j'] = 1 + 19,
    ['K'] = 1 + 20,
    ['k'] = 1 + 20,
    ['L'] = 1 + 21,
    ['l'] = 1 + 21,
    ['M'] = 1 + 22,
    ['m'] = 1 + 22,
    ['N'] = 1 + 23,
    ['n'] = 1 + 23,
    ['O'] = 1 + 24,
    ['o'] = 1 + 24,
    ['P'] = 1 + 25,
    ['p'] = 1 + 25,
    ['Q'] = 1 + 26,
    ['q'] = 1 + 26,
    ['R'] = 1 + 27,
    ['r'] = 1 + 27,
    ['S'] = 1 + 28,
    ['s'] = 1 + 28,
    ['T'] = 1 + 29,
    ['t'] = 1 + 29,
    ['U'] = 1 + 30,
    ['u'] = 1 + 30,
    ['V'] = 1 + 31,
    ['v'] = 1 + 31,
    ['W'] = 1 + 32,
    ['w'] = 1 + 32,
    ['X'] = 1 + 33,
    ['x'] = 1 + 33,
    ['Y'] = 1 + 34,
    ['y'] = 1 + 34,
    ['Z'] = 1 + 35,
    ['z'] = 1 + 35,

    // The following are special for G commands

    ['+'] = -1,
    ['*'] = -1,
    ['_'] = -1,
};


#define QLOCAL_MAX      64          ///< Maximum local Q-register nesting
#define QSTACK_MAX      64          ///< Maximum Q-register stack depth

static uint qlocal_depth = 0;       ///< Current local Q-register nesting

static uint qstack_depth = 0;       ///< Current Q-register stack depth

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

///  @var    local_base
///  @brief  Local Q-register set used at prompt level.

static struct qlocal local_base;

///  @var    local_head
///  @brief  Head of local Q-register set linked list.

static struct qlocal *local_head = &local_base;

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

static inline struct qreg *qregister(int qindex);


///
///  @brief    Append character to Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void append_qchr(int qindex, int c)
{
    struct qreg *qreg = qregister(qindex);

    if (qreg->text.data == NULL)
    {
        qreg->text.pos  = 0;
        qreg->text.len  = 0;
        qreg->text.size = KB;
        qreg->text.data = alloc_mem((uint_t)qreg->text.size);
    }
    else
    {
        if (qreg->text.len == qreg->text.size)
        {
            qreg->text.data = expand_mem(qreg->text.data, qreg->text.size, KB);
            qreg->text.size += KB;
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
    struct qreg *qreg = qregister(qindex);

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

void exit_qreg(void)
{
    reset_qreg();                       // Free local Q-registers.

    // Free up what's on the Q-register push-down list.

    struct qlist *savedq;

    while ((savedq = list_head) != NULL)
    {
        list_head = savedq->next;

        free_mem(&savedq->qreg.text.data);
        free_mem(&savedq);
    }

    qstack_depth = 0;

    // The prompt level set of local Q-registers is not freed by reset_qreg(),
    // so we have to do that ourselves.

    if (local_head != NULL)
    {
        assert(local_head->next == NULL); // Error if more than one set

        for (uint i = 0; i < QCOUNT; ++i)
        {
            if (local_head->qreg[i].text.data != NULL)
            {
                free_mem(&local_head->qreg[i].text.data);
            }
        }
    }

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

uint_t get_qall(void)
{
    uint_t n = 0;
    struct qreg *qreg;

    // Get count of all text in global Q-registers

    for (int_t i = 0; i < QCOUNT; ++i)
    {
        qreg = &qglobal[i];
        n += qreg->text.len;
    }

    struct qlocal *qnext = local_head;

    // Get count of all text in local Q-registers

    while (qnext != NULL)
    {
        for (int_t i = 0; i < QCOUNT; ++i)
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

    return (uint_t)n;
}


///
///  @brief    Get ASCII value of nth character in Q-register text string.
///
///  @returns  ASCII value of character, or -1 if not found.
///
////////////////////////////////////////////////////////////////////////////////

int get_qchr(int qindex, uint n)
{
    struct qreg *qreg = qregister(qindex);

    if (n >= qreg->text.len)            // Out of range?
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
    int qindex;

    if ((uint)qname > countof(qtable) || (qindex = qtable[qname]) <= 0)
    {
        return -1;
    }
    else if (qlocal)
    {
        qindex += QCOUNT;
    }

    return --qindex;                    // Make it zero-based
}


///
///  @brief    Get number in Q-register.
///
///  @returns  Q-register number.
///
////////////////////////////////////////////////////////////////////////////////

int_t get_qnum(int qindex)
{
    struct qreg *qreg = qregister(qindex);

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

uint_t get_qsize(int qindex)
{
    struct qreg *qreg = qregister(qindex);

    return (uint_t)qreg->text.len;
}


///
///  @brief    Initialize Q-register storage.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_qreg(void)
{
    memset(&local_base, '\0', sizeof(local_base));
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

    --qlocal_depth;
}


///
///  @brief    Pop Q-register from push-down list.
///
///  @returns  true if success, false if push-down list is empty.
///
////////////////////////////////////////////////////////////////////////////////

bool pop_qreg(int qindex)
{
    struct qreg *qreg = qregister(qindex);

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

    --qstack_depth;

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
    struct qreg *qreg = qregister(qindex);

    for (uint_t i = 0; i < qreg->text.len; ++i)
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
    if (qlocal_depth == QLOCAL_MAX)
    {
        throw(E_MAX);
    }

    ++qlocal_depth;

    struct qlocal *qlocal = alloc_mem((uint_t)sizeof(*qlocal));

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
    if (qstack_depth == QSTACK_MAX)
    {
        throw(E_MAX);
    }

    ++qstack_depth;

    struct qreg *qreg    = qregister(qindex);
    struct qlist *savedq = alloc_mem((uint_t)sizeof(*savedq));

    savedq->qreg.n         = qreg->n;
    savedq->qreg.text.size = qreg->text.size;
    savedq->qreg.text.pos  = qreg->text.pos;
    savedq->qreg.text.len  = qreg->text.len;

    if (savedq->qreg.text.size == 0)
    {
        savedq->qreg.text.data = NULL;
    }
    else
    {
        savedq->qreg.text.data = alloc_mem(savedq->qreg.text.size);

        memcpy(savedq->qreg.text.data, qreg->text.data,
               (size_t)savedq->qreg.text.size);
    }

    savedq->next = list_head;

    list_head = savedq;

    return true;
}


///
///  @brief    Get pointer to Q-register data structure.
///
///  @returns  Pointer to data.
///
////////////////////////////////////////////////////////////////////////////////

static inline struct qreg *qregister(int qindex)
{
    if (qindex < QCOUNT)
    {
        return &qglobal[qindex];
    }
    else
    {
        return &local_head->qreg[qindex - QCOUNT];
    }
}


///
///  @brief    Free local Q-registers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_qreg(void)
{
    if (local_head != NULL)
    {
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
    }

    qlocal_depth = 0;
}


///
///  @brief    Scan Q-register following command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_qreg(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c = require_cbuf();

    trace_cbuf(c);

    if (c == '.')                       // Local Q-register?
    {
        cmd->qlocal = true;             // Yes, mark it

        c = require_cbuf();

        trace_cbuf(c);
    }

    cmd->qname = (char)c;               // Save the name

    if ((uint)c > countof(qtable) || (cmd->qindex = qtable[c]) == 0)
    {
        if (cmd->qname == ESC)          // ESCape means no Q-register
        {
            throw(E_MQN);               // Missing Q-register name
        }
        else
        {
            throw(E_IQN, cmd->qname);   // Invalid Q-register name
        }
    }

    if (cmd->qindex == -1)              // Special Q-registers?
    {
        if (toupper(cmd->c1) == 'G')    // Yes, is this a G command?
        {
            return;                     // Yes
        }

        throw(E_IQN, cmd->qname);       // Invalid Q-register name
    }
    else if (cmd->qlocal)
    {
        cmd->qindex += QCOUNT;
    }

    --cmd->qindex;                      // Make it zero-based
}


///
///  @brief    Store character in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_qchr(int qindex, int c)
{
    struct qreg *qreg = qregister(qindex);

    free_mem(&qreg->text.data);

    qreg->text.pos  = 0;
    qreg->text.len  = 0;
    qreg->text.size = KB;
    qreg->text.data = alloc_mem(qreg->text.size);

    qreg->text.data[qreg->text.len++] = (char)c;
}


///
///  @brief    Store number in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_qnum(int qindex, int_t n)
{
    struct qreg *qreg = qregister(qindex);

    qreg->n = n;
}


///
///  @brief    Store text in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_qtext(int qindex, tbuffer *text)
{
    assert(text != NULL);
    assert(text->size != 0);            // Error if no data for text

    struct qreg *qreg = get_qreg(qindex);

    free_mem(&qreg->text.data);

    qreg->text = *text;
}
