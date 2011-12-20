#ifndef __BUFFER_H_
#define __BUFFER_H_

#include "proxy.h"

typedef struct buffer_s {
  void *data;
  ssize_t len;/*data len*/
  list_head_t list;
}buffer_t;

#define buffer_fetch(pool,datapool)		\
  ({						\
    buffer_t *__buffer;				\
    __buffer = mp_alloc((pool));		\
    __buffer->data = mp_calloc((datapool));	\
    INIT_LIST_HEAD(&__buffer->list);		\
    __buffer;					\
  })

#define buffer_release(buf,pool,datapool)	\
  ({						\
    mp_free(datapool,buf->data);		\
    mp_free(pool,buf);				\
  })

#define buffer_read_char(buf,idx)		\
  ({						\
    char * __c;					\
    int __i = (idx);				\
    buffer_t *__b = (buf);			\
    while(unlikely(__i > __b->len)){		\
      __i -= BUFFER_SIZE;			\
      __b = __b->next;				\
    }						\
    __c = (char*)__b->data;			\
    __c += i;					\
    __c;					\
  })
  

  

#endif

