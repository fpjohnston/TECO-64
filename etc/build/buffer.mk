#
#  Check to see which buffer handler we should use
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
