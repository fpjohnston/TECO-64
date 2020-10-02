################################################################################
#
#  Makefile for TECO-64 text editor
#
#  Copyright (C) 2019-2020 Franklin P. Johnston / Nowwith Treble Software
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.
#
#  Build targets:
#
#      all      Equivalent to 'teco' target. [default]"
#      clean    Clean object files."
#      clobber  Clean everything."
#      doc      Equivalent to 'doxygen' target."
#      doxygen  Update Doxygen documentation."
#      help     Print help message."
#      lint     Lint .c and .lob files (requires PC-lint)."
#      lobs     Lint .c files (requires PC-lint)."
#      scratch  Equivalent to 'clobber' and 'all' targets."
#      teco     Build TECO-64 text editor."
#
#  Build options:
#
#      BUFFER=gap  Use gap buffer for editing text. [default]"
#      DEBUG=1     Enable debugging features."
#      GDB=1       Enable use of GDB debugger."
#      GPROF=1     Enable use of GPROF profiler."
#      NDEBUG=1    Disable run-time assertions."
#      PAGING=std  Use standard paging. [default]"
#      PAGING=vm   Use virtual memory paging."
#      TRACE=1     Enable tracing of commands."
#      VERBOSE=1   Enable verbosity during build."
#      WINDOWS=1   Enable windows commands."
#
################################################################################

TARGET = teco

CC = gcc

CFLAGS = -std=gnu11 -Wall -Wextra -Wno-unused-parameter -fshort-enums

DFLAGS =

OPTIONS_DEBUG =

INCDIR = include

OPT ?= 3
OPT_OPT = -O$(OPT)

OPTIONS_H = $(INCDIR)/options.h

INCLUDES = -I ../$(INCDIR)

VPATH=src:obj:$(INCDIR)

LIBS =

SOURCES = \
    build_str.c    \
    cmd_buf.c      \
    cmd_estack.c   \
    cmd_exec.c     \
    cmd_tables.c   \
    env_sys.c      \
    errors.c       \
    file.c         \
    file_sys.c     \
    memory.c       \
    option_sys.c   \
    qreg.c         \
    search.c       \
    teco.c         \
    term_buf.c     \
    term_in.c      \
    term_out.c     \
    term_sys.c     \
    window.c       \
                   \
    a_cmd.c        \
    bracket_cmd.c  \
    bslash_cmd.c   \
    case_cmd.c     \
    color_cmd.c    \
    ctrl_a_cmd.c   \
    ctrl_c_cmd.c   \
    ctrl_t_cmd.c   \
    ctrl_u_cmd.c   \
    ctrl_v_cmd.c   \
    ctrl_w_cmd.c   \
    datetime_cmd.c \
    delete_cmd.c   \
    ea_cmd.c       \
    eb_cmd.c       \
    ec_cmd.c       \
    ef_cmd.c       \
    eg_cmd.c       \
    ei_cmd.c       \
    ek_cmd.c       \
    el_cmd.c       \
    en_cmd.c       \
    eo_cmd.c       \
    ep_cmd.c       \
    eq_cmd.c       \
    equals_cmd.c   \
    er_cmd.c       \
    e_pct_cmd.c    \
    e_ubar_cmd.c   \
    ew_cmd.c       \
    ex_cmd.c       \
    fb_cmd.c       \
    fd_cmd.c       \
    fk_cmd.c       \
    flag_cmd.c     \
    fr_cmd.c       \
    g_cmd.c        \
    goto_cmd.c     \
    if_cmd.c       \
    insert_cmd.c   \
    loop_cmd.c     \
    m_cmd.c        \
    move_cmd.c     \
    n_cmd.c        \
    number_cmd.c   \
    oper_cmd.c     \
    p_cmd.c        \
    pct_cmd.c      \
    q_cmd.c        \
    radix_cmd.c    \
    s_cmd.c        \
    trace_cmd.c    \
    type_cmd.c     \
    ubar_cmd.c     \
    u_cmd.c        \
    var_cmd.c      \
    w_cmd.c        \
    x_cmd.c        \
    xoper_cmd.c    \
    yank_cmd.c     \

#
#  Check to see if we should include windows support
#
################################################################################

ifdef   WINDOWS

CFLAGS     += -D TECO_WINDOWS
LINT_DEBUG += -D TECO_WINDOWS

endif

#
#  Check to see which buffer handler we should use
#
################################################################################

ifeq (${BUFFER}, rope)

$(error Rope buffer handler is not yet implemented)

SOURCES += rope_buf.c

else ifeq (${BUFFER}, gap)

SOURCES += gap_buf.c

else ifeq (${BUFFER}, )

SOURCES += gap_buf.c

else

$(error Unknown buffer handler: ${BUFFER})

endif

#
#  Check to see which paging handler we should use
#
################################################################################

ifeq (${PAGING}, vm)

SOURCES += page_vm.c

else ifeq (${PAGING}, file)

$(error Holding file paging is not yet implemented)

SOURCES += page_file.c

else ifeq (${PAGING}, standard)

SOURCES += page_std.c

else ifeq (${PAGING}, )

SOURCES += page_std.c

else

$(error Unknown paging handler: ${PAGING})

endif

ifdef   VERBOSE

AT =
NULL = >/dev/null 2>&1
NULL2 = 

else

AT = @
NULL =
NULL2 = >/dev/null 2>&1

endif

ifdef   DEBUG

CFLAGS += -D TECO_DEBUG
OPTIONS_DEBUG += -d
LINT_DEBUG += -DTECO_DEBUG

endif

ifdef   GDB

DFLAGS += -g
OPT_OPT = -O0

endif

ifdef   GPROF

DFLAGS += -pg
OPT_OPT = -O0

endif

ifdef   NDEBUG

CFLAGS += -DNDEBUG

endif

ifdef   TRACE

CFLAGS += -D TECO_TRACE
LINT_DEBUG += -DTECO_TRACE

endif

LOBS = $(SOURCES:.c=.lob)

OBJECTS = $(SOURCES:.c=.o)

DFILES = $(SOURCES:.c=.d)

CFLAGS += -MMD -c $(INCLUDES) $(OPT_OPT) $(DFLAGS)

LINT = flint -b -zero -i$(HOME)/flint/lnt $(LINT_DEBUG) ../etc/std.lnt \
             -e126 -e786 -e818 -e830 -e843 -e844 +fan +fas

.PHONY: all
all: $(TARGET)

.PHONY: scratch
scratch: clobber all

.PHONY: help
help:
	@echo "Build targets:"
	@echo ""
	@echo "    all      Equivalent to 'teco' target. [default]"
	@echo "    clean    Clean object files."
	@echo "    clobber  Clean everything."
	@echo "    doc      Equivalent to 'doxygen' target."
	@echo "    doxygen  Update Doxygen documentation."
	@echo "    help     Print this message."
	@echo "    lint     Lint .c and .lob files (requires PC-lint)."
	@echo "    lobs     Lint .c files (requires PC-lint)."
	@echo "    options  Print build options."
	@echo "    scratch  Equivalent to 'clobber' and 'all' targets."
	@echo "    teco     Build TECO-64 text editor."
	@echo ""
	@echo "Build options:"
	@echo ""
	@echo "    BUFFER=gap  Use gap buffer for editing text. [default]"
	@echo "    DEBUG=1     Enable debugging features."
	@echo "    GDB=1       Enable use of GDB debugger."
	@echo "    GPROF=1     Enable use of GPROF profiler."
	@echo "    NDEBUG=1    Disable run-time assertions."
	@echo "    PAGING=std  Use standard paging. [default]"
	@echo "    PAGING=vm   Use virtual memory paging."
	@echo "    TRACE=1     Enable tracing of commands."
	@echo "    VERBOSE=1   Enable verbosity during build."
	@echo "    WINDOWS=1   Enable windows commands."

.PHONY: init

init: bin obj
	$(AT)chmod ugo+x etc/options.pl

bin:
	$(AT)mkdir -p bin

obj:
	$(AT)mkdir -p obj

.PHONY: $(TARGET) 
$(TARGET): bin/$(TARGET)

bin/$(TARGET): $(OBJECTS)
	@echo Making $(@F) $(NULL)
	$(AT)cd obj && $(CC) $(DFLAGS) -o ../$@ $(OBJECTS) $(LIBS) -lncurses

%.lob: %.c
	@echo Making $@ $(NULL)
	$(AT)cd src && $(LINT) -u $(INCLUDES) -oo\(../obj/$@\) ../$<

%.o: %.c
	@echo Making $@ $(NULL)
	$(AT)cd obj && $(CC) @../obj/CFLAGS ../$<

-include $(DFILES)

$(OBJECTS): $(OPTIONS_H) obj/CFLAGS

$(OPTIONS_H): etc/options.xml
	$(AT)etc/options.pl -c $< -o $@ $(OPTIONS_BUG)

.PHONY: FORCE
obj/CFLAGS: FORCE
	-$(AT)echo '$(CFLAGS)' | cmp -s - $@ || echo '$(CFLAGS)' > $@

.PHONY: clean
clean:
	-$(AT)cd bin && rm -f $(TARGET) $(TARGET).map $(NULL2)
	-$(AT)cd obj && rm -f *.o $(NULL2)

.PHONY: clobber
clobber: clean
	-$(AT)rm -f obj/CFLAGS $(NULL2) 
	-$(AT)rm -rf html $(NULL2) 
	-$(AT)cd obj && rm -f *.d *.lob $(NULL2)
	-$(AT)cd src && rm -f *.bak $(NULL2)
	-$(AT)cd $(INCDIR) && rm -f *.bak $(NULL2)

.PHONY: lobs
lobs: $(OPTIONS_H) $(LOBS)

.PHONY: lint
lint:   $(OPTIONS_H) $(LOBS)
	@echo Linting object files $(NULL)
	$(AT)cd obj && $(LINT) -e768 -e769 -summary *.lob

.PHONY: doc
doc: doxygen html

.PHONY: doxygen
doxygen:
	-$(AT)echo "Making Doxygen documents" $(NULL)
	-$(AT)doxygen etc/Doxyfile

.PHONY: html
html: html/options.html

html/options.html: etc/options.xml etc/options.xsl
	-$(AT)echo "Making HTML options file" $(NULL)
	$(AT)xalan -in etc/options.xml -xsl etc/options.xsl -out html/options.html
