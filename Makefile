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
#  See etc/build/help.mk for a list of build targets and options.
#
################################################################################

TARGET  = teco
SHELL   = /bin/sh
VPATH   = src:obj:include
CC      = gcc
INCLUDE = include/

MAKE     := $(MAKE) --no-print-directory

.SUFFIXES:
.SUFFIXES: .c .o .lob

CC_OPTS = -c -std=gnu11 -Wall -Wextra -Wno-unused-parameter -fshort-enums -MMD
CC_OPTS += -I $(INCLUDE)

HEADERS = $(INCLUDE)commands.h $(INCLUDE)errcodes.h $(INCLUDE)errtables.h \
		$(INCLUDE)exec.h $(INCLUDE)options.h

include etc/build/sources.mk            # List of source files
include etc/build/variables.mk          # Variable definitions

CC_OPTS += $(DEFINES)
DFILES  = $(SOURCES:.c=.d)
O_FILES = $(SOURCES:.c=.o)
O_FILES := $(patsubst %,obj/%,$(O_FILES))

#  Define default target ('teco').

.PHONY: $(TARGET) 
$(TARGET): bin/$(TARGET)

.PHONY: all
all: $(TARGET)                          # Equivalent to default target.

#
#  Define targets that create header files from XML files and template files.
#  This is done to avoid duplicating information in multiple header or source
#  files, regarding such things as TECO commands, error codes, and command-line
#  options.
#

.PHONY: headers
headers: $(HEADERS)

include/commands.h: etc/make_commands.pl etc/xml/commands.xml etc/templates/commands.h
	$^ --out $@

include/errcodes.h: etc/make_errors.pl etc/xml/errors.xml etc/templates/errcodes.h
	$^ --out $@

include/errtables.h: etc/make_errors.pl etc/xml/errors.xml etc/templates/errtables.h
	$^ --out $@

include/exec.h: etc/make_commands.pl etc/xml/commands.xml etc/templates/exec.h
	$^ --out $@

include/options.h: etc/make_options.pl etc/xml/options.xml etc/templates/options.h
	$^ --out $@

#  The following targets are present because git repositories only allow for
#  files in directories, not for directories that contain no files. So before
#  we start compiling and linking, we make sure that these two directories
#  exist.
#
#  Note that once created, these will remain in the directory tree, and will
#  not be deleted by any targets.

bin:
	@mkdir -p bin

obj:
	@mkdir -p obj

#  Always compile source files if compiler options have changed.

$(O_FILES): $(HEADERS) obj/cc_opts

obj/cc_opts: obj                       # Create compiler options file
	-@echo '$(CC_OPTS)' | cmp -s - $@ || echo '$(CC_OPTS)' > $@

obj/%.o: %.c                            # Compile source file
	$(CC) -o obj/$(@F) $< @obj/cc_opts

bin/$(TARGET): $(O_FILES) bin           # Link object files, create executable
	@echo $(O_FILES) > obj/o_files.lis
	$(CC) -o $@ @obj/o_files.lis $(LINKOPTS)

#  Copy executable image and library files to system directories.

.PHONY: install
install: $(TARGET)
	install -v -C bin/teco /usr/local/bin
	install -v -d /usr/local/lib/teco
	install -v -C --mode=0644 lib/*.tec /usr/local/lib/teco

#  Create new public release of TECO with updated version number.

.PHONY: release
release: distclean
	etc/make_version.pl include/version.h etc/templates/version.h \
		--out include/version.h --version=$(version)
	$(MAKE) -B -s include/version.h
	$(MAKE) $(TARGET)

#  Clean binary files, object files, and temporary files.

.PHONY: clean
clean:
	-rm -f bin/$(TARGET) bin/$(TARGET).map obj/*

#  Clean everything.

.PHONY: distclean
distclean: obj bin clean
	-rm -f include*.bak src/*.bak
	-rm -f test/cases/* test/results/*
	-rm -rf html 

# Create HTML documentation with Doxygen

.PHONY: doc
doc: html/options.html doc/errors.md | html
	-@cp etc/Doxyfile html/Doxyfile
	-@echo "PREDEFINED = DOXYGEN" >>html/Doxyfile
	-doxygen html/Doxyfile

html:
	-@mkdir -p html

# Create HTML documentation for TECO command-line options.

html/options.html: etc/xml/options.xml etc/xml/options.xsl | html
	xalan -in etc/xml/options.xml -xsl etc/xml/options.xsl -out html/options.html

# Create Markdown documentation for TECO run-time errors.

doc/errors.md: etc/make_errors.pl etc/xml/errors.xml etc/templates/errors.md
	$^ -o $@

# Additional targets

-include $(DFILES)                      # Add in dependency targets

include etc/build/help.mk               # Help message target

include etc/build/test.mk               # Test targets

ifdef   FLINT

include etc/build/lint.mk

endif
