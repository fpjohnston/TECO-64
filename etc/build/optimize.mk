#
#  Disable run-time assertions.
#

ifdef   ndebug
    DEFINES += -D NDEBUG
    DOXYGEN +=    NDEBUG
endif

#
#  Disable strict syntax checking.
#

ifdef   nostrict
    DEFINES += -D NOSTRICT
endif

#
#  Define optimization level.
#

OPT ?= 3                                # Default is to use opt. level 3
