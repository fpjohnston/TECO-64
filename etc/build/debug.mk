# gdb and gprof options don't play well together

ifdef   gdb
ifdef   gprof
    $(error gdb and gprof options cannot both be included for a build)
endif
endif

#  Enable use of GDB.

ifdef   gdb
    DEBUG  += -g
    OPT_OPT = -O0
    STRIP   =                       # Don't strip symbol table
endif

#  Enable use of GPROF.

ifdef   gprof
    DEBUG  += -pg
    OPT_OPT = -O3
    STRIP   =                       # Don't strip symbol table
endif

#  Enable basic memory checks.

ifdef   memcheck
    DEFINES += -D MEMCHECK
    DOXYGEN +=    MEMCHECK
    STRIP    =                      # Don't strip symbol table
endif

#
#  Enable test mode.
#

ifdef   debug
    DEFINES += -D DEBUG
    DOXYGEN +=    DEBUG
    OPTIONS += -d
    STRIP    =                      # Don't strip symbol table
endif
