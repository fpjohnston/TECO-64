
#
#  Define targets that clean things up
#

.PHONY: clean
clean: mostlyclean
	-$(AT)cd bin && rm -f $(TARGET) $(TARGET).map $(NULL)

.PHONY: distclean
distclean: obj bin mostlyclean clean
	-$(AT)rm -f obj/OPTIONS obj/Doxyfile $(NULL) 
	-$(AT)rm -rf html $(NULL) 
	-$(AT)cd src && rm -f *.bak $(NULL)
	-$(AT)cd test && rm -f cases/* results/* $(NULL)
	-$(AT)cd $(INCDIR) && rm -f *.bak $(NULL)

.PHONY: mostlyclean
mostlyclean: obj
	-$(AT)cd obj && rm -f *.o *.d *.lob *.tmp $(NULL)
