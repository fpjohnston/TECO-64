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
    LINK =
endif

#  Enable use of GPROF.

ifdef   gprof
    DEBUG  += -pg
    OPT_OPT = -O3
    LINK =
endif

#  Enable basic memory checks.

ifdef   memcheck
    DEFINES += -D MEMCHECK
    DOXYGEN += MEMCHECK
endif

#
#  Enable test mode.
#

ifdef   test
    DEFINES       += -D TEST
    DOXYGEN       += TEST
    OPTIONS_DEBUG += -d
endif
