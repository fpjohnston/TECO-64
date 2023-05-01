#
#  Edit buffer variables.
#
################################################################################

ifeq (${buffer}, rope)

	$(error Rope buffer handler is not yet implemented)

else ifeq (${buffer}, gap)

    SOURCES += gap_buf.c

else ifeq (${buffer}, )

    SOURCES += gap_buf.c

else

	$(error Unknown buffer handler: ${buffer})

endif

#
#  Debugging variables.
#
################################################################################

ifdef   gdb
ifdef   gprof

    $(error gdb and gprof options cannot both be included for a build)

else

    DEBUG += -g
    OPTIMIZE = -O0
    STRIP =

endif
endif

#  Enable use of GPROF.

ifdef   gprof

    DEBUG += -pg
    OPTIMIZE = -O3
    STRIP =

endif

#  Enable basic memory checks. This is only intended as a quick and lightweight
#  version of tools such as valgrind, and not a replacement.

ifdef   memcheck

    DEFINES += -D MEMCHECK
    DOXYGEN += MEMCHECK
    STRIP =

endif

#  Enable debug mode.

ifdef   debug

    DEFINES += -D DEBUG
    DOXYGEN += DEBUG
    STRIP =

endif

#
#  Check display variables.
#
################################################################################

option = disable

ifeq ($(display),disable)
else ifeq ($(display),off)
else ifeq ($(display),no)
else ifeq ($(display),0)
else ifeq ($(display),enable)

    option = enable

else ifeq ($(display),on)

    option = enable

else ifeq ($(display),yes)

    option = enable

else ifeq ($(display),1)

    option = enable

else ifndef display

    option = enable

else

    $(error Invalid display mode option: $(display))

endif

ifeq ($(option),enable)

    DEFINES += -D DISPLAY_MODE
    LIBS    += -l ncurses
    SOURCES += display.c color_cmd.c key_cmd.c map_cmd.c status.c

else

    SOURCES += stubs.c

endif

option =

#
#  Check to see if TECO commands should use 32-bit or 64-bit integers.
#
################################################################################

ifeq      (${int}, 64)

    DEFINES += -D INT_T=64
    DOXYGEN +=    INT_T=64

else ifeq (${int}, 32)

    DEFINES += -D INT_T=32
    DOXYGEN +=    INT_T=32

else ifeq (${int}, )

    DEFINES += -D INT_T=32
    DOXYGEN +=    INT_T=32

else

    $(error Unknown integer size: ${int}: expected 32 or 64)

endif

#
#  Disable run-time assertions.
#
################################################################################

ifdef   ndebug

    DEFINES += -D NDEBUG
    DOXYGEN +=    NDEBUG

endif

#
#  Disable strict syntax checking.
#
################################################################################

ifdef   nostrict

    DEFINES += -D NOSTRICT

endif

#
#  Define default optimization level.
#
################################################################################

OPTIMIZE ?= -Ofast

#
#  Check to see which paging handler we should use
#
################################################################################

ifeq (${paging}, vm)

    SOURCES += page_vm.c
    DEFINES += -D PAGE_VM
    DOXYGEN +=    PAGE_VM

else ifeq (${paging}, file)

	#  This is a placeholder for eventual implementation of a method for using
	#  a "holding file" to allow backwards paging, as described in The Craft of
	#  Text Editing, by Craig A. Finseth.

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
