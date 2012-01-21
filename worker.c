
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
	D("create ev for listen fd error");
	goto start_failed;
    }
    ev_add_file_item(worker->ev,fi);

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
    pxy_agent_for_each(a,worker->agents){
	pxy_agent_close(a);
    }

    worker->ev->stop = 1;
    close(master->listen_fd);
    return 0;
}

void
worker_accept(ev_t *ev, ev_file_item_t *ffi)
{
    int i,f,err;
    pxy_agent_t *agent;
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

	    agent = pxy_agent_new(worker->agent_pool,f,0);
	    if(!agent){
		D("create new agent error"); return;
	    }

	    fi = ev_file_item_new(f,
				  agent,
				  agent_recv_client,
				  NULL,
				  EV_READABLE | EPOLLET);
	    if(!fi){
		D("create file item error");
	    }
	    ev_add_file_item(worker->ev,fi);

	    pxy_agent_append(agent,worker->agents);
	}

	else{ break; }
    }
}

void 
worker_recv_cmd(ev_t *ev,ev_file_item_t *fi)
{
    /* only for quit now */
    worker_close();
}
