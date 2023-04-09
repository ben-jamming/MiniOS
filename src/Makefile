CC=gcc
CFLAGS=

mysh: shell.c interpreter.c shellmemory.c lrucache.c
	$(CC) $(CFLAGS) -D FRAME_STORE_SIZE=$(framesize) -D VAR_STORE_SIZE=$(varmemsize) -c shell.c interpreter.c shellmemory.c kernel.c pcb.c ready_queue.c backingstore.c lrucache.c -lpthread -lm
	$(CC) $(CFLAGS) -o mysh shell.o interpreter.o shellmemory.o kernel.o pcb.o ready_queue.o backingstore.o lrucache.o -lpthread -lm

clean: 
	rm mysh; rm *.o; rm *.txt
