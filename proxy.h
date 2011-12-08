#ifndef _PROXY_H_

#define _PROXY_H_
#define L(x) printf("%s\n",x);


#include <stdio.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "config.h"
#include "mempool.h"

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
