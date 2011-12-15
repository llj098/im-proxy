#include "proxy.h"

/*
 * List style memory pool
 */ 


mp_pool_t*
mp_create(int size,int max,char* name)
{
  mp_pool_t* p = malloc(sizeof(mp_pool_t));
  
  if(p){
    p->size = size;
    p->max = max;
    p->used = 0;
    p->allocated = 0;

    if(name)
      strncpy(p->name,name,sizeof(p->name));
  }

  return p;
}


void*
mp_alloc(mp_pool_t* p)
{
  void* d= NULL;

  if(p->freelist){
    d = (void*)p->freelist;
    p->freelist = *(void**)p->freelist;
  }
  else{
    d = malloc(p->size);
  }

  return d;
}


void*
mp_calloc(mp_pool_t* p)
{
  void* d = mp_alloc(p);
  if(d){
    pxy_memzero(d,p->size);
  }

  return d;
}


/*
 * we use the data area to save the 'next' pointer 
 */
void 
mp_free(mp_pool_t* p,void* d)
{
  if(p){
    *(void**)d = p->freelist;
    p->freelist = (void**)d;
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
