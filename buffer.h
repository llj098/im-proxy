#ifndef __BUFFER_H_
#define __BUFFER_H_

#include "proxy.h"

typedef struct buffer_s {
  void *data;
  list_head_t list;
}buffer_t;

#define buffer_fetch(pool,datapool)		\
  ({						\
    buffer_t *__buffer;				\
    __buffer = mp_palloc((pool));		\
    __buffer->data = mp_pcalloc((datapool));	\
    INIT_LIST_HEAD(__buffer->list);		\
    __buffer;					\
  })

#define bufffer_release(buf,pool,datapool)	\
  ({						\
    mp_free(datapool,buf->data);		\
    mp_free(pool,buf);				\
  })

#endif
