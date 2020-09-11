///
///  @file    cmd_exec.c
///  @brief   Execute command string.
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
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "qreg.h"
#include "term.h"


uint nparens;                       ///< Parenthesis nesting count

///  @var    null_cmd
///
///  @brief  Initial command block values.

static const struct cmd null_cmd =
{
    .c1     = NUL,
    .c2     = NUL,
    .c3     = NUL,
    .qname  = NUL,
    .qlocal = false,
    .m_set  = false,
    .m_arg  = 0,
    .n_set  = false,
    .n_arg  = 0,
    .h      = false,
    .ctrl_y = false,
    .w      = false,
    .colon  = false,
    .dcolon = false,
    .atsign = false,
    .delim  = ESC,
    .text1  = { .data = NULL, .len = 0 },
    .text2  = { .data = NULL, .len = 0 },
};


// Local functions

static void check_qreg(const struct cmd_table *entry, struct cmd *cmd);

static void end_cmd(struct cmd *cmd, enum cmd_opts opts);

static exec_func *next_cmd(struct cmd *cmd);

static const struct cmd_table *scan_cmd(struct cmd *cmd);

static const struct cmd_table *scan_ef(struct cmd *cmd,
                                       const struct cmd_table *table,
                                       uint count, int error);

static void scan_text(int delim, struct tstring *text);

static void scan_texts(struct cmd *cmd, enum cmd_opts opts);


///
///  @brief    Check to see if we've already processed H or CTRL/Y.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void check_args(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (f.e2.args && (cmd->h || cmd->ctrl_y))
    {
        throw(E_ARG);                   // Improper arguments
    }
}


///
///  @brief    Check to see if command requires a Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void check_qreg(const struct cmd_table *entry, struct cmd *cmd)
{
    assert(entry != NULL);
    assert(cmd != NULL);

    if (!(entry->opts & OPT_Q))         // Does command require Q-register?
    {
        return;                         // No
    }

    int c = fetch_cbuf();               // Get Q-register (or dot)

    if (c == '.')                       // Is it local?
    {
        cmd->qlocal = true;             // Yes, mark it

        c = fetch_cbuf();               // Get Q-register name for real
    }

    // Q-registers must be alphanumeric. G commands also allow *, _, and $.

    if (!isalnum(c))
    {
        if (toupper(cmd->c1) != 'G' || strchr("*_$", c) == NULL)
        {
            throw(E_IQN, c);            // Illegal Q-register name
        }
    }

    cmd->qname = (char)c;               // Save the name
}


///
///  @brief    Check for modifiers and text strings after command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void end_cmd(struct cmd *cmd, enum cmd_opts opts)
{
    assert(cmd != NULL);                // Error if no command block

    if ((cmd->atsign && f.e2.atsign && !(opts & OPT_A)))
    {
        throw(E_ATS);                   // Illegal at-sign
    }

    if ((cmd->colon  && f.e2.colon  && !(opts & OPT_C)) ||
        (cmd->dcolon && f.e2.colon  && !(opts & OPT_D)))
    {
        throw(E_COL);                   // Illegal colon
    }

    // See if we have an n argument. If not, then check to see if the command
    // was preceded by a minus sign, which is equivalent to an argument of -1.

    cmd->n_set = pop_expr(&cmd->n_arg);

    if (cmd->n_set == false && unary_expr())
    {
        cmd->n_set = true;
        cmd->n_arg = -1;
    }

    // If we have an m argument, verify that it is valid for this command, and
    // that it is followed by an n argument.

    if (cmd->m_set)
    {
        if (f.e2.m_arg && !(opts & OPT_M))
        {
            throw(E_IMA);               // Illegal m argument
        }
        else if (!cmd->n_set)
        {
            throw(E_NON);               // No n argument after m argument
        }
    }
}


///
///  @brief    Execute command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_cmd(struct cmd *macro)
{
    struct cmd cmd = null_cmd;

    // If we were called from a macro, then copy any numeric arguments.

    if (macro != NULL)
    {
        if (macro->n_set)
        {
            push_expr(macro->n_arg, EXPR_VALUE);
        }

        cmd.m_set = macro->m_set;
        cmd.m_arg = macro->m_arg;
    }

    // Loop for all commands in command string.

    while (cbuf->len != 0)
    {
        exec_func *exec = next_cmd(&cmd);

        if (exec == NULL)
        {
            break;
        }

        int c = cmd.c1;

        (*exec)(&cmd);

        cmd = null_cmd;

        // Check for commands that allow numeric arguments to be passed
        // through to subsequent commands. This includes [ and ], but
        // also !, because it's sometimes useful to interpose comments
        // between two commands.

        if (strchr("![]", c) != NULL)
        {
            cmd.n_set = pop_expr(&cmd.n_arg);
            cmd.m_set = pop_expr(&cmd.m_arg);

            if (cmd.n_set)
            {
                push_expr(cmd.n_arg, EXPR_VALUE);
            }
        }

        if (f.e0.ctrl_c)                // If CTRL/C typed, return to main loop
        {
            f.e0.ctrl_c = false;

            throw(E_XAB);               // Execution aborted
        }
    }
}


///
///  @brief    Execute ESCape command. We're called here only for ESCapes
///            between commands, or at the end of command strings, not for
///            ESCapes used to delimit text arguments after commands.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_escape(struct cmd *unused1)
{
    // Skip past any whitespace after the ESCape.

    while (!empty_cbuf())
    {
        int c = peek_cbuf();

        if (!isspace(c) || c == TAB)    // Whitespace?
        {
            break;                      // No, so we're done skipping chrs.
        }

        (void)fetch_cbuf();
    }

    // If we've read all characters in command string, then reset for next time.

    if (empty_cbuf())
    {
        cbuf->pos = cbuf->len = 0;
    }
}


///
///  @brief    Scan command string for next command. Since many commands are
///            used only to create expressions (such as numeric arguments) for
///            other commands, we will continue looping here until we have a
///            complete command.
///
///  @returns  Command function to execute, or NULL if at end of command string.
///
////////////////////////////////////////////////////////////////////////////////

static exec_func *next_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    while (!empty_cbuf())
    {
        int c = fetch_cbuf();

        if (c == SPACE || c == LF || c == CR || c == FF || c == NUL)
        {
            continue;                   // Just skip the no-op commands
        }

        cmd->c1 = (char)c;

        const struct cmd_table *entry = scan_cmd(cmd);

        if (entry == NULL || entry->exec == NULL)
        {
            continue;
        }

        enum cmd_opts opts = entry->opts;

        if (opts & OPT_T1)
        {
            scan_texts(cmd, opts);      // Scan for text strings
        }

        // Handle commands that require special cases. These include A, which
        // is a simple command if it's preceded by an expression, but not a
        // colon, ^Q, which is a simple command if preceded by an expression,
        // and 'flag' commands (e.g, ET), which are simple commands if they
        // are NOT preceded by an expression.

        if (c == 'A' || c == 'a')
        {
            if (check_expr() && !cmd->colon)
            {
                end_cmd(cmd, opts);     // Do final check

                opts |= OPT_S;
            }
        }
        else if (c == CTRL_Q)
        {
            if (check_expr())
            {
                end_cmd(cmd, opts);     // Do final check
            }

            opts |= OPT_S;
        }
        else if ((entry->opts & OPT_F) && !check_expr())
        {
            opts |= OPT_S;
        }

        if (!(opts & OPT_S))            // Simple command?
        {
            end_cmd(cmd, entry->opts);  // Do final check

            return entry->exec;         // Tell caller to execute
        }

        (*entry->exec)(cmd);            // Execute and continue
    }

    // If we're not in a macro, then confirm that parentheses were properly
    // matched, and that there's nothing left on the expression stack.

    if (!check_macro())
    {
        // Here if we've reached the end of the command string.

        if (nparens)
        {
            throw(E_MRP);               // Missing right parenthesis
        }

        if (f.e2.args && estack.base != estack.level)
        {
            throw(E_ARG);               // Improper arguments
        }
    }

    return NULL;
}


///
///  @brief    Find table entry for command.
///
///  @returns  Table entry for function, or NULL if nothing to do.
///
////////////////////////////////////////////////////////////////////////////////

static const struct cmd_table *scan_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    int c = (char)cmd->c1;

    // Reset the fields that can change from command to command.

    cmd->c2     = NUL;
    cmd->c3     = NUL;
    cmd->qname  = NUL;
    cmd->qlocal = false;

    if (c == 'E' || c == 'e')
    {
        const struct cmd_table *entry = scan_ef(cmd, e_table, e_max, E_IEC);

        check_qreg(entry, cmd);

        return entry;
    }
    else if (c == 'F' || c == 'f')
    {
        const struct cmd_table *entry = scan_ef(cmd, f_table, f_max, E_IFC);

        //check_qreg(entry, cmd);       // (F commands don't allow Q-registers)

        return entry;
    }
    else if (c == 'P' || c == 'p')      // P may be followed by W
    {
        if (!empty_cbuf() && toupper(peek_cbuf()) == 'W')
        {
            (void)fetch_cbuf();

            cmd->w = true;
        }
    }
    else if (c < 0 || c >= (int)cmd_max)
    {
        throw(E_ILL, c);                // Illegal character
    }
    else if (c == '=')                  // Allow for =, ==, and ===
    {
        if (!empty_cbuf() && peek_cbuf() == '=')
        {
            (void)fetch_cbuf();

            cmd->c2 = cmd->c1;

            if (!empty_cbuf() && peek_cbuf() == '=')
            {
                (void)fetch_cbuf();

                cmd->c3 = cmd->c1;
            }
        }
    }
    else if (c == '"')                  // " requires additional character
    {
        cmd->c2 = (char)fetch_cbuf();
    }
    else if (c == '^' || c == '\x1E')
    {
        check_args(cmd);

        if (c == '\x1E' || (c = fetch_cbuf()) == '^')
        {
            c = fetch_cbuf();

            push_expr(c, EXPR_VALUE);

            return NULL;
        }

        c -= 'A' - 1;

        if (c <= NUL || c >= SPACE)
        {
            throw(E_IUC, c);            // Illegal character following ^
        }

        cmd->c1 = (char)c;
    }
    else if (c == '@')
    {
        if (cmd->atsign && f.e2.atsign)
        {
            throw(E_ATS);               // Illegal at-sign
        }

        cmd->atsign = true;
    }
    else if (cmd->c1 == ':')
    {
        if (peek_cbuf() == ':')         // Double colon?
        {
            (void)fetch_cbuf();         // Yes, count it

            if (cmd->dcolon && f.e2.colon)
            {
                throw(E_COL);           // Illegal colon
            }

            cmd->dcolon = true;         // And flag it
        }

        cmd->colon = true;              // Flag the first colon
    }
    else if (nparens != 0 && f.e1.xoper && exec_xoper(c))
    {
        if (c != '{' && c != '}')
        {
            check_args(cmd);
        }

        return NULL;
    }

    const struct cmd_table *entry = &cmd_table[c];

    if (entry->opts & OPT_B)
    {
        throw(E_ILL, c);                // Illegal character
    }

    check_qreg(entry, cmd);

    return entry;
}


///
///  @brief    Scan 2nd character for E or F command.
///
///  @returns  Table entry.
///
////////////////////////////////////////////////////////////////////////////////

static const struct cmd_table *scan_ef(struct cmd *cmd,
                                       const struct cmd_table *table,
                                       uint count, int error)
{
    assert(cmd != NULL);                // Error if no command block
    assert(table != NULL);              // Error if no command table pointer

    int c = fetch_cbuf();

    if (c < 0 || (uint)c > count || table[c].exec == NULL)
    {
        throw(error, c);                // Illegal E or F character
    }

    cmd->c2 = (char)c;

    return &table[c];
}


///
///  @brief    Scan the text string following the command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void scan_text(int delim, struct tstring *text)
{
    assert(text != NULL);

    text->data = cbuf->data + cbuf->pos;

    uint n = cbuf->len - cbuf->pos;
    char *end = memchr(text->data, delim, (ulong)n);

    if (end == NULL)
    {
        if (check_macro())
        {
            throw(E_UTM);               // Unterminated macro
        }
        else
        {
            throw(E_UTC);               // Unterminated command
        }
    }

    text->len = (uint)(end - text->data);

    cbuf->pos += text->len + 1;
}


///
///  @brief    Scan for text strings following command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void scan_texts(struct cmd *cmd, enum cmd_opts opts)
{
    assert(cmd != NULL);                // Error if no command block

    // Here to check for text arguments. The standard delimiter is ESCape,
    // except for CTRL/A and ! commands. If an at-sign was specified, then
    // the (non-whitespace) delimiter follows the command.

    if (cmd->c1 == CTRL_A)
    {
        cmd->delim = CTRL_A;            // ^A normally ends with ^A
    }
    else if (cmd->c1 == '!')
    {
        // If feature enabled, !! starts a comment that ends with LF.

        if (f.e1.bang && !empty_cbuf() && peek_cbuf() == '!')
        {
            (void)fetch_cbuf();

            cmd->delim = LF;            // Tag goes to end of line
        }
        else
        {
            cmd->delim = '!';           // ! normally ends with !
        }
    }
    else
    {
        cmd->delim = ESC;               // Standard delimiter
    }

    // If the user specified the at-sign modifier, then skip any whitespace
    // between the command and the delimiter.

    if (cmd->atsign)                    // @ modifier?
    {
        while (!empty_cbuf() && isspace(peek_cbuf()))
        {
            (void)fetch_cbuf();
        }

        // Treat first non-whitespace character as text delimiter.

        cmd->delim = (char)fetch_cbuf();
    }

    if (cmd->delim != '{' || !f.e1.text)
    {
        scan_text(cmd->delim, &cmd->text1);

        if (opts & OPT_T2)
        {
            scan_text(cmd->delim, &cmd->text2);
        }

        return;
    }

    // Here if user wants to delimit text string(s) with braces. This means
    // the text strings may be of the form {xxx}, and may include whitespace.
    // This allows for commands such as @S {foo} or @FS {foo} {baz}.

    scan_text('}', &cmd->text1);

    if (!(opts & OPT_T2))
    {
        return;
    }

    // Skip any whitespace after '}'

    while (!empty_cbuf() && isspace(peek_cbuf()))
    {
        (void)fetch_cbuf();
    }

    int c = fetch_cbuf();

    scan_text(c == '{' ? '}' : c, &cmd->text2);
}


///
///  @brief    Scan command string for next command. Since many commands are
///            used only to create expressions (such as numeric arguments) for
///            other commands, we will continue looping here until we have a
///            complete command.
///
///            The skip parameter determines whether we just ignore commands
///            until we find one that matches one of the characters in the
///            string, at which time we return to the caller. This is used for
///            branch and loop commands such as ", F>, and O.
///
///  @returns  true if found another command, false if at end of command string.
///
////////////////////////////////////////////////////////////////////////////////

bool skip_cmd(struct cmd *cmd, const char *skip)
{
    assert(cmd != NULL);
    assert(skip != NULL);

    *cmd = null_cmd;

    // Some of the commands we parse may push operands or operators on the
    // stack. Since we don't need those when we find the command we want,
    // we just reset the expression stack when we return. If an error occurs,
    // the entire stack will be reset elsewhere.

    uint saved_level = estack.level;

    while (!empty_cbuf())
    {
        int c = fetch_cbuf();

        if (c == SPACE || c == LF || c == CR || c == FF || c == NUL)
        {
            continue;                   // Just skip the no-op commands
        }

        cmd->c1 = (char)c;

        const struct cmd_table *entry = scan_cmd(cmd);

        if (entry == NULL || entry->exec == NULL)
        {
            continue;
        }

        if (entry->opts & OPT_T1)
        {
            scan_texts(cmd, entry->opts); // Scan for text strings
        }

        if (strchr(skip, cmd->c1) != NULL)
        {
            estack.level = saved_level;

            return true;                // Found what we were looking for
        }

        if (!(entry->opts & OPT_S))     // If not simple command,
        {
            *cmd = null_cmd;            //  then reset
        }
    }

    estack.level = saved_level;

    return false;
}
