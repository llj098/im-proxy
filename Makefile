
CC = 	gcc
CFLAGS = -pipe  -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Wunused-variable -Wunused-value -Werror -g 
LINK =	$(CC)



LIB_OBJS = \
	./mempool.o \
	./hashtable.o

PXY_OBJS = \
	./proxy.o \
	./ev.o 

OUTPUT = proxy

all:  $(LIB_OBJS) $(PXY_OBJS) 
	$(LINK) $(LIB_OBJS) $(PXY_OBJS) -o $(OUTPUT)

clean:
	rm -f $(PXY_OBJS)
	rm -f $(LIB_OBJS)
	rm -f $(OUTPUT)

ht_test: $(LIB_OBJS)
	$(CC) ht_test.o hashtable.c hashtable.h mempool.c mempool.h list.h buffer.h agent.h agent.c hashtable_test.c

