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
  worker = (pxy_worker_t*)malloc(sizeof(*worker));
  if(worker) {
    worker->ev = ev_create();
    worker->pool = mp_create(BUFFER_SIZE,0,"WorkerBufPool");
    
    if(worker->ev != NULL)
      return 1;
    else
      return -1;
  }

  return -1;
}

void
pxy_worker_client_rfunc(ev_t* ev,ev_file_item_t* fi)
{
  int i,f,n,readn=0;
  buffer_t *buffer,*buffer_head = NULL;

  if(fi->fd == master->listen_fd){
    for(i=0;i<100;i++){
      /*try to accept 100 times*/
      socklen_t sin_size = sizeof(master->addr);
      f = accept(master->listen_fd,&(master->addr),&sin_size);
      if(f>0){

	/* FIXME:maybe we should try best to accept and 
	 * delay add events */
	ev_file_item_new(f,NULL,pxy_worker_client_rfunc,NULL,EV_READABLE);
      }
      else{
	break;
      }
    }
  }
  else{

    if( ioctl(fi->fd,FIONREAD,&readn) >0 && readn > 0) {

      n = readn / BUFFER_SIZE + (((readn % BUFFER_SIZE) > 0) ? 1 : 0);

      struct iovec iov[n];
      buffer_head = buffer_fetch(worker->pool,worker->datapool);
      buffer_head->len = readn> BUFFER_SIZE ? BUFFER_SIZE : readn;
      iov[i].iov_base = buffer_head->data;
      iov[i].iov_len = buffer_head->len;
      readn -= BUFFER_SIZE;

      for(i=1; i < n ;i++){

	buffer = buffer_fetch(worker->pool,worker->datapool);
	buffer->len = readn> BUFFER_SIZE ? BUFFER_SIZE : readn;
	list_append(&buffer->list,&buffer_head->list);

	iov[i].iov_base = buffer->data;
	iov[i].iov_len = BUFFER_SIZE;
	readn -= BUFFER_SIZE;
      }
      
      readn = readv(fi->fd,iov,n);
      if(readn){
	//handle received data
      }
      
    }
  }
}

int 
pxy_start_worker()
{
  ev_file_item_t* fi ;
  int fd = master->listen_fd;

  fi = ev_file_item_new(fd, NULL, pxy_worker_client_rfunc, NULL, EV_READABLE);

  if(fi)
    return 1;
  else
    return -1;
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
      /*I("%d\n",getpid());*/
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

  if(!s)
    return -1;
  
  master->listen_fd =socket(AF_INET,SOCK_STREAM,0);
  if(master->listen_fd < 0)
    return -1;
	
  addr1.sin_family = AF_INET;
  addr1.sin_port = htons(config->client_port);
	
  s = bind(master->listen_fd, (struct sockaddr*)&addr1, sizeof(addr1));

  if(s < 0)
    return -1;

  if(listen(master->listen_fd,1000) < 0)
    return -1;

  return pxy_spawn_worker();
}

int 
main(int len,char** args)
{
  int s = pxy_start();
  char st[2];
  printf("%s\n%d","STARTED",s);
  scanf("%s",st);

  return 1;
}


