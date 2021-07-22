///
///  @file    search.h
///  @brief   Header file for TECO search functions.
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

#if     !defined(_SEARCH_H)
#define _SEARCH_H

#include <stdbool.h>            //lint !e451 !e537
#include <sys/types.h>          //lint !e451 !e537

#include "teco.h"               //lint !e451 !e537
#include "exec.h"               //lint !e451 !e537


///  @enum   search_type
///  @brief  Type of search requested.

enum search_type
{
    SEARCH_S = 1,                   ///< Local search within page
    SEARCH_N = 2,                   ///< Non-stop search
    SEARCH_U = 3,                   ///< Search with yank protection
    SEARCH_E = 4                    ///< Search w/o yank protection
};

///  @struct  search
///  @brief   Common block used by all search functions.

struct search
{
    enum search_type type;              ///< Search type
    bool (*search)(struct search *s);   ///< Search function
    int_t count;                        ///< No. of iterations for search
    int_t text_start;                   ///< Start search at this position
    int_t text_end;                     ///< End search at this position
    int_t text_pos;                     ///< Position of string relative to dot
    uint_t match_len;                   ///< No. of characters left to match
    const char *match_buf;              ///< Next character to match
};

// Global variables

extern struct tstring last_search;

// Global functions

extern bool search_loop(struct search *s);

extern bool search_backward(struct search *s);

extern void search_failure(struct cmd *cmd);

extern bool search_forward(struct search *s);

extern void search_success(struct cmd *cmd);

#endif  // !defined(_SEARCH_H)
