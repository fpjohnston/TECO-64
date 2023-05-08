#
#  Define debug target (for use with gdb).
#

.PHONY: debug
debug:
	@echo Building TECO for debugging...
	@$(MAKE) gdb=1 debug=1 teco

#
#  Define fast target without assertions or strict checks.
#

.PHONY: fast
fast:
	@echo Building TECO for speed...
	@$(MAKE) ndebug=1 nostrict=1 notrace=1 teco

#
#  Define profile target (for use with gprof).
#

.PHONY: profile
profile:
	@echo Building TECO for profiling...
	@$(MAKE) gprof=1 nostrict=1 ndebug=1 notrace=1 teco

#
#  Define targets that verify Perl scripts.
#

.PHONY: critic
critic:
	@echo Checking Perl scripts...
	@$(MAKE) obj/Teco.tmp
	@$(MAKE) obj/commands.tmp
	@$(MAKE) obj/errors.tmp
	@$(MAKE) obj/options.tmp
	@$(MAKE) obj/version.tmp
	@$(MAKE) obj/test_options.tmp
	@$(MAKE) obj/test_commands.tmp

obj/Teco.tmp: etc/Teco.pm
	perlcritic $<
	@touch $@

obj/commands.tmp: etc/make_commands.pl
	perlcritic $<
	@touch $@

obj/errors.tmp: etc/make_errors.pl
	perlcritic $<
	@touch $@

obj/options.tmp: etc/make_options.pl
	perlcritic $<
	@touch $@

obj/version.tmp: etc/make_version.pl
	perlcritic $<
	@touch $@

obj/test_options.tmp: etc/test_options.pl
	perlcritic $<
	@touch $@

obj/test_commands.tmp: etc/test_commands.pl
	perlcritic $<
	@touch $@

#
#  Define target to include required test features
#

.PHONY: test
test:
	@echo Compiling and linking TECO...
	@$(MAKE) debug=1 memcheck=1 teco

#
#  Define target to smoke test scripts, files, and executable image.
#

PHONY: smoke
smoke: distclean critic lint test
	@echo Testing TECO options...
	etc/test_options.pl --summary
	@echo Testing TECO commands...
	etc/test_commands.pl test/
