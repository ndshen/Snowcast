cc = gcc
ccflags = -g -I. -std=gnu99 -Wall
execs = server client

.PHONY: all clean

all: $(execs)

server: server.c protocol.c
	$(cc) ${ccflags} -pthread $^ -o $@

client: client.c protocol.c
	$(cc) ${ccflags} $^ -o $@

clean:
	/bin/rm -f *.o $(execs)
