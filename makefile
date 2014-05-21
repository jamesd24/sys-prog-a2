CC = clang

all:	sync

sync:	sync.c
		$(CC) -std=gnu99 -Wall -o sync sync.c -lpthread

sema:	sema.c sema.h sematest.c
		$(CC) -std=gnu99 -Wall sematest.c -g -o sematest -lpthread

event:	event_counter.c event_counter.h event_test.c
		$(CC) -std=gnu99 -Wall event_test.c -g -o event_test -lpthread

seq:	seq.c seq.h seq_test.c
		$(CC) -std=gnu99 -Wall seq_test.c -g -o seq_test -lpthread

queue: 	queue.c queue.h queue_test.c queue_test.h
		$(CC) -std=gnu99 -Wall queue_test.c -g -o queue_test -lpthread

clean:
		rm -rf *o *gch sync sematest event_test seq_test queue_test
