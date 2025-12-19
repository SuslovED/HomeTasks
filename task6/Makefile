CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
TARGET = tiger_keeper
SRC = tiger_keeper.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJ)