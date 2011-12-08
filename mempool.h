#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

#include "sysinc.h"


void* palloc(ssize_t n);

void pfree(void* m);

void* pxy_calloc(size_t size);



#endif

