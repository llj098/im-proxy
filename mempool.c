#include "proxy.h"

/*
 * fixed length,list style memory pool
 */ 
mp_pool_t*
pcreate(int size,int max,char* name)
{
  mp_pool_t* p = (mp_pool_t*)malloc(sizeof(mp_pool_t*));
  
  if(p){
    p->size = size;
    p->max = max;
    p->freelist = (mp_node_t**)malloc(DEFAULT_POOL_LEN*size);

    if(!(p->freelist))
      return NULL;
 
    if(name)
	strncpy(p->name,name,sizeof(p->name));
 }

  return p;
}

void*
palloc(mp_pool_t* p)
{
  void* d= NULL;
  if(p){
    return p->freelist[0]->data;
  }

  return d;
}

void*
pcalloc(mp_pool_t* p)
{
  void* d = palloc(p);
  if(d){
    pxy_memzero(d,p->size);
  }

  return d;
}

void 
pfree(mp_pool_t* p,void* d)
{
  if(p){
    mp_node_t* node =  (mp_node_t*)d;
    node->next = p->freelist[0];
    p->freelist = &node;
  }
}

void*
pxy_calloc(size_t size)
{
  void* p = malloc(size);
  if(p){
    pxy_memzero(p,size);
  }

  return p;
}
