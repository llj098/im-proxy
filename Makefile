
CC = 	gcc
CFLAGS =  -pipe  -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Wunused-function -Wunused-variable -Wunused-value -Werror -g 
CPP =	gcc -E
LINK =	$(CC)

all:
	$(CC) -c $(CFLAGS) proxy.c config.h hashtable.c hashtable.h mempool.c mempool.h ev.h ev.c
