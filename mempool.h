#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

#include "sysinc.h"

#define MP_MAX_ALLOC_FROM_POOL  4095
#define MP_MAX_LIST_COUNT 8
#define MP_DEFAULT_LIST_LEN     64				   
#define MALLOC(x) malloc((x))
#define FREE(x) free((x))

typedef struct mp_pool_s{
    void** freelist;
    size_t max;
    size_t size; /*item size*/
    size_t used;
    size_t allocated;
    char name[12];
}mp_pool_t;

struct mp_pool_list_s{
    mp_pool_t *next;
    mp_pool_t *prev;
};


#define mp_pool_list_init(__pool)		\
    ({						\
	struct mp_pool_s* __list;		\
	__list = malloc(sizeof(*__list));	\
	if(__list){				\
	    __list->next = __pool;		\
	    __list->prev = __pool;		\
	}					\
	__list;					\
    })					

mp_pool_t* mp_create(int size,int max,char* name);
void mp_destroy(mp_pool_t *pool);
void* mp_alloc(mp_pool_t* p);
void* mp_calloc(mp_pool_t* p);
void mp_free(mp_pool_t* p,void* d);
void mp_dump(mp_pool_t*);


void* pxy_calloc(size_t size);

#endif
