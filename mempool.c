#include "mempool.h"

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
