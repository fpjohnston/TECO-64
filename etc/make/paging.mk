#
#  Check to see which paging handler we should use
#
################################################################################

ifeq (${paging}, vm)
    SOURCES += page_vm.c
    DEFINES += -D PAGE_VM
    DOXYGEN +=    PAGE_VM
else ifeq (${paging}, file)
    $(error Holding file paging is not yet implemented)
else ifeq (${paging}, std)
    SOURCES += page_std.c
else ifeq (${paging}, )
    SOURCES += page_vm.c
    DEFINES += -D PAGE_VM
    DOXYGEN +=    PAGE_VM
else
    $(error Unknown paging handler: ${paging})
endif
