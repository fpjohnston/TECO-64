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

TARGET   = teco
VPATH    = src:obj:include
CC       = gcc
INCDIR   = include
INCLUDES = -I $(INCDIR)
LIBS     =                              # Default is no libraries

# Strip symbols from executable image. Will be undefined if debugging.

STRIP    = -s
MAKE     := $(MAKE) --no-print-directory

#  If verbose=1 is included when invoking make, we will print the actual
#  commands being executed, and will suppress the user-friendly messages
#  normally output.

ifdef   verbose

    AT =
    NULL = >/dev/null 2>&1

else

    AT = @

endif


include etc/build/sources.mk            # List of source files
include etc/build/variables.mk          # Variable definitions

OPTIONS = -c -std=gnu11 -Wall -Wextra -Wno-unused-parameter -fshort-enums
OPTIONS += $(INCLUDES) $(OPTIMIZE) $(DEBUG) $(DEFINES) -MMD

DFILES  = $(SOURCES:.c=.d)
LOBS    = $(SOURCES:.c=.lob)
OBJECTS = $(SOURCES:.c=.o)
OBJECTS := $(patsubst %,obj/%,$(OBJECTS))

#  Define default target ('teco').

.PHONY: $(TARGET) 
$(TARGET): bin/$(TARGET)

.PHONY: all
all: $(TARGET)                          # Equivalent to default target.

#  The following targets are present because git repositories only allow for
#  files in directories, not for directories that contain no files. So before
#  we start compiling and linking, we make sure that these two directories
#  exist.
#
#  Note that once created, these will remain in the directory tree, and will
#  not be deleted by any targets.

bin:
	$(AT)mkdir -p bin

obj:
	$(AT)mkdir -p obj

#  Always compile source files if compiler options have changed.

$(OBJECTS): obj/OPTIONS

obj/OPTIONS: obj                        # Create compiler options file
	-$(AT)@echo '$(OPTIONS)' | cmp -s - $@ || echo '$(OPTIONS)' > $@

obj/%.o: %.c                            # Compile source file
	@echo Making $@ $(NULL)
	$(AT)$(CC) @obj/OPTIONS $< -o obj/$(@F)

bin/$(TARGET): $(OBJECTS) bin           # Link object files, create executable
	@echo Making $(@F) $(NULL)
	$(AT)$(CC) $(DEBUG) $(STRIP) -o $@ $(OBJECTS) $(LIBS)

#  Copy executable image and library files to system directories.

.PHONY: install
install: $(TARGET)
	$(AT)install -v -C bin/teco /usr/local/bin
	$(AT)install -v -d /usr/local/lib/teco
	$(AT)install -v -C --mode=0644 lib/*.tec /usr/local/lib/teco

#  Update release version number.

ifeq ($(version),major)

else ifeq ($(version),minor)

else ifeq ($(version),patch)

else ifdef release

    $(error Invalid release version option: $(version))

endif

#  Define release target, used to update version number and create
#  a new public release of TECO.

.PHONY: release
release: distclean
	$(AT)etc/version.pl include/version.h etc/templates/version.h \
		--out include/version.h --version=$(version)
	$(AT)$(MAKE) -B -s include/version.h
	$(AT)$(MAKE) $(TARGET)

# Additional targets

-include $(DFILES)                      # Add in dependency targets

include etc/build/headers.mk            # Header file targets

include etc/build/help.mk               # Help message target

include etc/build/doc.mk                # Documentation targets

include etc/build/test.mk               # Test targets

include etc/build/clean.mk              # Clean targets
