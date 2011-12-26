#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#define HT_INIT_SIZE 16
#include "proxy.h"

typedef void ht_clean(void* d);


typedef struct ht_key_s{
  uint32_t hash;
  uint32_t key;
}ht_key_t;

typedef struct ht_node_s{
  ht_key_t key;
  void* data;
  list_head_t list;
  struct ht_node_s* next;
  int used;
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
int ht_set(ht_table_t* t,uint32_t k,void* v,ht_clean clean);
void* ht_get(ht_table_t* t,uint32_t k);
void ht_remove(ht_table_t* t,uint32_t k,ht_clean clean);


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


static inline void 
ht_node_init(ht_node_t *node,uint32_t hash,uint32_t key,void *d)
{
  node->key.key = key;
  node->key.hash = hash;
  node->data = d;
  INIT_LIST_HEAD(&(node->list));
}


static inline ht_table_t* 
ht_create()
{
  int i;
  ht_table_t *t = malloc(sizeof(*t));
  if(!t)
    goto failed;

  t->len = HT_INIT_SIZE;
  t->nodes = pxy_calloc(sizeof(ht_node_t)*HT_INIT_SIZE);
  if(!t->nodes) 
    goto failed;

  for (i=0; i<HT_INIT_SIZE; ++i) {
    ht_node_init((ht_node_t*)(t->nodes)+i,0,0,NULL);
  }
  
  return t;

 failed:
  if(t)
    FREE(t);

  return NULL;
}

/*This algorith comes from google's snappy*/
static inline uint32_t 
ht_hash_func(uint32_t bytes) 
{
  int shift = 5;
  uint32_t kMul = 0x1e35a7bd;
  return (bytes * kMul) >> shift;
}


#endif 
