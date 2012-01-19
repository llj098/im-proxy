/*
 * Copyright liulijin<llj098@gmail.com>
 */ 

#include "proxy.h"

pxy_master_t* master;
pxy_config_t* config;
pxy_worker_t* worker;

int 
pxy_init_config()
{
  config = (pxy_config_t*)pxy_calloc(sizeof(*config));

  if(config){
    config->client_port = 9000;
    config->backend_port = 9001;
    config->worker_count = 1;

    return 1;
  }

  return -1;
}

void
pxy_master_close()
{
  if(master->listen_fd > 0){
    D("close the listen fd");
    close(master->listen_fd);
  }
  
  /* TODO: Destroy the mempool */
}

int
pxy_send_command(pxy_worker_t *w,int cmd,int fd)
{
  struct msghdr m;
  struct iovec iov[1];
  pxy_command_t *c = malloc(sizeof(*c));

  if(!c) {
    D("no memory");
    return -1;
  }

  c->cmd = cmd;
  c->fd = fd;
  c->pid = w->pid;

  iov[0].iov_base = c;
  iov[0].iov_len = sizeof(*c);

  m.msg_name = NULL;
  m.msg_namelen = 0;
  m.msg_control = NULL;
  m.msg_controllen = 0;
  
  m.msg_iov = iov;
  m.msg_iovlen = 1;

  int a;
  if((a=sendmsg(w->socket_pair[0],&m,0)) < 0) {
    D("send failed%d",a);
    return  -1 ;
  }

  D("master sent cmd:%d to pid:%d fd:%d", cmd, w->pid, fd);
  return 0;
}

int 
pxy_start_listen()
{
  struct sockaddr_in addr1;

  master->listen_fd =socket(AF_INET,SOCK_STREAM,0);
  if(master->listen_fd < 0){
    D("create listen fd error");
    return -1;
  }

  if(setnonblocking(master->listen_fd) < 0){
    D("set nonblocling error");
    return -1;
  }

  addr1.sin_family = AF_INET;
  addr1.sin_port = htons(config->client_port);
  addr1.sin_addr.s_addr = 0;
	
  if(bind(master->listen_fd, (struct sockaddr*)&addr1, sizeof(addr1)) < 0){
    D("bind error");
    return -1;
  }

  if(listen(master->listen_fd,1000) < 0){
    D("listen error");
    return -1;
  }
    
  return 0;
}

int 
pxy_init_master()
{
  if(!pxy_init_config()){
    D("config initialize error");
    return -1;
  }
 
  master = (pxy_master_t*)malloc(sizeof(*master));
  if(!master){
    D("no memory for master");
    return -1;
  }
  master->config = config;
  master->workers = 
    (pxy_worker_t**)malloc(config->worker_count * sizeof(pxy_worker_t));

  if(!master->workers) {
    D("no memory for workers");
    return -1;
  }

  return pxy_start_listen();
}


int 
main(int len,char** args)
{
  /* char p[80]; */
  int i=0;
  char ch[80];
  pxy_worker_t *w;

  if(pxy_init_master() < 0){
    D("master initialize failed");
    return -1;
  }
 
  D("master initialized");

  /*spawn worker*/

  for(;i<config->worker_count;i++){
  
    w = (pxy_worker_t*)(master->workers + i);

    if(socketpair(AF_UNIX,SOCK_STREAM,0,w->socket_pair) < 0) {
      D("create socket pair error"); continue;
    }

    if(setnonblocking(w->socket_pair[0]) < 0) {
      D("setnonblocking error fd:#%d",w->socket_pair[0]); continue;
    }
 
    if(setnonblocking(w->socket_pair[1]) < 0) {
      D("setnonblocking error fd:#%d",w->socket_pair[1]); continue;
    }
    
    pid_t p = fork();

    if(p < 0) {
      D("%s","forkerror");
    }
    else if(p == 0){/*child*/

      if(worker_init()<0){
	D("worker #%d initialized failed" , getpid());
	return -1;
      }
      D("worker #%d initialized success", getpid());


      close (w->socket_pair[0]); /*child should close the pair[0]*/
      ev_file_item_t *f = ev_file_item_new(w->socket_pair[1],
					   worker,
					   worker_recv_cmd,
					   NULL,
					   EV_READABLE | EPOLLET);
      if(!f){ 
	D("new file item error"); return -1; 
      }
      if(ev_file_item_ctl(worker->ev,EV_CTL_ADD,f) < 0) {
	D("add event error"); return -1;
      }


      if(!worker_start()) {
	D("worker #%d started failed", getpid()); return -1;
      }
    }
    else{ /*parent*/
      w->pid = p;
      close(w->socket_pair[1]); /*parent close the pair[1]*/
    }

  }


  while(scanf("%s",ch) >= 0 && strcmp(ch,"quit") !=0){ 
  }

  w = (pxy_worker_t*)master->workers;
  pxy_send_command(w,PXY_CMD_QUIT,-1);

  sleep(5);
  pxy_master_close();
  return 1;
}
