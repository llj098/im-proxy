#include "mempool.h"
#include "proxy.h"

void*
palloc(ssize_t n)
{
  return malloc(n);
}

void 
pfree(void* m)
{
  free(m);
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
