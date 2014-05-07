CC = clang

all:	sync

sync:	sync.c
		$(CC) -std=gnu99 -Wall -o sync sync.c -lpthread

sema:	sema.c sema.h sematest.c
		$(CC) -std=gnu99 -Wall sematest.c -g -o sematest -lpthread

clean:
		rm -rf *o *gch sync sema
