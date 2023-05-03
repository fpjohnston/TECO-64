#
#  Define targets that build documentation
#

DOXYGEN   = "DOXYGEN"

.PHONY: doc
doc: html/options.html doc/errors.md
	-$(AT)echo Making Doxygen documents...
	-$(AT)cp etc/Doxyfile obj/Doxyfile
	-$(AT)echo "PREDEFINED = $(DOXYGEN)" >>obj/Doxyfile
	-$(AT)doxygen obj/Doxyfile

html:
	-$(AT)mkdir -p html

html/options.html: html etc/xml/options.xml etc/xml/options.xsl
	-$(AT)echo Making HTML options file...
	$(AT)xalan -in etc/xml/options.xml -xsl etc/xml/options.xsl -out html/options.html

doc/errors.md: etc/xml/errors.xml etc/templates/errors.md
	$(AT)etc/make_errors.pl $^ -o $@

