CFLAGS = -Wall -g

all: server client 

server: server.c

client: client.c

clean:
	rm -f server client
