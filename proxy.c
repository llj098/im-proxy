/*
 * Copyright liulijin<llj098@gmail.com>
 */ 

#include "proxy.h"

typedef struct pxy_config_s{
  ushort client_port;
  ushort backend_port;
  int worker_count;
}pxy_config_t;

typedef struct pxy_master_s{
  pxy_confg_t* config;
  int listen_fd;
}pxy_master_t;

typedef struct pxy_woker{
  int connection_n;
  ev_t* ev;
}prx_worker_t;

#define MAX_EVENTS 1000


pxy_master_t* master;
pxy_confg_t* config;
pxy_woker_t* worker;


int 
pxy_init_config()
{
  config = (pxy_master_t*)pxy_calloc(sizeof(*config));

  if(config){
    config->client_port = 9000;
    config->backend_port = 9001;
    config->worker_count = 4;

    return 1;
  }

  return -1;
}

int 
pxy_init_master()
{
  if(pxy_init_config()){
    
    master = (pxy_master_t*)malloc(sizeof(*master));
    master->config = config;

    return 1;
  }
  return -1;
}

int 
pxy_init_worker()
{
  worker = (pxy_woker_t*)malloc(sizeof(*worker));
  if(worker) {
    worker->ev = ev_create();
    
    return worker->ev;
  }

  return -1;
}

int 
pxy_worker_rfunc(ev_t* ev,ev_file_item_t* fi)
{
  int i=0;
  if(fi->fd == master->listen_fd){
    
    for(;i<100;i++){
      /*try to accept 100 times*/
    }
  }
  else{
    /*read the socket data*/
  }
}

int 
pxy_start_worker()
{
  ev_file_item_t* fi ;
  int fd = server->listen_fd;

  fi = ev_file_item_new(fd, NULL, pxy_worker_rfunc, NULL, (worker->ev));

  return fi;
}


int 
pxy_spawn_worker()
{
  int i = 0;

  for(;i<config->worker_count;i++){

    pid_t p = fork();

    if(p < 0) {
      printf("%s","forkerror");
    }
    else if(p == 0){/*child*/
      printf("%d\n",getpid());
      if(pxy_init_worker())
	pxy_start_worker();
    }
    else{/*parent*/
      I("%d\n",getpid());
    }
  }

  return 1;

}

int 
create_shm()
{
  int p = shmget(IPC_PRIVATE,sizeof(4),0600/*user read&writer*/);
  char* ptr;
  if(p>0) {
    printf("shm created, id %d\n",p);
    ptr = shmat(p,0,0);
    ptr = "123";
    printf("%s\n",ptr);
  }
  
  return p;
}

int 
pxy_start()
{
  int s;
  struct sockaddr_in addr1;

  s = pxy_init_master();

  if(!r)
    return -1;
  
  master->fd = listen_fd =socket(AF_INET,SOCK_STREAM,0);
  if(master->listen_fd < 0)
    return -1;
	
  addr1.sin_family = AF_INET;
  addr1.sin_port = htons(config->client_port);
	
  s = bind(master->listen_fd, (struct sockaddr*)&addr1, sizeof(addr1));

  if(s < 0)
    return -1;

  if(listen(listen_fd,1000) < 0)
    return -1;

  return pxy_spawn_worker();
}

int 
main(int len,char** args)
{
  int s = start();
  char st[2];
  printf("%s\n%d","STARTED",s);
  scanf("%s",st);

  return 1;
}


