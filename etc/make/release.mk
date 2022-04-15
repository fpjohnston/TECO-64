#
#  Update release version number.
#

ifeq ($(release),major)

VERSION = version

else ifeq ($(release),minor)

VERSION = version

else ifeq ($(release),patch)

VERSION = version

else ifdef release

$(error Invalid release version option: $(release))

endif
