#
#  Define debug target (for use with gdb).
#

.PHONY: debug
debug:
	@$(MAKE) gdb=1 debug=1 teco

#
#  Optimize for speed by inlining functions, removing all assertions, removing
#  strict checks on command syntax, and removing command tracing.
#

.PHONY: fast
fast:
	@$(MAKE) inline=1 ndebug=1 nstrict=1 ntrace=1 lto=1 teco

#
#  Define profile target (for use with gprof).
#

.PHONY: profile
profile:
	@$(MAKE) gprof=1 inline=1 ndebug=1 nstrict=1 ntrace=1 teco

#
#  Define targets that verify Perl scripts.
#

PERLCRITIC = $(shell which perlcritic)

ifneq ($(PERLCRITIC), )

    CRITIC := critic

.PHONY: critic
critic:
	@$(MAKE) obj/Teco.tmp
	@$(MAKE) obj/commands.tmp
	@$(MAKE) obj/errors.tmp
	@$(MAKE) obj/options.tmp
	@$(MAKE) obj/version.tmp
	@$(MAKE) obj/test_options.tmp
	@$(MAKE) obj/test_commands.tmp

obj/Teco.tmp: etc/Teco.pm
	@perlcritic $<
	@touch $@

obj/commands.tmp: etc/make_commands.pl
	@perlcritic $<
	@touch $@

obj/errors.tmp: etc/make_errors.pl
	@perlcritic $<
	@touch $@

obj/options.tmp: etc/make_options.pl
	@perlcritic $<
	@touch $@

obj/version.tmp: etc/make_version.pl
	@perlcritic $<
	@touch $@

obj/test_options.tmp: etc/test_options.pl
	@perlcritic $<
	@touch $@

obj/test_commands.tmp: etc/test_commands.pl
	@perlcritic $<
	@touch $@

else

.PHONY: critic
critic:
	@$(error Make target 'critic' requires perlcritic)

endif

#
#  Define target to include required test features
#

.PHONY: test
test:
	@$(MAKE) debug=2 teco

#
#  Define target to smoke test scripts, files, and executable image.
#

ifneq ($(PERL), )

.PHONY: smoke
smoke: distclean $(CRITIC) $(LINT) test
	@echo Testing TECO options...
	etc/test_options.pl --summary
	@echo Testing TECO commands...
	etc/test_commands.pl test/

else

.PHONY: smoke
smoke:
	@$(error Make target '$@' requires Perl)

endif
