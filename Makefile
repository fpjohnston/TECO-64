################################################################################
#
#  Makefile for TECO-64 text editor
#
#  Copyright (C) 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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
#      all          Equivalent to 'teco' target.
#      clean        Clean object files and executables.
#      distclean    Clean everything.
#      doc          Create or update documentation.
#      help         Print help message.
#      install      Build executable and copy to /usr/local/bin.
#      mostlyclean  Clean object files.
#      teco         Build TECO-64 text editor. [default]
#
#  Build options:
#
#      buffer=gap   Use gap buffer for editing text. [default]
#      display=on   Enable display mode. [default]
#      display=off  Disable display mode.
#      headers      Regenerate header files if needed.
#      int=32       Use 32-bit integers. [default]
#      int=64       Use 64-bit integers.
#      paging=std   Use standard paging.
#      paging=vm    Use virtual memory paging. [default]
#
#  Debugging options:
#
#      lint         Lint .c and .lob files (requires PC-lint).
#      lobs         Lint .c files (requires PC-lint).
#      gdb=1        Enable use of GDB debugger.
#      gprof=1      Enable use of GPROF profiler.
#      memcheck=1   Enable checks for memory leaks.
#      verbose=1    Enable verbosity during build.
#
#  Optimization options:
#
#      ndebug=1     Disable run-time assertions.
#      nostrict=1   Disable run-time syntax checking.
#
################################################################################

TARGET   = teco
VPATH    = src:obj
CC       = gcc
INCDIR   = include
INCLUDES = -I ../$(INCDIR)
LIBS     =                              # Default is no libraries
STRIP    = -s                           # Strip symbol table when linking

export PATH := etc/:${PATH}

#  Enable verbosity if requested.

ifdef   verbose
    AT   =
    NULL = 
else
    AT   = @
    NULL = >/dev/null 2>&1
endif

include etc/make/sources.mk             # Source files
include etc/make/display.mk             # Display node option
include etc/make/buffer.mk              # Buffer handler option
include etc/make/paging.mk              # Page handler option
include etc/make/integer.mk             # Integer option
include etc/make/debug.mk               # Debugging options
include etc/make/optimize.mk            # Optimization options
include etc/make/release.mk             # Release option

DFILES  = $(SOURCES:.c=.d)
LOBS    = $(SOURCES:.c=.lob)
OBJECTS = $(SOURCES:.c=.o)

CFLAGS = -c -std=gnu11 -Wall -Wextra -Wno-unused-parameter -fshort-enums \
         -O$(OPT) $(INCLUDES) $(OPT_OPT) $(DEBUG) $(DEFINES) -MMD

#  Define default target ('teco'). This may also update the release version.

.PHONY: $(TARGET) 
$(TARGET): $(VERSION) bin/$(TARGET)

.PHONY: all
all: $(TARGET)                          # Equivalent to default target.

.PHONY: install
install: $(TARGET)
	$(AT)install -v -C bin/teco /usr/local/bin
	$(AT)install -v -d /usr/local/lib/teco
	$(AT)install -v -C --mode=0644 lib/*.tec /usr/local/lib/teco

.PHONY: version
version: include/version.h

#
#  Define targets that create directories.
#

bin:
	$(AT)mkdir -p bin

obj:
	$(AT)mkdir -p obj

#
#  Define targets that create header files from XML files and template files.
#  This is done to avoid duplicating information pertaining to such things as
#  commands, error codes, or start-up options.
#

HEADERS = include/commands.h include/errcodes.h include/errtables.h \
		  include/exec.h include/options.h

.PHONY: headers
headers: $(HEADERS)

include/commands.h: etc/commands.xml etc/templates/commands.h
	$(AT)commands.pl $^ --out $@

include/errcodes.h: etc/errors.xml etc/templates/errcodes.h
	$(AT)errors.pl $^ --out $@

include/errtables.h: etc/errors.xml etc/templates/errtables.h
	$(AT)errors.pl $^ --out $@

include/exec.h: etc/commands.xml etc/templates/exec.h
	$(AT)commands.pl $^ --out $@

include/options.h: etc/options.xml etc/templates/options.h
	$(AT)options.pl etc/options.xml etc/templates/options.h --out $@ $(OPTIONS)

include/version.h: distclean
	$(AT)version.pl include/version.h etc/templates/$(@F) --out $@ --release=$(release)

#
#  Define how to compile source files.
#

$(OBJECTS): obj/CFLAGS

obj/CFLAGS: obj
	-$(AT)echo '$(CFLAGS)' | cmp -s - $@ || echo '$(CFLAGS)' > $@

-include $(DFILES)

%.o: %.c $(HEADERS)
	@echo Making $@
	$(AT)cd obj && $(CC) @CFLAGS ../$<

#
#  Define how to link object files and create executable image.
#

bin/$(TARGET): $(OBJECTS) bin
	@echo Making $(@F)
	$(AT)cd obj && $(CC) $(DEBUG) $(STRIP) -o ../$@ $(OBJECTS) $(LIBS)

#
#  Define help target.
#

include etc/make/help.mk                # Help target

#
#  Define targets that build documentation
#

DOXYGEN   = "DOXYGEN"

.PHONY: doc
doc: html/options.html doc/errors.md
	-$(AT)echo "Making Doxygen documents"
	-$(AT)cp etc/Doxyfile obj/Doxyfile
	-$(AT)echo "PREDEFINED = $(DOXYGEN)" >>obj/Doxyfile
	-$(AT)doxygen obj/Doxyfile

html:
	-$(AT)mkdir -p html

html/options.html: html etc/options.xml etc/options.xsl
	-$(AT)echo "Making HTML options file"
	$(AT)xalan -in etc/options.xml -xsl etc/options.xsl -out html/options.html

doc/errors.md: etc/errors.xml etc/templates/errors.md
	$(AT)etc/errors.pl $^ -o $@

#
#  Define targets that clean things up
#

.PHONY: clean
clean: mostlyclean
	-$(AT)cd bin && rm -f $(TARGET) $(TARGET).map $(NULL)

.PHONY: distclean
distclean: obj bin mostlyclean clean
	-$(AT)rm -f obj/CFLAGS obj/Doxyfile $(NULL) 
	-$(AT)rm -rf html $(NULL) 
	-$(AT)cd src && rm -f *.bak $(NULL)
	-$(AT)cd test && rm -f cases/* results/* $(NULL)
	-$(AT)cd $(INCDIR) && rm -f *.bak $(NULL)

.PHONY: mostlyclean
mostlyclean: obj
	-$(AT)cd obj && rm -f *.o *.d *.lob $(NULL)

#
#  Define targets that verify Perl scripts.
#

.PHONY: critic
critic:
	$(AT)perlcritic etc/Teco.pm
	$(AT)perlcritic etc/commands.pl
	$(AT)perlcritic etc/errors.pl
	$(AT)perlcritic etc/options.pl
	$(AT)perlcritic etc/version.pl

#
#  Define how to lint source files.
#

LINT = flint -b -zero -i$(HOME)/flint/lnt $(DEFINES) ../etc/std.lnt \
             -e126 -e786 -e818 -e830 -e843 -e844 +fan +fas

%.lob: %.c obj
	@echo Making $@
	$(AT)cd src && $(LINT) -u $(INCLUDES) -oo\(../obj/$@\) ../$<

#
#  Define how to lint .lob (lint object) files.
#

.PHONY: lint
lint: obj $(LOBS)
	@echo Linting object files
	$(AT)cd obj && $(LINT) -e768 -e769 -summary *.lob

.PHONY: lobs
lobs: obj $(LOBS)

#
#  Define target to smoke test executable image.
#

PHONY: smoke
smoke:
	@echo Rebuilding teco for smoke testing
	cd . && $(MAKE) -B debug=1 memcheck=1 include/options.h
	cd . && $(MAKE) debug=1 memcheck=1 teco
	@echo Checking $(TARGET) command-line options
	$(AT)test/option_test.pl --summary
	@echo Smoke testing $(TARGET)
	$(AT)test/smoke_test.pl test/
