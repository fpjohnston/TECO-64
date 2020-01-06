TARGET = teco

CC = gcc

CFLAGS = -std=gnu18 -Wall -Wextra -Wno-unused-parameter -fshort-enums

INCDIR = include

INCLUDES = -I ../$(INCDIR)

VPATH=src:obj:$(INCDIR)

LIBS =

SOURCES = a_cmd.o

OBJECTS = $(SOURCES:.c=.o)

CFLAGS += -MMD -c $(INCLUDES) $(DFLAGS)

%.o: %.c
	chdir obj && $(CC) @../CFLAGS ../$<

%.d: %.c
	chdir obj && $(CC) @../CFLAGS ../$<

.PHONY: $(TARGET) 
$(TARGET): bin/$(TARGET)

bin/$(TARGET): $(OBJECTS)
	chdir obj && $(CC) -Xlinker -Map=../$@.map $(DFLAGS) -o ../$@ $(OBJECTS) $(LIBS) -lncurses

include a_cmd.d

$(OBJECTS): CFLAGS

.PHONY: FORCE
CFLAGS: FORCE
	-echo '$(CFLAGS)' | cmp -s - $@ || echo '$(CFLAGS)' > $@

