#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

#include "sysinc.h"

#define MAX_ALLOC_FROM_POOL  4095
#define DEFAULT_POOL_LEN     64				   


typedef struct mp_node_s{
  void* data;
  struct mp_node_s* next;
}mp_node_t;

typedef struct mp_pool_s{
  mp_node_t** freelist;
  size_t size; /*item size*/
  size_t len; /*totoal len*/
  size_t max;
  size_t aligned;
  uint32_t current;
  char name[12];
}mp_pool_t;

mp_pool_t* pcreate(int size,int max,char* name);
void* palloc(mp_pool_t* p);
void* pcalloc(mp_pool_t* p);
void pfree(mp_pool_t* p,void* d);
void* pxy_calloc(size_t size);



#endif

