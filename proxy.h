#ifndef _PROXY_H_
#define _PROXY_H_

#define MAX_EVENTS 1000
#define BUFFER_SIZE 20
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
    printf("[DEBUG] %03d.%06d %s L%d P%d " format "\n",		\
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
  int bfd;
  ev_t* ev;
  int connection_n;
  pid_t pid;
  pxy_agent_t *agents;
  mp_pool_t *buf_pool;
  mp_pool_t *buf_data_pool;
  mp_pool_t *agent_pool;
  struct sockaddr_in *baddr;
  int socket_pair[2];
}pxy_worker_t;

typedef struct pxy_master_s{
  int listen_fd;
  mp_pool_t* pool;
  pxy_config_t* config;
  struct sockaddr addr;
  pxy_worker_t **workers;
}pxy_master_t;

typedef struct pxy_command_s{
  #define PXY_CMD_QUIT 1
  pid_t pid;
  int cmd;
  int fd;
}pxy_command_t;


int worker_init();
int worker_start();
int worker_close();
void worker_accept(ev_t*,ev_file_item_t*);
void worker_recv_client(ev_t*,ev_file_item_t*);
void worker_recv_cmd(ev_t*,ev_file_item_t*);

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
