#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#define HT_INIT_SIZE 16
#include "sysinc.h"
#include "mempool.h"

typedef struct ht_key_s{
  uint32_t hash;
  uint32_t key;
}ht_key_t;

typedef struct ht_node_s{
  ht_key_t key;
  void* data;
  struct ht_node_s* next;
}ht_node_t;

typedef struct ht_table_s{ 
  int alloced;
  int used;
  int len;
  ht_node_t **nodes;
  mp_pool_t* pool;
}ht_table_t;

int ht_resize(ht_table_t* t);
int ht_key_compare(ht_key_t k1,ht_key_t k2);
int ht_set(ht_table_t* t,uint32_t k,void* v);
void* ht_get(ht_table_t* t,uint32_t k);
void* ht_remove(ht_table_t* t,uint32_t k);


#define ht_node_from(__src,__dst)		\
  ({						\
    (__dst)->data = (__src)->data;		\
    (__dst)->key.hash = (__src)->key.hash;	\
    (__dst)->key.key = (__src)->key.key;	\
  })

#define ht_key_init(__key,__len)		\
  ({						\
    ht_key_t __k;				\
    __k.key = __key;				\
    __k.len = __len;				\
    __k;					\
  })


static inline ht_table_t* ht_create()
{
  ht_table_t *t = pxy_calloc(sizeof(*t));
  if(!t)goto failed;
  t->len = HT_INIT_SIZE;
  t->pool = mp_create(sizeof(ht_node_t),0,"HT");
  if(!t->pool)goto failed;
  t->nodes = mp_alloc(t->pool);
  if(!t->nodes) goto failed;

  return t;

 failed:
  if(t->pool)
    mp_destroy(t->pool);
  if(t)
    free(t);

  return NULL;
}

static inline int ht_init(ht_table_t* t)
{
  if(t == NULL)
    return -1;

  t->len = HT_INIT_SIZE;
  t->nodes = (ht_node_t**)calloc(1,sizeof(struct ht_node_s) 
				 * HT_INIT_SIZE);
  if(!t->nodes)
    return -1;

  return 0;
}


/*This algorith comes from google's snappy*/
static inline uint32_t ht_hash_func(uint32_t bytes) 
{
  int shift = 5;
  uint32_t kMul = 0x1e35a7bd;
  return (bytes * kMul) >> shift;
}


#endif 
