#  Enable use of GDB.

ifdef   gdb
    DEBUG  += -g
    OPT_OPT = -O0
endif

#  Enable use of GPROF.

ifdef   gprof
ifdef   gdb
    $(error gdb and gprof options cannot both be included for a build)
endif
    DEBUG  += -pg
    OPT_OPT = -O3
endif

#  Enable basic memory checks.

ifdef   memcheck
    DEFINES       += -D MEMCHECK
    DOXYGEN       += MEMCHECK
    OPTIONS_DEBUG += -d
endif

#
#  Enable test mode.
#

ifdef   test
    DEFINES += -D TEST
    DOXYGEN +=    TEST
endif
