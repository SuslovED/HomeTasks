CC       ?= gcc
CFLAGS    = -g -Wall -Wextra -std=c99 -pedantic

SRCMODULES = words.c
OBJMODULES = $(SRCMODULES:.c=.o)

TARGET = task3

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): main.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif

deps.mk: main.c $(SRCMODULES)
	$(CC) -MM $^ > $@

.PHONY: clean
clean:
	rm -f *.o $(TARGET) deps.mk
