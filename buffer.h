#ifndef __BUFFER_H_
#define __BUFFER_H_

#include "proxy.h"


typedef struct buffer_s {
  void            *data;
  size_t           len;
  struct buffer_s *next;
}buffer_t;

#define buffer_fetch(pool,datapool)		\
  ({						\
    buffer_t *__buffer;				\
    __buffer = mp_alloc((pool));		\
    __buffer->data = mp_calloc((datapool));	\
    __buffer;					\
  })

#define buffer_release(buf,pool,datapool)	\
  ({						\
    buf->len = 0;				\
    buf->next = NULL;				\
    mp_free(datapool,buf->data);		\
    mp_free(pool,buf);				\
  })

static inline void 
buffer_append(buffer_t *b,buffer_t *head)
{
  buffer_t *bb = head;
  while(bb->next) { bb = bb->next; }
  bb->next = b;
}

static inline char * 
buffer_read(buffer_t *buf,size_t offset)
{
  buffer_t *b = buf;
  size_t n    = offset;

  while(b!= NULL && n >= b->len) {
    n -= b->len;
    b = b->next;
  }
  
  if(b){
    return (char *)b->data + n;
  }
  
  return NULL;
}

#endif

