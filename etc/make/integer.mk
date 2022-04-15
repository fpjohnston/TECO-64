#
#  Determine whether to use 32-bit or 64-bit integers internally.
#

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
