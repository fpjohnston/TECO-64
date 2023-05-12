#  If 'flint' is in PATH, then use that, otherwise use the executable in the
#  directory pointed to by FLINT. If FLINT is not defined, then we shouldn't
#  get here.

LINT = lint

#  Each .lob file is created from a .c source file.

LOBS = $(patsubst src/%,obj/%,$(SOURCES:.c=.lob))

#  Each .lob file depends on the lint configuration files we use.

$(LOBS): obj/teco.lnt | obj

#  Create configuration file

obj/teco.lnt: etc/std.lnt | obj
	@echo -i $(FLINT)/lnt > $@
	@echo $(DEFINES) >> $@
	@echo -I $(INCLUDE) >> $@
	@echo -e786 -e818 -e830 -e843 -e844 >> $@
	@cat etc/std.lnt >> $@

#  Create lint object files from C source files.

obj/%.lob: src/%.c
	$(FLINT)/flint -b obj/teco.lnt -u -oo[$@] $<

#  Lint all C source files and then lint object files.

.PHONY: $(LINT)
$(LINT): obj/lobs.lnt

#  Create list of lint object files and lint all of them.

obj/lobs.lnt: $(LOBS)
	@echo $(subst $() $(),\\n,$(LOBS)) > obj/lobs.lnt
	$(FLINT)/flint -b obj/teco.lnt -summary -e768 -e769 obj/lobs.lnt

#  Create all lint object files

lobs: $(LOBS)
