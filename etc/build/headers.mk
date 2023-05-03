#
#  Define targets that create header files from XML files and template files.
#  This is done to avoid duplicating information in multiple header or source
#  files, regarding such things as TECO commands, error codes, and command-line
#  options.
#

.PHONY: headers
headers: include/commands.h \
		 include/errcodes.h \
		 include/errtables.h \
		 include/exec.h \
		 include/options.h

include/commands.h: etc/make_commands.pl etc/xml/commands.xml etc/templates/commands.h
	$(AT)$^ --out $@

include/errcodes.h: etc/make_errors.pl etc/xml/errors.xml etc/templates/errcodes.h
	$(AT)$^ --out $@

include/errtables.h: etc/make_errors.pl etc/xml/errors.xml etc/templates/errtables.h
	$(AT)$^ --out $@

include/exec.h: etc/make_commands.pl etc/xml/commands.xml etc/templates/exec.h
	$(AT)$^ --out $@

include/options.h: etc/make_options.pl etc/xml/options.xml etc/templates/options.h
	$(AT)$^ --out $@
