CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

TARGET = task3
OBJS = main.o words.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c words.h
	$(CC) $(CFLAGS) -c main.c

words.o: words.c words.h
	$(CC) $(CFLAGS) -c words.c

clean:
	rm -f $(OBJS) $(TARGET)