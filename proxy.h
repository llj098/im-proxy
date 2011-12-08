#ifndef _PROXY_H_

#define _PROXY_H_
#define L(x) printf("%s\n",x);

#include "sysinc.h"
#include "config.h"
#include "mempool.h"
#include "hashtable.h"

#define pxy_memzero(buf, n)       (void) memset(buf, 0, n)

typedef struct pxy_master{
  struct socaddr* front_end_addr;
  
} pxy_master_t;

typedef struct pxy_woker{

  struct sockaddr* back_end_addr;


}prx_worker_t;

typedef struct pxy_agent{
  int fd;
  int user_id;
  //so on
}prx_agent_t;

#endif
