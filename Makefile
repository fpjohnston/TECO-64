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
#  See help target below for a list of build targets and options.
#
################################################################################

TECO    = teco
SHELL   = /bin/sh
CC      = gcc
INCLUDE = include/
VPATH   = %h $(INCLUDE)
VPATH   = %c src
VPATH   = %o obj lob
LINT    =

MAKE := $(MAKE) --no-print-directory

EXCLUDES =

# Get the list of all possible source files.

SOURCES = $(wildcard src/*.c)

CFLAGS = -c -std=gnu11 -Wall -Wextra -Wno-unused-parameter -fshort-enums -MMD

include etc/build/init.mk           # Initialize variables we'll need

CFLAGS += -I $(INCLUDE) $(DEFINES)

# Filter out source files not needed for this build.

EXCLUDES := $(patsubst %,src/%,$(EXCLUDES))

SOURCES := $(filter-out $(EXCLUDES),$(SOURCES))

OBJECTS = $(patsubst src/%.c,obj/%.o,$(SOURCES))

#  Define default target.

.PHONY: teco
teco: bin/teco

bin/teco: $(OBJECTS) bin                # Link object files, create executable
	@echo $(OBJECTS) > obj/objects.lis
	$(CC) -o $@ @obj/objects.lis $(LINKOPTS)

.PHONY: all
all: $(TECO)

$(eval PERL=$(shell which perl))

ifneq ($(PERL), )

PERL := perl

endif

ifneq ($(PERL), )

    ANCILLARY = $(INCLUDE)_cmd_exec.c $(INCLUDE)errcodes.h $(INCLUDE)_errors.c \
			$(INCLUDE)exec.h $(INCLUDE)_option_sys.c

else

	ANCILLARY =

endif

$(OBJECTS): $(ANCILLARY) obj/cflags

#  The following targets are present because git repositories only allow for
#  files in directories, not for directories that contain no files. So before
#  we start compiling and linking, we make sure that these two directories
#  exist.

bin:
	@mkdir -p bin

obj:
	@mkdir -p obj

#  Always compile source files if compiler options have changed.

obj/cflags: obj                             # Create compiler options file
	-@echo '$(CFLAGS)' | cmp -s - $@ || echo '$(CFLAGS)' > $@

obj/%.o: src/%.c                                # Compile source file
	gcc -o $@ $< @obj/cflags

#  Copy executable image and library files to system directories.

.PHONY: install
install: $(TECO)
	install -v -C bin/teco /usr/local/bin
	install -v -d /usr/local/lib/teco
	install -v -C --mode=0644 lib/*.tec /usr/local/lib/teco

#  Create new public release of TECO with updated version number.

.PHONY: release
release: distclean
	etc/make_version.pl include/version.h etc/templates/version.h \
		--out include/version.h --version=$(version)
	$(MAKE) -B -s include/version.h
	$(MAKE) $(TECO)

#  Clean binary files, object files, and temporary files.

.PHONY: clean
clean:
	-rm -f bin/$(TECO) bin/$(TECO).map obj/*

#  Clean everything.

.PHONY: distclean
distclean: obj bin clean
	-rm -f include*.bak src/*.bak
	-rm -f test/cases/* test/results/*
	-rm -rf html 

#  Print help message

.PHONY: help
help:
	@echo "Build targets:"
	@echo ""
	@echo "    teco         Build TECO-64 text editor. [default]"
	@echo "    all          Equivalent to 'teco' target."
	@echo "    clean        Clean object files and executables."
	@echo "    distclean    Clean everything."
	@echo "    doc          Create documentation w/ Doxygen."
	@echo "    help         Print this message."
	@echo "    install      Build executable and copy to /usr/local/bin."
	@echo ""
	@echo "Build options:"
	@echo ""
	@echo "    buffer=gap   Use gap buffer for editing text in target. [default]"
	@echo "    display=on   Enable display mode in target. [default]"
	@echo "    display=off  Enable display mode in target."
	@echo "    int=32       Use 32-bit integers in target. [default]."
	@echo "    int=64       Use 64-bit integers in target."
	@echo "    paging=std   Use standard paging in target."
	@echo "    paging=vm    Use virtual memory paging in target. [default]"
	@echo ""
	@echo "Development targets:"
	@echo ""
	@echo "    critic       Do static code analysis of Perl scripts."
	@echo "    debug        Build TECO for debugging w/ gdb."
	@echo "    fast         Build TECO w/ minimal run-time checks."
	@echo "    lint         Lint C source files."
	@echo "    profile      Build TECO for profiling w/ gprof."
	@echo "    smoke        Run all smoke tests."
	@echo "    test         Build TECO for testing."
	@echo ""

# Additional targets

DFILES = $(SOURCES:.c=.d)

-include $(DFILES)                      # Dependency targets

include etc/build/doc.mk                # Documentation targets

ifdef   FLINT

include etc/build/lint.mk               # Lint targets

endif

include etc/build/test.mk               # Test targets


#  Define targets that build ancillary files from XML files, which contain such
#  information as TECO commands, error codes, and command-line options. If the
#  user does not have Perl, the supplied copies of these files may still be used,
#  but the targets will below either not be called within the Makefile, or will
#  generate errors if explicitly specified by the user.

ifneq ($(PERL), )

.PHONY: ancillary
ancillary: $(AUX)

$(INCLUDE)_cmd_exec.c: etc/make_commands.pl etc/xml/commands.xml etc/templates/_cmd_exec.c
	$^ --out $@

$(INCLUDE)/errcodes.h: etc/make_errors.pl etc/xml/errors.xml etc/templates/errcodes.h
	$^ --out $@

$(INCLUDE)_errors.c: etc/make_errors.pl etc/xml/errors.xml etc/templates/_errors.c
	$^ --out $@

$(INCLUDE)/exec.h: etc/make_commands.pl etc/xml/commands.xml etc/templates/exec.h
	$^ --out $@

$(INCLUDE)_option_sys.c: etc/make_options.pl etc/xml/options.xml etc/templates/_option_sys.c
	$^ --out $@

else

.PHONY: ancillary
ancillary:
	@$(error Make target '$@' requires Perl)

$(INCLUDE)_commands.c:
	@$(error Make target '$@' requires Perl)

$(INCLUDE)/errcodes.h:
	@$(error Make target '$@' requires Perl)

$(INCLUDE)_errtables.c:
	@$(error Make target '$@' requires Perl)

$(INCLUDE)/exec.h:
	@$(error Make target '$@' requires Perl)

$(INCLUDE)_option_sys.c:
	@$(error Make target '$@' requires Perl)

endif
