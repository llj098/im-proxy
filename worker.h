#ifndef _WORKER_H_
#define _WORKER_H_

#include "proxy.h"

typedef struct pxy_worker_s{
  int connection_n;
  ev_t* ev;
  //ht_table_t* conns;
  mp_pool_t* pool;
}pxy_worker_t;

#endif
