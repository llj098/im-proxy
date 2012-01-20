#include "proxy.h"

/*
 * List style memory pool
 */ 

static list_head_t pools = LIST_HEAD_INIT(pools);

mp_pool_t* mp_create(int size,int max,char* name)
{
    mp_pool_t* p = malloc(sizeof(mp_pool_t));
  
    if(p){
	p->size = size;
	p->max = max;
	p->used = 0;
	p->allocated = 0;

	if(name)
	    strncpy(p->name,name,sizeof(p->name));

	list_append(&p->list,&pools);
    }

    return p;
}


void mp_flush(mp_pool_t *pool) 
{
    void *tmp, *next;

    next = pool->freelist;
    while(next) {
	tmp  = next;
	next = *(void**)tmp;
	pool->allocated--;
	FREE(tmp);
    }
    pool->freelist = NULL;
}

void* mp_alloc(mp_pool_t* p)
{
    void* d= NULL;
    
    /* 
     *  we save the 'next' pointer of free list in the data area 
     *  so when we get item from the freelist,we should get the next 
     *  item from the data area 
     */

    if(p->freelist){
	d = (void*)p->freelist;
	p->freelist = *(void**)p->freelist;
    }
    else{
	d = malloc(p->size);
	p->allocated++;
	p->used++;
    }

    return d;
}


void* mp_calloc(mp_pool_t* p)
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
void mp_free(mp_pool_t* p,void* d)
{
    if(p){
	*(void**)d = p->freelist;
	p->freelist = (void**)d;
	p->used--;
    }
}



void* pxy_calloc(size_t size)
{
    void* p = malloc(size);
    if(p){
	pxy_memzero(p,size);
    }

    return p;
}

void mp_dump(mp_pool_t *pool)
{
    printf ("[dump pool]name:%s,size:%d,allocted:%d,used:%d\n",
	    pool->name,
	    pool->size,
	    pool->allocated,
	    pool->used
	);
}

void mp_dump_pools()
{
    mp_pool_t *p;
    mp_pool_for_each(p) {
	mp_dump(p);
    }
}
