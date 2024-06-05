BINARY = eptdump
CC ?= gcc
SRCS=eptdump.c
STRIP ?= strip
STATIC ?= 0

ifeq ($(STATIC), 1)
	LDFLAGS += -static
endif

.PHONY: all clean


all: $(BINARY) 

$(BINARY): $(SRCS)
	$(CC) -flto -fuse-linker-plugin -o $@ $(SRCS)  $(CFLAGS) $(LDFLAGS)
	$(STRIP) $(BINARY)

clean:
	rm -f $(BINARY)
