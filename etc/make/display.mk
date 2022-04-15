#
#  Check to see if we should enable display mode.
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
    option = error
endif

ifeq ($(option),enable)
    DEFINES += -D DISPLAY_MODE
    LIBS    += -l ncurses
    SOURCES += display.c color_cmd.c key_cmd.c map_cmd.c status.c
else ifeq ($(option),disable)
    SOURCES += stubs.c
else
    $(error Invalid display mode option: $(display))
endif

undefine option
