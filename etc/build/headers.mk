
#  Define header targets. This includes the scripts we run to regenerate header
#  files for TECO commands, error codes, and command-line options. If the user
#  does not have Perl, the supplied copies of these files may still be used, but
#  no changes will be possible.

ifneq ($(PERL), )

HEADERS = $(INCLUDE)commands.h $(INCLUDE)errcodes.h $(INCLUDE)errtables.h \
		$(INCLUDE)exec.h $(INCLUDE)options.h

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

else

HEADERS =

.PHONY: headers
headers:
	@$(error Make target '$@' requires Perl)

include/commands.h:
	@$(error Make target '$@' requires Perl)

include/errcodes.h:
	@$(error Make target '$@' requires Perl)

include/errtables.h:
	@$(error Make target '$@' requires Perl)

include/exec.h:
	@$(error Make target '$@' requires Perl)

include/options.h:
	@$(error Make target '$@' requires Perl)

endif
