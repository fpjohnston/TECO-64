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

include/commands.h: etc/commands.pl etc/commands.xml etc/templates/commands.h
	$(AT)$^ --out $@

include/errcodes.h: etc/errors.pl etc/errors.xml etc/templates/errcodes.h
	$(AT)$^ --out $@

include/errtables.h: etc/errors.pl etc/errors.xml etc/templates/errtables.h
	$(AT)$^ --out $@

include/exec.h: etc/commands.pl etc/commands.xml etc/templates/exec.h
	$(AT)$^ --out $@

include/options.h: etc/options.pl etc/options.xml etc/templates/options.h
	$(AT)$^ --out $@
