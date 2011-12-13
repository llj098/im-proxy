#ifndef _PROXY_H_

#define _PROXY_H_
#define I(x) printf("INFO:%s\n",x);
#define W(x) printf("WARN:%s\n",x);
#define E(x) printf("ERROR:%s\n",x);

#include "sysinc.h"
#include "config.h"
#include "mempool.h"
#include "hashtable.h"
#include "ev.h"

#define pxy_memzero(buf, n)       (void) memset(buf, 0, n)

typedef struct pxy_master{
  struct socaddr* front_end_addr;
  
} pxy_master_t;

typedef struct pxy_agent{
  int fd;
  int user_id;
  //so on
}prx_agent_t;

int 
setnonblocking(int sock)
{
  int opts;
  opts = fcntl(sock,F_GETFL);
  if(opts<0) return -1;

  opts = opts | O_NONBLOCK;
  if(fcntl(sock,F_SETFL,opts) < 0)
    return -1;

  return 0;
}

#endif
