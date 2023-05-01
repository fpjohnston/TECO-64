#
#  Define debug target (for use with gdb).
#

.PHONY: debug
debug:
	@echo Building TECO for debugging...
	@$(MAKE) gdb=1 debug=1 teco

#
#  Define profile target (for use with gprof).
#

.PHONY: profile
profile:
	@echo Building TECO for profiling...
	@$(MAKE) gprof=1 nostrict=1 ndebug=1 teco

#
#  Define fast target without assertions or strict checks.
#

.PHONY: fast
fast:
	@echo Building TECO for speed...
	@$(MAKE) nostrict=1 ndebug=1 teco

#
#  Define targets that verify Perl scripts.
#

.PHONY: critic
critic:
	@echo Checking Perl scripts...
	@$(MAKE) -s obj/Teco.tmp
	@$(MAKE) -s obj/commands.tmp
	@$(MAKE) -s obj/errors.tmp
	@$(MAKE) -s obj/options.tmp
	@$(MAKE) -s obj/version.tmp
	@$(MAKE) -s obj/option_test.tmp
	@$(MAKE) -s obj/smoke_test.tmp

obj/Teco.tmp: etc/Teco.pm
	$(AT)perlcritic $<
	$(AT)touch $@

obj/commands.tmp: etc/commands.pl
	$(AT)perlcritic $<
	$(AT)touch $@

obj/errors.tmp: etc/errors.pl
	$(AT)perlcritic $<
	$(AT)touch $@

obj/options.tmp: etc/options.pl
	$(AT)perlcritic $<
	$(AT)touch $@

obj/version.tmp: etc/version.pl
	$(AT)perlcritic $<
	$(AT)touch $@

obj/option_test.tmp: test/option_test.pl
	$(AT)perlcritic $<
	$(AT)touch $@

obj/smoke_test.tmp: test/smoke_test.pl
	$(AT)perlcritic $<
	$(AT)touch $@

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
lint: obj
	@echo Linting source files...
	@$(MAKE) -s $(LOBS)
	$(AT)cd obj && $(LINT) -e768 -e769 -summary *.lob

.PHONY: lobs
lobs: obj $(LOBS)

#
#  Define target to include required test features
#

.PHONY: test
test:
	@echo Compiling and linking TECO...
	@$(MAKE) debug=1 memcheck=1 teco

#
#  Define target to smoke test executable image.
#

PHONY: smoke
smoke: critic lint test
	@echo Testing TECO options...
	$(AT)test/option_test.pl --summary
	@echo Testing TECO commands...
	$(AT)test/smoke_test.pl test/
