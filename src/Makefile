CC=gcc
CFLAGS=-c -g -Wall
LDFLAGS=-g -O0

all: mysh

mysh: shell.o interpreter.o shellmemory.o
	gcc $(LDFLAGS) shell.o interpreter.o shellmemory.o -o mysh

shell.o: shell.c
	gcc $(CFLAGS) shell.c

interpreter.o: interpreter.c
	gcc $(CFLAGS) interpreter.c

shellmemory.o: shellmemory.c
	gcc $(CFLAGS) shellmemory.c

clean:
	rm -f *.o mysh
