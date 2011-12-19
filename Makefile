
CC = 	gcc
CFLAGS =  -pipe  -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Wunused-function -Wunused-variable -Wunused-value -Werror -g 
CPP =	gcc -E
LINK =	$(CC)

all:
	$(CC) -c $(CFLAGS) proxy.c config.h hashtable.c hashtable.h mempool.c mempool.h ev.h ev.c connection.c connection.h list.h buffer.h agent.h agent.c

ht_test:
	$(CC) -c $(CFLAGS) hashtable.c hashtable.h mempool.c mempool.h list.h buffer.h agent.h agent.c hashtable_test.c

