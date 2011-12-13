#ifndef _PROXY_H_

#define _PROXY_H_
#define I(x) printf("INFO:%s\n",x)
#define W(x) printf("WARN:%s\n",x)
#define E(x) printf("ERROR:%s\n",x)
#define MAX_EVENTS 1000
#define pxy_memzero(buf, n)       (void) memset(buf, 0, n)


#include "sysinc.h"
#include "config.h"
#include "mempool.h"
#include "hashtable.h"
#include "ev.h"


typedef struct pxy_config_s{
  short client_port;
  short backend_port;
  int worker_count;
}pxy_config_t;

typedef struct pxy_master_s{
  pxy_config_t* config;
  int listen_fd;
  struct sockaddr add;
}pxy_master_t;

typedef struct pxy_woker{
  int connection_n;
  ev_t* ev;
}prx_worker_t;

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
