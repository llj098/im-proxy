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
pxy_worker_master_rfunc(ev_t *ev, ev_file_item_t *fi)
{
  /* only for quit now */
  ev->stop = 1;
}

void
pxy_worker_client_rfunc(ev_t* ev,ev_file_item_t* fi)
{
  D("func fired!");
  int iovn=0,i=0,existn=0,readn=0,f;
  buffer_t *buffer,*bh = NULL;
  void *d = NULL;
  pxy_agent_t *agent = NULL;
  

  if(fi->fd == master->listen_fd){
    for(i=0;i<100;i++){
      /*try to accept 100 times*/
      socklen_t sin_size = sizeof(master->addr);
      f = accept(master->listen_fd,&(master->addr),&sin_size);

      if(f>0){
	/* FIXME:maybe we should try best to accept and 
	 * delay add events */
	setnonblocking(f);
	D("SET NON BLOCKING");
	agent = pxy_agent_new(worker->agent_pool,f,0,NULL);
	D("NEW AGENT");

	ev_file_item_t *fi = ev_file_item_new(f,
					      agent,
					      pxy_worker_client_rfunc,
					      NULL,
					      EV_READABLE);

	if(fi){
	  ev_file_item_ctl(worker->ev,EV_CTL_ADD,fi);
	  D("new file item");
	}
	else {
	  D("file ev item is null");
	}
	
      }
      else{
	break;
      }
    }
  }
  else{

    agent = fi->data;
    if(!agent){
      W("fd has no agent,ev->data is NULL,close the fd");
      close(fi->fd);
      return;
    }

    ioctl(fi->fd,FIONREAD,&readn);
    D("FD:#%d,data to read :%d",fi->fd,readn);

    if(readn > 0) {
      existn = agent->buf_offset % BUFFER_SIZE;

      if(existn > 0){
	iovn = 1;
	readn -= BUFFER_SIZE - existn;
      }

      iovn += readn / BUFFER_SIZE + (((readn % BUFFER_SIZE) > 0) ? 1 : 0);
      struct iovec iov[iovn];
      D("existn:%d,agent->buf_offset:%d,iovn:%d",existn,agent->buf_offset,iovn);

      if(existn > 0 && agent->buffer){

	bh = list_entry(&(agent->buffer->list.prev), buffer_t, list);
	d = (void*)((char*)bh->data + existn);

	iov_init(&(iov[0]), d, BUFFER_SIZE - existn);
	readn -= BUFFER_SIZE;
	i = 1;
      }

      for(; i < iovn ;i++){

	buffer = buffer_fetch(worker->buf_pool,worker->buf_data_pool);

	if(i == 0){
	  bh = buffer;
	}
	
	list_append(&buffer->list,&bh->list);

	iov_init(&(iov[i]), buffer->data, BUFFER_SIZE);
	readn -= BUFFER_SIZE;
      }
      
      readn = readv(fi->fd,iov,iovn);
      D("readv returns :%d",readn);
      if(readn > 0){

	if(!agent->buffer){ agent->buffer = bh; }
	list_append(&bh->list,&agent->buffer->list);

	agent->buf_offset += readn;

	D("Call echo test");
	/*if(pxy_agent_data_received(agent) < 0){*/
	if(pxy_agent_echo_test(agent) < 0){
	  pxy_agent_close(agent);
	}

      }
    }
  }
}


int 
pxy_start_worker()
{
  ev_file_item_t* fi ;
  int fd = master->listen_fd;

  fi = ev_file_item_new(fd, worker, pxy_worker_client_rfunc, NULL, EV_READABLE);

  if(!fi){
    goto start_failed;
  }

  ev_file_item_ctl(worker->ev,EV_CTL_ADD,fi);

  /* TODO: Backend todo
     worker->bfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
     if(!worker->bfd){
     goto start_failed;
     }
  
     if(!connect(worker->bfd,(struct sockaddr*)worker->baddr,
     sizeof(*(worker->baddr)))){
     goto start_failed;
     }
  */
  
  ev_main(worker->ev);

  return 1;

 start_failed:
  return -1;
}


int 
pxy_init_worker()
{
  worker = (pxy_worker_t*)malloc(sizeof(*worker));
  if(worker) {
    worker->ev = ev_create();
    worker->agent_pool = mp_create(sizeof(pxy_agent_t),0,"AgentPool");
    worker->buf_data_pool = mp_create(BUFFER_SIZE,0,"BufDataPool");
    worker->buf_pool = mp_create(sizeof(buffer_t),0,"BufPool");
    

    if(worker->ev != NULL)
      return 1;
    else
      return -1;
  }

  return -1;
}

int
pxy_send_command(pxy_worker_t *w,int cmd,int fd)
{
  D("F");
  struct msghdr m;
  struct iovec iov[1];
  pxy_command_t *c = malloc(sizeof(*c));

  if(!c) {
    D("no memory");
    return -1;
  }

  c->cmd = cmd;
  c->fd = fd;
  D("F,%p",w);
  c->pid = w->pid;
  D("F");

  iov[0].iov_base = c;
  iov[0].iov_len = sizeof(*c);

  m.msg_name = NULL;
  m.msg_namelen = 0;
  m.msg_control = NULL;
  m.msg_controllen = 0;
  
  m.msg_iov = iov;
  m.msg_iovlen = 1;

  D("F %d",w->socket_pair[0]);
  int a;
  if((a=sendmsg(w->socket_pair[0],&m,0)) < 0) {
    D("send failed%d",a);
    return  -1 ;
  }

  D("F");
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
      D("create socket pair error");
      continue;
    }

    if(setnonblocking(w->socket_pair[0]) < 0) {
      D("setnonblocking error fd:#%d",w->socket_pair[0]);
    }
 
    if(setnonblocking(w->socket_pair[1]) < 0) {
      D("setnonblocking error fd:#%d",w->socket_pair[1]);
    }
    
    pid_t p = fork();

    if(p < 0) {
      D("%s","forkerror");
    }
    else if(p == 0){/*child*/

      if(pxy_init_worker()<0){
	D("worker #%d initialized failed" , getpid());
	return -1;
      }
      D("worker #%d initialized success", getpid());


      close (w->socket_pair[0]); /*child should close the pair[0]*/
      ev_file_item_t *f = ev_file_item_new(w->socket_pair[1],
					   worker,
					   pxy_worker_master_rfunc,
					   NULL,
					   EV_READABLE);
      if(!f){ 
	D("new file item error"); return -1; 
      }
      if(ev_file_item_ctl(worker->ev,EV_CTL_ADD,f) < 0) {
	D("add event error"); return -1;
      }


      if(!pxy_start_worker()) {
	D("worker #%d started failed", getpid()); return -1;
      }
    }
    else{ /*parent*/
      w->pid = p;
      close(w->socket_pair[1]); /*parent close the pair[1]*/
    }
  }


  while(scanf("%s",ch) >= 0 && strcmp(ch,"quit") !=0){ 
    w = (pxy_worker_t*)master->workers;
    pxy_send_command(w,PXY_CMD_QUIT,-1);
  }


  return 1;
}
