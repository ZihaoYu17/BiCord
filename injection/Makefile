KERNEL = $(strip $(shell uname -r))
INCLUDE = -I /usr/src/linux-headers-$(KERNEL)/include
CC_OPTS = -O3 -Wall -static -Wno-unknown-pragmas
CC = gcc

CFLAGS = $(CC_OPTS) $(INCLUDE)

ALL = wifi_send

all: $(ALL)

clean:
	rm $(ALL)

wifi_send: wifi_send.c util.o
	$(CC) $(CFLAGS) -o $@ $^ -lorcon -lm -lrt

util.o: util.c util.h Makefile
