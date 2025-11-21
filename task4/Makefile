CC       ?= gcc
CFLAGS    = -g -Wall -Wextra -std=c99 -pedantic

PROGRAMS  = echo pwd ls ln wc cat
SRCS      = $(PROGRAMS:=.c)
OBJS      = $(PROGRAMS:=.o)

all: $(PROGRAMS)

%: %.o
	$(CC) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif

deps.mk: $(SRCS)
	$(CC) -MM $^ > $@

.PHONY: all clean
clean:
	rm -f $(OBJS) $(PROGRAMS) deps.mk