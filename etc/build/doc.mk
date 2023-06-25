# Create HTML documentation with Doxygen

ifneq ($(PERL), )

ERRORS_MD = doc/errors.md

# Create Markdown documentation for TECO run-time errors (if we have Perl).

doc/errors.md: etc/make_errors.pl etc/xml/errors.xml etc/pattern/errors.md
	$^ -o $@

else

ERRORS_MD =

doc/errors.md:
	@$(error Make target '$@' requires Perl)

endif

XALAN = $(shell which xalan)

ifneq ($(XALAN), )

OPTIONS_HTML = html/options.html

# Create HTML documentation for TECO command-line options (if we have xalan).

html/options.html: etc/xml/options.xml etc/xml/options.xsl
	@mkdir -p $(@D)
	xalan -in etc/xml/options.xml -xsl etc/xml/options.xsl -out html/options.html

else

html/options.html:
	@mkdir -p $(@D)
	@$(error Make target '$@' requires xalan)

endif

#  Create doc target (if we have Doxygen).

DOXYGEN = $(shell which doxygen)

ifneq ($(DOXYGEN), )

.PHONY: doc
doc: $(OPTIONS_HTML) $(ERRORS_MD)
	@mkdir -p $(@D)
	-@cp etc/Doxyfile html/Doxyfile
	-@echo "PREDEFINED = DOXYGEN" >>html/Doxyfile
	-doxygen html/Doxyfile

else

.PHONY: doc
doc:
	@$(error Make target '$@' requires Doxygen)

endif
