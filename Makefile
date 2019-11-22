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
#      doc        doxygen + html
#      doxygen    Update Doxygen documentation.
#      help       Print help message.
#      html       Generate HTML options file.
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

DFLAGS =

ENABLE_DEBUG =

INCDIR = include

LINT_DEBUG =

OPT ?= 3
OPT_OPT = -O$(OPT)

INCLUDES = -I ../$(INCDIR)

VPATH=src:obj:$(INCDIR)

LIBS =

SOURCES = \
    a_cmd.c        \
    atsign_cmd.c   \
    bang_cmd.c     \
    cmd_buf.c      \
    cmd_exec.c     \
    cmd_scan.c     \
    colon_cmd.c    \
    comma_cmd.c    \
    ctrl_a_cmd.c   \
    ctrl_b_cmd.c   \
    ctrl_d_cmd.c   \
    ctrl_e_cmd.c   \
    ctrl_f_cmd.c   \
    ctrl_h_cmd.c   \
    ctrl_i_cmd.c   \
    ctrl_o_cmd.c   \
    ctrl_r_cmd.c   \
    ctrl_t_cmd.c   \
    ctrl_u_cmd.c   \
    ctrl_x_cmd.c   \
    digit_cmd.c    \
    e_cmd.c        \
    ea_cmd.c       \
    eb_cmd.c       \
    ec_cmd.c       \
    ed_cmd.c       \
    ee_cmd.c       \
    ef_cmd.c       \
    eg_cmd.c       \
    eh_cmd.c       \
    ei_cmd.c       \
    ej_cmd.c       \
    ek_cmd.c       \
    el_cmd.c       \
    em_cmd.c       \
    en_cmd.c       \
    env_sys.c      \
    eo_cmd.c       \
    ep_cmd.c       \
    e_pct_cmd.c    \
    eq_cmd.c       \
    equals_cmd.c   \
    er_cmd.c       \
    errors.c       \
    es_cmd.c       \
    et_cmd.c       \
    e_ubar_cmd.c   \
    eu_cmd.c       \
    ev_cmd.c       \
    ew_cmd.c       \
    ex_cmd.c       \
    ey_cmd.c       \
    ez_cmd.c       \
    f_cmd.c        \
    fc_cmd.c       \
    fd_cmd.c       \
    fk_cmd.c       \
    fn_cmd.c       \
    fr_cmd.c       \
    fs_cmd.c       \
    f_ubar_cmd.c   \
    g_cmd.c        \
    get_flag.c     \
    i_cmd.c        \
    lbracket_cmd.c \
    n_cmd.c        \
    o_cmd.c        \
    p_cmd.c        \
    percent_cmd.c  \
    push_expr.c    \
    q_cmd.c        \
    quote_cmd.c    \
    qreg.c         \
    rbracket_cmd.c \
    rparen_cmd.c   \
    s_cmd.c        \
    stubs.c        \
    teco.c         \
    term_in.c      \
    term_out.c     \
    term_sys.c     \
    u_cmd.c        \
    x_cmd.c        \
    y_cmd.c        \

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

LINT = flint -b -zero -i$(HOME)/flint/lnt $(LINT_DEBUG) std.lnt

%.lob: %.c
	@echo Making $@ $(NULL)
	$(AT)chdir src && $(LINT) -u $(INCLUDES) -oo\(../obj/$@\) ../$<

%.o: %.c
	@echo Making $@ $(NULL)
	$(AT)chdir obj && $(CC) @../CFLAGS ../$<

.PHONY: all
all: lint $(TARGET) doc

.PHONY: scratch
scratch: clobber all

.PHONY: help
help:
	@echo "Build targets:"
	@echo "    all        lint + teco + doc [default]"
	@echo "    clean      Clean object files."
	@echo "    clobber    Clean everything."
	@echo "    doc        doxygen + html"
	@echo "    doxygen    Update Doxygen documentation."
	@echo "    help       Print this message."
	@echo "    html       Generate HTML options file."
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
	$(AT)chdir obj && $(CC) -Xlinker -Map=../$@.map $(DFLAGS) -o ../$@ $(OBJECTS) $(LIBS) -lncurses

-include $(DFILES) 

$(OBJECTS): CFLAGS

.PHONY: FORCE
CFLAGS: FORCE
	-$(AT)echo '$(CFLAGS)' | cmp -s - $@ || echo '$(CFLAGS)' > $@

.PHONY: clean
clean:
	-$(AT)chdir bin && rm -f $(TARGET) $(TARGET).map $(NULL2)
	-$(AT)chdir obj && rm -f *.o $(NULL2)

.PHONY: clobber
clobber: clean
	-$(AT)rm -f CFLAGS $(NULL2) 
	-$(AT)chdir obj && rm -f *.d *.lob $(NULL2)
	-$(AT)chdir src && rm -f *.bak $(NULL2)
	-$(AT)chdir $(INCDIR) && rm -f *.bak _*.h $(NULL2)
	-$(AT)rm -rf html $(NULL2)

.PHONY: lobs
lobs: $(LOBS)

.PHONY: lint
lint:   $(LOBS)
	@echo Linting object files $(NULL)
	$(AT)chdir obj && $(LINT) -summary *.lob

.PHONY: doc
doc: doxygen

.PHONY: doxygen
doxygen:
	-$(AT)echo "Updating Doxygen documents" $(NULL)
	-$(AT)doxygen Doxyfile
