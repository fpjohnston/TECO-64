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

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "teco.h"
#include "ascii.h"
#include "errors.h"

#define NDIGITS    ('9' - '0' + 1)
#define QREG_COUNT (NDIGITS + (('Z' - 'A') + 1))

static struct qreg q_global[] = { [0 ... QREG_COUNT] = { NULL, NULL, 0 } };
static struct qreg q_local[]  = { [0 ... QREG_COUNT] = { NULL, NULL, 0 } };

struct qreg *qreg = NULL;

// Local functions

static void free_qreg(void);


///
///  @brief    Free Q-register storage.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void free_qreg(void)
{
    print_term("<Q-registers deallocated>");

    for (int i = 0; i < QREG_COUNT; ++i)
    {
        struct qreg *reg;

        // Free the global Q-registers

        reg = &q_global[i];

        if (reg->start != NULL)
        {
            free(reg->start);

            reg->start = reg->end = NULL;
        }

        // Free the main-level local Q-registers

        reg = &q_local[i];

        if (reg->start != NULL)
        {
            free(reg->start);

            reg->start = reg->end = NULL;
        }
    }
}


///
///  @brief    Get pointer to specified Q-register structure.
///
///  @returns  Q-register pointer.
///
////////////////////////////////////////////////////////////////////////////////

struct qreg *get_qreg(int qname, bool qdot)
{
    int qindex;

    if (isdigit(qname))
    {
        qindex = qname - '0';
    }
    else if (isalpha(qname))
    {
        qname = toupper(qname);
        qindex = NDIGITS + (qname - 'A');
    }
    else
    {
        putc_term(CRLF);
        printc_err(E_IQN, qname);
    }

    if (qdot)                           // Local Q-register?
    {
        return qreg = &q_local[qindex]; // Set local Q-register
    }
    else
    {
        return qreg = &q_global[qindex]; // Set global Q-register
    }

    // TODO: Add code to check to see if we're in a macro.
    //       Use a local Q-reg. if so.

#if     0
        if (MStTop >= 0)                // If in a macro
        {
            int TMIndx;
            QRptr QRp;

            for (TMIndx = MStTop; TMIndx > 0; TMIndx--)
            {
                if (MStack[TMIndx].QRgstr != NULL)
                {
                    QRp = MStack[TMIndx].QRgstr;

                    break;
                }
            }

            QR = (TMIndx != 0) ?        // if we found one
                 &QRp[QIndex]:          // use the one we found
                 (QRptr) &QRgstr[QIndex+36]; // else use main level ones
        }
#endif

}


///
///  @brief    Initialize Q-register storage.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_qreg(void)
{
    if (atexit(free_qreg) != 0)
    {
        exit(EXIT_FAILURE);
    }
}
