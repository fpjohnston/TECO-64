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

html/options.html: html etc/options.xml etc/options.xsl
	-$(AT)echo Making HTML options file...
	$(AT)xalan -in etc/options.xml -xsl etc/options.xsl -out html/options.html

doc/errors.md: etc/errors.xml etc/templates/errors.md
	$(AT)etc/errors.pl $^ -o $@

