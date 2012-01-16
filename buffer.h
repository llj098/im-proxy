#ifndef __BUFFER_H_
#define __BUFFER_H_

#include "proxy.h"


typedef struct buffer_s {
  void *data;
  list_head_t list;
}buffer_t;

/*
typedef struct stream_s {
  buffer_t *buf;
  uint32_t size; //buf element size 
  uint32_t count; //buf element number 
};
*/

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

#define buffer_for_each(b,blist)		\
  list_for_each_entry(b,list,&(blist)->list)	\

#define buffer_next(b)				\
  list_entry(&(b)->list.next,buffer_t,list)	\

#define buffer_append(b,buffer)			\
  list_append(&b->list,&buffer->list)		\

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
  

static inline char* 
buffer_read(buffer_t *buf,uint32_t offset)
{
  buffer_t *b = buf;
  int n,i;

  n = offset / BUFFER_SIZE;
  i = offset % BUFFER_SIZE;

  while(b != NULL && (n--) >0){
    b = list_entry(&b->list.next,buffer_t,list);
  }
  
  if(b){
    return (char *)b->data + i;
  }
  
  return 0;
  
}

#endif

