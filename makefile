CC = clang

all:	sync

sync:	sync.c
		$(CC) -std=gnu99 -Wall -o sync sync.c -lpthread

sema:	sema.c sema.h sematest.c
		$(CC) -std=gnu99 -Wall sematest.c -g -o sematest -lpthread

event:	event_counter.c event_counter.h event_test.c
		$(CC) -std=gnu99 -Wall event_test.c -g -o event_test -lpthread

clean:
		rm -rf *o *gch sync sematest event_test
