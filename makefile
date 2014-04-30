CC = clang

all:	sync

sync:	sync.c
		$(CC) -std=gnu99 -Wall -o sync sync.c -lpthread
clean:
		rm -rf *o sync
