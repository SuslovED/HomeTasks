all: server client

server: server.c
	gcc server.c -o server -Wall

client: client.c
	gcc client.c -o client -Wall

clean:
	rm -f server client

.PHONY: all clean