#ifndef _PROXY_H_
#define _PROXY_H_

#define MAX_EVENTS 1000
#define BUFFER_SIZE 1460
#define pxy_memzero(buf, n)       (void) memset(buf, 0, n)

#include "sysinc.h"
#include "ev.h"
#include "list.h"
#include "buffer.h"
#include "config.h"
#include "mempool.h"
#include "hashtable.h"
#include "agent.h"

#define D(format,...)						\
  do {								\
    struct timeval __xxts;					\
    gettimeofday(&__xxts,NULL);					\
    printf("%03d.%06d %s [%d] pid:[%d] " format "\n",		\
	   (int)__xxts.tv_sec % 1000, (int) __xxts.tv_usec,	\
	   __FUNCTION__,__LINE__,getpid(),##__VA_ARGS__);	\
  }while(0)							\

#define I(x) printf("INFO:%s\n",x)
#define W(x) printf("WARN:%s\n",x)
#define E(x) printf("ERROR:%s\n",x)

typedef struct pxy_config_s{
  short client_port;
  short backend_port;
  int worker_count;
}pxy_config_t;

typedef struct pxy_worker_s{
  int connection_n;
  int bfd;
  struct sockaddr_in *baddr;
  ev_t* ev;
  //ht_table_t* conns;
  mp_pool_t *buf_pool;
  mp_pool_t *buf_data_pool;
  mp_pool_t *agent_pool;
}pxy_worker_t;

typedef struct pxy_master_s{
  pxy_config_t* config;
  int listen_fd;
  struct sockaddr addr;
  mp_pool_t* pool;
}pxy_master_t;


static inline int setnonblocking(int sock)
{
  int opts;
  opts = fcntl(sock,F_GETFL);
  if(opts<0) return -1;

  opts = opts | O_NONBLOCK;
  if(fcntl(sock,F_SETFL,opts) < 0)
    return -1;

  return 0;
}

static inline void iov_init(struct iovec *iov,void *base,int len)
{
  iov->iov_base = base;
  iov->iov_len = len;
}

#endif
