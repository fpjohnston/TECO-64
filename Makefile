################################################################################
#
#  Makefile for TECO-64 text editor
#
#  Copyright (C) 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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
#      all          Equivalent to 'teco' target. [default]
#      clean        Clean object files and executables.
#      distclean    Clean everything.
#      doc          Create or update documentation.
#      help         Print help message.
#      lint         Lint .c and .lob files (requires PC-lint).
#      lobs         Lint .c files (requires PC-lint).
#      mostlyclean  Clean object files.
#      scratch      Equivalent to 'distclean' and 'all' targets.
#      teco         Build TECO-64 text editor.
#
#  Build options:
#
#      buffer=gap  Use gap buffer for editing text. [default]
#      display=1   Enable display mode.
#      gdb=1       Enable use of GDB debugger.
#      gprof=1     Enable use of GPROF profiler.
#      long=1      Use 64-bit integers.
#      memcheck=1  Enable checks for memory leaks.
#      ndebug=1    Disable run-time assertions.
#      nostrict=1  Relax run-time syntax checking.
#      paging=std  Use standard paging. [default]
#      paging=vm   Use virtual memory paging.
#      trace=1     Enable tracing of commands.
#      verbose=1   Enable verbosity during build.
#
################################################################################

TARGET = teco

CC = gcc

CFLAGS = -std=gnu11 -Wall -Wextra -Wno-unused-parameter -fshort-enums

INCDIR = include

OPT ?= 3
OPT_OPT = -O$(OPT)

COMMANDS_H  = $(INCDIR)/commands.h
OPTIONS_H   = $(INCDIR)/options.h
ERRCODES_H  = $(INCDIR)/errcodes.h
ERRTABLES_H = $(INCDIR)/errtables.h
EXEC_H      = $(INCDIR)/exec.h

ERRORS_MD   = doc/errors.md

INCLUDES = -I ../$(INCDIR)

VPATH=src:obj:$(INCDIR)

LIBS =

SOURCES = \
    build_str.c    \
    cmd_buf.c      \
    cmd_estack.c   \
    cmd_exec.c     \
    cmd_scan.c     \
    display.c      \
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
                   \
    a_cmd.c        \
    bracket_cmd.c  \
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
    em_cmd.c       \
    en_cmd.c       \
    eo_cmd.c       \
    ep_cmd.c       \
    e_pct_cmd.c    \
    eq_cmd.c       \
    equals_cmd.c   \
    er_cmd.c       \
    esc_cmd.c      \
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
    map_cmd.c      \
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
    yank_cmd.c     \

#
#  Check to see if we should enable display mode.
#
################################################################################

ifdef   display

DEFINES += -D DISPLAY_MODE
DOXYGEN +=    DISPLAY_MODE
LIBS    += -l ncurses
endif

#
#  Check to see which buffer handler we should use
#
################################################################################

ifeq (${buffer}, rope)

$(error Rope buffer handler is not yet implemented)

SOURCES += rope_buf.c

else ifeq (${buffer}, gap)

SOURCES += gap_buf.c

else ifeq (${buffer}, )

SOURCES += gap_buf.c

else

$(error Unknown buffer handler: ${buffer})

endif

#
#  Check to see which paging handler we should use
#
################################################################################

ifeq (${paging}, vm)

SOURCES += page_vm.c
DEFINES += -D PAGE_VM
DOXYGEN +=    PAGE_VM

else ifeq (${paging}, file)

$(error Holding file paging is not yet implemented)

SOURCES += page_file.c

else ifeq (${paging}, std)

SOURCES += page_std.c

else ifeq (${paging}, )

SOURCES += page_vm.c
DEFINES += -D PAGE_VM
DOXYGEN +=    PAGE_VM

else

$(error Unknown paging handler: ${paging})

endif

# Always enable ESCape as dollar sign regardless of alternate delimiter

# Change default for :U command

ifdef   default_u

DEFINES += -D DEFAULT_U=$(default_u)
DOXYGEN +=    DEFAULT_U

endif

# Always echo ESCape as dollar sign, regardless of alternate delimiters

ifdef   dollar

DEFINES += -D DOLLAR_ESC
DOXYGEN +=    DOLLAR_ESC

endif

# Enable obsolete EU command

ifdef   eu

DEFINES += -D EU_COMMAND
DOXYGEN +=    EU_COMMAND

endif

# Use 64-bit long integers

ifdef   long

DEFINES += -D LONG_64
DOXYGEN +=    LONG_64

endif

ifdef   verbose

AT =
NULL = >/dev/null 2>&1
NULL2 = 

else

AT = @
NULL =
NULL2 = >/dev/null 2>&1

endif

ifdef   memcheck

DEFINES += -D MEMCHECK
DOXYGEN +=    MEMCHECK
OPTIONS_DEBUG += -d

endif

ifdef   gdb

DFLAGS += -g
OPT_OPT = -O0

endif

ifdef   gprof

DFLAGS += -pg
OPT_OPT = -O3

endif

ifdef   ndebug

DEFINES += -D NDEBUG
DOXYGEN +=    NDEBUG

endif

ifdef   nostrict

DEFINES += -D NOSTRICT

endif

ifdef   trace

DEFINES += -D TECO_TRACE
DOXYGEN +=    TECO_TRACE

endif

LOBS = $(SOURCES:.c=.lob)

OBJECTS = $(SOURCES:.c=.o)

DFILES = $(SOURCES:.c=.d)

CFLAGS += -MMD -c $(INCLUDES) $(OPT_OPT) $(DFLAGS) $(DEFINES)

LINT = flint -b -zero -i$(HOME)/flint/lnt $(DEFINES) ../etc/std.lnt \
             -e126 -e786 -e818 -e830 -e843 -e844 +fan +fas

.PHONY: all
all: $(TARGET)

.PHONY: scratch
scratch: distclean all

.PHONY: help
help:
	@echo "Build targets:"
	@echo ""
	@echo "    all          Equivalent to 'teco' target. [default]"
	@echo "    clean        Clean object files and executables."
	@echo "    distclean    Clean everything."
	@echo "    doc          Create or update documentation."
	@echo "    help         Print this message."
	@echo "    lint         Lint .c and .lob files (requires PC-lint)."
	@echo "    lobs         Lint .c files (requires PC-lint)."
	@echo "    mostlyclean  Clean object files."
	@echo "    scratch      Equivalent to 'distclean' and 'all' targets."
	@echo "    teco         Build TECO-64 text editor."
	@echo ""
	@echo "Build options:"
	@echo ""
	@echo "    buffer=gap  Use gap buffer for editing text. [default]"
	@echo "    display=1   Enable display mode."
	@echo "    gdb=1       Enable use of GDB debugger."
	@echo "    gprof=1     Enable use of GPROF profiler."
	@echo "    long=1      Use 64-bit integers."
	@echo "    memcheck=1  Enable checks for memory leaks."
	@echo "    ndebug=1    Disable run-time assertions."
	@echo "    nostrict=1  Relax run-time syntax checking."
	@echo "    paging=std  Use standard paging. [default]"
	@echo "    paging=vm   Use virtual memory paging."
	@echo "    trace=1     Enable tracing of commands."
	@echo "    verbose=1   Enable verbosity during build."

bin:
	$(AT)mkdir -p bin

obj:
	$(AT)mkdir -p obj

.PHONY: $(TARGET) 
$(TARGET): bin/$(TARGET)

bin/$(TARGET): $(OBJECTS)
	@echo Making $(@F) $(NULL)
	$(AT)cd obj && $(CC) $(DFLAGS) -o ../$@ $(OBJECTS) $(LIBS)

%.lob: %.c
	@echo Making $@ $(NULL)
	$(AT)cd src && $(LINT) -u $(INCLUDES) -oo\(../obj/$@\) ../$<

%.o: %.c
	@echo Making $@ $(NULL)
	$(AT)cd obj && $(CC) @../obj/CFLAGS ../$<

-include $(DFILES)

$(OBJECTS): $(COMMANDS_H) $(ERRCODES_H) $(ERRTABLES_H) $(EXEC_H) $(OPTIONS_H) obj/CFLAGS

$(COMMANDS_H): etc/commands.xml etc/templates/commands.h etc/commands.pl
	$(AT)etc/commands.pl -i $< -t etc/templates/commands.h -o $@

$(ERRCODES_H): etc/errors.xml etc/templates/errcodes.h etc/errors.pl
	$(AT)etc/errors.pl -i $< -t etc/templates/errcodes.h -o $@

$(ERRTABLES_H): etc/errors.xml etc/templates/errtables.h etc/errors.pl
	$(AT)etc/errors.pl -i $< -t etc/templates/errtables.h -o $@

$(EXEC_H): etc/commands.xml etc/templates/exec.h etc/commands.pl
	$(AT)etc/commands.pl -i $< -t etc/templates/exec.h -o $@

$(OPTIONS_H): etc/options.xml etc/templates/options.h etc/options.pl
	$(AT)etc/options.pl -c $< -t etc/templates/options.h -o $@ $(OPTIONS_DEBUG)

.PHONY: FORCE
obj/CFLAGS: FORCE
	-$(AT)echo '$(CFLAGS)' | cmp -s - $@ || echo '$(CFLAGS)' > $@

.PHONY: clean
clean: mostlyclean
	-$(AT)cd bin && rm -f $(TARGET) $(TARGET).map $(NULL2)

.PHONY: distclean
distclean: mostlyclean clean
	-$(AT)rm -f obj/CFLAGS obj/Doxyfile $(NULL2) 
	-$(AT)rm -rf html $(NULL2) 
	-$(AT)cd src && rm -f *.bak $(NULL2)
	-$(AT)cd $(INCDIR) && rm -f *.bak $(NULL2)

.PHONY: doc
doc: html/options.html $(ERRORS_MD)
	-$(AT)echo "Making Doxygen documents" $(NULL)
	-$(AT)cp etc/Doxyfile obj/Doxyfile
	-$(AT)echo "PREDEFINED = $(DOXYGEN)" >>obj/Doxyfile
	-$(AT)doxygen obj/Doxyfile

html:
	-$(AT)mkdir html

html/options.html: html etc/options.xml etc/options.xsl
	-$(AT)echo "Making HTML options file" $(NULL)
	$(AT)xalan -in etc/options.xml -xsl etc/options.xsl -out html/options.html

$(ERRORS_MD): etc/errors.xml etc/templates/errors.md etc/errors.pl
	$(AT)etc/errors.pl -i $< -t etc/templates/errors.md -o $@

.PHONY: lobs
lobs: $(OPTIONS_H) $(LOBS)

.PHONY: lint
lint:   $(OPTIONS_H) $(LOBS)
	@echo Linting object files $(NULL)
	$(AT)cd obj && $(LINT) -e768 -e769 -summary *.lob

.PHONY: mostlyclean
mostlyclean:
	-$(AT)cd obj && rm -f *.o *.d *.lob $(NULL2)

