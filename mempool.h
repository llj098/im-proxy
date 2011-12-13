#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

#include "sysinc.h"

#define MP_MAX_ALLOC_FROM_POOL  4095
#define MP_MAX_LIST_COUNT 8
#define MP_DEFAULT_LIST_LEN     64				   

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

static int mp_list_count[] = {
  32,
  64,
  128,
  256,
  512,
  1024,
  2048,
  4095
};

mp_pool_t* mp_create(int size,int max,char* name);
void* mp_alloc(mp_pool_t* p);
void* mp_calloc(mp_pool_t* p);
void mp_free(mp_pool_t* p,void* d);
void* pxy_calloc(size_t size);

#endif

