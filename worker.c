
#include "proxy.h"

extern pxy_master_t *master;
extern pxy_worker_t *worker;

int worker_init();
int worker_start();
int worker_close();
void worker_accept(ev_t*,ev_file_item_t*);
void worker_recv_client(ev_t*,ev_file_item_t*);
void worker_recv_cmd(ev_t*,ev_file_item_t*);

int 
worker_init()
{
  worker = (pxy_worker_t*)malloc(sizeof(*worker));

  if(worker) {
    worker->ev = ev_create();
    if(!worker->ev){
      D("create ev error"); return -1;
    }

    worker->agent_pool = mp_create(sizeof(pxy_agent_t),0,"AgentPool");
    if(!worker->agent_pool){
      D("create agent_pool error"); return -1;
    }

    worker->buf_data_pool = mp_create(BUFFER_SIZE,0,"BufDataPool");
    if(!worker->buf_data_pool){
      D("create buf_data_pool error"); return -1;
    }

    worker->buf_pool = mp_create(sizeof(buffer_t),0,"BufPool");
    if(!worker->buf_pool) {
      D("create buf_pool error"); return -1;
    }
    
    worker->agents = mp_alloc(worker->agent_pool);
    if(!worker->agents){
      D("create agents error"); return -1;
    }

    INIT_LIST_HEAD(&(worker->agents->list));
    return 0;
  }

  return -1;
}

int 
worker_start()
{
  ev_file_item_t* fi ;
  int fd = master->listen_fd;

  fi = ev_file_item_new(fd, worker, worker_accept, NULL, EV_READABLE);
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
worker_close()
{
  pxy_agent_t *a;
  worker->ev->stop = 1;
  pxy_agent_for_each(a,worker->agents){
    pxy_agent_close(a);
  }

  return 0;
}

void
worker_accept(ev_t *ev, ev_file_item_t *ffi)
{
  int i=0,f,err;
  pxy_agent_t *agent = NULL;
  ev_file_item_t *fi;
  
  for(i=0;i<100;i++){
    /*try to accept 100 times*/
    socklen_t sin_size = sizeof(master->addr);
    f = accept(ffi->fd,&(master->addr),&sin_size);
    D("fd#%d accepted",f);

    if(f>0){

      /* FIXME:maybe we should try best to accept and 
       * delay add events */
      err = setnonblocking(f);
      if(err < 0){
	D("set nonblocking error"); return;
      }

      agent = pxy_agent_new(worker->agent_pool,f,0,NULL);
      if(!agent){
	D("create new agent error"); return;
      }

      fi = ev_file_item_new(f, agent, worker_recv_client, NULL, EV_READABLE);
      if(!fi){
	D("create file item error");
      }
      ev_file_item_ctl(worker->ev,EV_CTL_ADD,fi);

      pxy_agent_append(agent,worker->agents);
    }

    else{ break; }
  }
}


void
worker_recv_client(ev_t* ev,ev_file_item_t* fi)
{
  int iovn=0,i=0,existn=0,readn=0;
  buffer_t *buffer,*bh = NULL;
  void *d = NULL;
  pxy_agent_t *agent = fi->data;

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
      readn -= (BUFFER_SIZE - existn);
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

      /*if(pxy_agent_data_received(agent) < 0){*/
      if(pxy_agent_echo_test(agent) < 0){
	pxy_agent_close(agent);
      }

    }
  }
}

void 
worker_recv_cmd(ev_t *ev,ev_file_item_t *fi)
{
  /* only for quit now */
  worker_close();
}
