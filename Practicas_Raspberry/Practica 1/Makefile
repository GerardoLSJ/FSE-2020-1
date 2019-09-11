CC = gcc
CFLAGS = -O2 -Wall
LFLAGS =

all:	hello_world something_cool

hello_world:	hello_world.o
	$(CC) $(LFLAGS) -o hello_world hello_world.o

hello_world.o:	hello_world.c
	$(CC) $(CFLAGS) -c hello_world.c

something_cool:	something_cool.o
	$(CC) $(LFLAGS) -o something_cool something_cool.o

something_cool.o:	something_cool.c
	$(CC) $(CFLAGS) -c something_cool.c

submit:
	tar -czvf code_P1_FSE.tar.gz README Makefile hello_world.c something_cool.c

clean:	
	rm -f *~ *.o hello_world something_cool

