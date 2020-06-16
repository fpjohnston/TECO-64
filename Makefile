################################################################################
#
#  Makefile for TECO - Text editor
#
#  Copyright (C) tbd
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
#  make targets:
#
#      all        lint + teco + doc [default]
#      clean      Clean object files.
#      clobber    Clean everything.
#      doc        doxygen
#      doxygen    Update Doxygen documentation.
#      help       Print help message.
#      lint       Lint source files.
#      lobs       Build lint object files.
#      options    Print list of makefile options.
#      scratch    clobber + all
#      teco       Build TECO text editor
#
#  Configuration options:
#
#      DEBUG=1    Enable debugging features.
#      GDB=1      Enable use of GDB debugger.
#      GPROF=1    Enable use of GPROF profiler.
#      NDEBUG=1   Disable run-time assertions.
#      VERBOSE=1  Enable verbosity during build.
#
################################################################################

GDB = 1                                 # TODO: only for development
TARGET = teco

CC = gcc

CFLAGS = -std=gnu18 -Wall -Wextra -Wno-unused-parameter -fshort-enums
CFLAGS += -D SCOPE

DFLAGS =

ENABLE_DEBUG =

INCDIR = include

LINT_DEBUG = -D SCOPE

OPT ?= 3
OPT_OPT = -O$(OPT)

INCLUDES = -I ../$(INCDIR)

VPATH=src:obj:$(INCDIR)

LIBS =

SOURCES = \
    build_str.c    \
    cmd_buf.c      \
    cmd_exec.c     \
    cmd_scan.c     \
    cmd_tables.c   \
    env_sys.c      \
    errors.c       \
    estack.c       \
    file.c         \
    file_sys.c     \
    memory.c       \
    option_sys.c   \
    print_cmd.c    \
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
    back_cmd.c     \
    brace_cmd.c    \
    bracket_cmd.c  \
    case_cmd.c     \
    comma_cmd.c    \
    ctrl_a_cmd.c   \
    ctrl_c_cmd.c   \
    ctrl_t_cmd.c   \
    ctrl_u_cmd.c   \
    ctrl_v_cmd.c   \
    ctrl_w_cmd.c   \
    datetime_cmd.c \
    delete_cmd.c   \
    digit_cmd.c    \
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
    ep_cmd.c       \
    eq_cmd.c       \
    equals_cmd.c   \
    er_cmd.c       \
    e_ubar_cmd.c   \
    ew_cmd.c       \
    ex_cmd.c       \
    ez_cmd.c       \
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
    p_cmd.c        \
    pct_cmd.c      \
    q_cmd.c        \
    quest_cmd.c    \
    radix_cmd.c    \
    s_cmd.c        \
    type_cmd.c     \
    ubar_cmd.c     \
    u_cmd.c        \
    value_cmd.c    \
    w_cmd.c        \
    x_cmd.c        \
    yank_cmd.c     \

#
#  Check to see which buffer handler we should use
#
################################################################################

ifeq (${TECO_BUFFER}, rope)

$(error Rope buffer handler is not yet implemented)

SOURCES += ropebuf.c

else ifeq (${TECO_BUFFER}, gap)

SOURCES += gapbuf.c

else ifeq (${TECO_BUFFER}, )

SOURCES += gapbuf.c

else

$(error Unknown buffer handler: ${TECO_BUFFER})

endif

#
#  Check to see which paging handler we should use
#
################################################################################

ifeq (${TECO_PAGING}, vm)

SOURCES += page_vm.c

$(warning Virtual memory paging is in development.)

else ifeq (${TECO_PAGING}, file)

$(error Holding file paging is not yet implemented)

SOURCES += page_file.c

else ifeq (${TECO_PAGING}, standard)

SOURCES += page_std.c

else ifeq (${TECO_PAGING}, )

SOURCES += page_std.c

else

$(error Unknown paging handler: ${TECO_PAGING})

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

CFLAGS += -D DEBUG
ENABLE_DEBUG += -d
LINT_DEBUG += -DDEBUG

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

LOBS = $(SOURCES:.c=.lob)

OBJECTS = $(SOURCES:.c=.o)

DFILES = $(SOURCES:.c=.d)

CFLAGS += -MMD -c $(INCLUDES) $(OPT_OPT) $(DFLAGS)

LINT = flint -b -zero -i$(HOME)/flint/lnt $(LINT_DEBUG) std.lnt -e126 -e786 -e818 -e830 +fan +fas

.PHONY: default 
default: $(TARGET)

.PHONY: all
all: lobs $(TARGET) doc

.PHONY: scratch
scratch: clobber all

.PHONY: help
help:
	@echo "Build targets:"
	@echo "    all        lint + teco + doc [default]"
	@echo "    clean      Clean object files."
	@echo "    clobber    Clean everything."
	@echo "    doc        doxygen"
	@echo "    doxygen    Update Doxygen documentation."
	@echo "    help       Print this message."
	@echo "    lint       Lint source files."
	@echo "    lobs       Build lint object files."
	@echo "    scratch    clobber + all"
	@echo "    teco       Build TECO text editor."

.PHONY: options
options:
	@echo "Build options:"
	@echo "    DEBUG=1    Enable debugging features."
	@echo "    GDB=1      Enable use of GDB debugger."
	@echo "    GPROF=1    Enable use of GPROF profiler."
	@echo "    NDEBUG=1   Disable run-time assertions."
	@echo "    VERBOSE=1  Enable verbosity during build."


.PHONY: $(TARGET) 
$(TARGET): bin/$(TARGET)

bin/$(TARGET): $(OBJECTS)
	@echo Making $(@F) $(NULL)
	$(AT)cd obj && $(CC) $(DFLAGS) -o ../$@ $(OBJECTS) $(LIBS) -lncurses

%.lob: %.c
	@echo Making $@ $(NULL)
	$(AT)cd src && $(LINT) -u $(INCLUDES) -oo\(../obj/$@\) ../$<

#%.d: %.c CFLAGS
#	@echo Making $@ $(NULL)
#	$(AT)cd obj && $(CC) @../CFLAGS ../$<

%.o: %.c
	@echo Making $@ $(NULL)
	$(AT)cd obj && $(CC) @../CFLAGS ../$<

-include $(DFILES)

$(OBJECTS): CFLAGS

.PHONY: FORCE
CFLAGS: FORCE
	-$(AT)echo '$(CFLAGS)' | cmp -s - $@ || echo '$(CFLAGS)' > $@

.PHONY: clean
clean:
	-$(AT)cd bin && rm -f $(TARGET) $(TARGET).map $(NULL2)
	-$(AT)cd obj && rm -f *.o $(NULL2)

.PHONY: clobber
clobber: clean
	-$(AT)rm -f CFLAGS $(NULL2) 
	-$(AT)cd obj && rm -f *.d *.lob $(NULL2)
	-$(AT)cd src && rm -f *.bak $(NULL2)
	-$(AT)cd $(INCDIR) && rm -f *.bak _*.h $(NULL2)

.PHONY: lobs
lobs: $(LOBS)

.PHONY: lint
lint:   $(LOBS)
	@echo Linting object files $(NULL)
	$(AT)cd obj && $(LINT) -e768 -e769 -summary *.lob

.PHONY: doc
doc: doxygen

.PHONY: doxygen
doxygen:
	-$(AT)echo "Updating Doxygen documents" $(NULL)
	-$(AT)doxygen Doxyfile
