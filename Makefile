
CC = 	gcc
CFLAGS = -pipe  -O -W -Wall -Wno-unused-parameter -g
LINK =	$(CC)



LIB_OBJS = \
	./ev.o \
	./mempool.o \
	./hashtable.o \
	./rbtree.o

PXY_OBJS = \
	./worker.o \
	./proxy.o \
	./agent.o \

PXY_TEST = \
	./pxy_test.o \

HT_TEST = \
	./hashtable_test.o \

TEST = \
	$(PXY_TEST) \
	$(HT_TEST)  \

OUTPUT = proxy

all:  $(LIB_OBJS) $(PXY_OBJS) 
	$(LINK) $(LIB_OBJS) $(PXY_OBJS) -o $(OUTPUT)

clean:
	rm -f $(PXY_OBJS)
	rm -f $(LIB_OBJS)
	rm -f $(OUTPUT)
	rm -f $(TEST)

ht_test: $(LIB_OBJS) $(HT_TEST)
	$(LINK) $(LIB_OBJS) $(TEST) -o $@

