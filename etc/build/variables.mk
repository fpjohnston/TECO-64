################################################################################
#
#  Edit buffer options.
#

ifeq (${buffer}, )                  # Default is gap buffer

    SOURCES += gap_buf.c

else ifeq (${buffer}, gap)          # Did user ask for a gap buffer?

    SOURCES += gap_buf.c

else ifeq (${buffer}, rope)         # Did user ask for a rope buffer?

	$(error Rope buffer handler is not yet implemented)

else                                # We don't know what the user wants

	$(error Unknown buffer handler: ${buffer})

endif

################################################################################
#
#  Debugging and compiler optimization options.
#

ifdef   gdb                         # Can't build for both gdb and gprof
ifdef   gprof

    $(error gdb and gprof options cannot both be included for a build)

endif
endif

ifdef   gdb                         # Enable use of gdb.

    CC_OPTS += -O0 -g
    LINKOPTS = -g

else ifdef gprof                    # Enable use of gprof.

    CC_OPTS += -O3 -pg
    LINKOPTS += -pg
    DEFINES += -D PROFILE

else                                # Neither gdb nor gprof.

	CC_OPTS += -Ofast
    LINKOPTS += -s

endif

ifdef   debug                       # Enable basic internal debugging

    DEFINES += -D DEBUG
    DOXYGEN += DEBUG

endif

ifdef   memcheck                    # Enable basic internal memory checks

    DEFINES += -D MEMCHECK
    DOXYGEN += MEMCHECK

endif

################################################################################
#
#  Display mode options.
#

option = enable

ifndef display                      # Default is to enable display
else ifeq ($(display),enable)       # Did user ask for to enable display?
else ifeq ($(display),on)
else ifeq ($(display),yes)
else ifeq ($(display),1)
else

    option = disable

    ifeq ($(display),disable)       # Did user ask for to disable display?
    else ifeq ($(display),off)
    else ifeq ($(display),no)
    else ifeq ($(display),0)
    else                            # We don't know what the user wants

        $(error Invalid display mode option: $(display))

    endif
endif

ifeq ($(option),enable)

    LINKOPTS += -l ncurses
    SOURCES  += display.c color_cmd.c key_cmd.c map_cmd.c status.c

else

    SOURCES += stubs.c

endif

option =

################################################################################
#
#  Integer size options.
#

ifeq (${int}, )                     # Default is 32-bit integers

    DEFINES += -D INT_T=32
    DOXYGEN +=    INT_T=32

else ifeq (${int}, 32)              # Did user ask for 32-bit integers?

    DEFINES += -D INT_T=32
    DOXYGEN +=    INT_T=32

else ifeq (${int}, 64)              # Did user ask for 64-bit integers?

    DEFINES += -D INT_T=64
    DOXYGEN +=    INT_T=64

else                                # We don't know what the user wants

    $(error Unknown integer size: ${int}: expected 32 or 64)

endif

################################################################################
#
#  Speed options.
#

ifdef   ndebug                      # Disable run-time assertions

    DEFINES += -D NDEBUG
    DOXYGEN +=    NDEBUG

endif

ifdef   nostrict                    # Disable strict syntax checking

    DEFINES += -D NOSTRICT
    DOXYGEN +=    NOSTRICT

endif

ifdef   notrace                     # Disable command tracing

    DEFINES += -D NOTRACE
    DOXYGEN +=    NOTRACE

endif

################################################################################
#
#  Memory paging options.
#

ifeq (${paging}, )                  # Default is virtual memory paging

    SOURCES += page_vm.c
    DEFINES += -D PAGE_VM
    DOXYGEN +=    PAGE_VM

else ifeq (${paging}, vm)           # Did user ask for virtual memory paging?

    SOURCES += page_vm.c
    DEFINES += -D PAGE_VM
    DOXYGEN +=    PAGE_VM

else ifeq (${paging}, std)          # Did user ask for standard TECO paging?

    SOURCES += page_std.c

else ifeq (${paging}, file)         # Did user ask for holding file paging?

    #  This is a placeholder for eventual implementation of a method for using
    #  a "holding file" to allow backwards paging, as described in The Craft of
    #  Text Editing, by Craig A. Finseth.

    $(error Holding file paging is not yet implemented)

else                                # We don't know what the user wants

    $(error Unknown paging handler: ${paging})

endif
