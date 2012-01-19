#include "proxy.h"

extern pxy_worker_t *worker;

static int 
pxy_agent_send2(pxy_agent_t *agent,int fd)
{
  int i = 0;
  ssize_t n;
  void *data;
  buffer_t *b = agent->buffer;
  
  n = agent->buf_parsed - agent->buf_sent;

  while(b) {
    
    if(i == 0){
      data = (void*)((char*)b->data + agent->buf_sent);
      n    = send(fd, data, b->len-agent->buf_sent,0);
      D("i:%d",i);
    }
    else{
      data = b->data;
      n    = send(fd, data, b->len,0);
      D("i:%d",i);
    }

    D("n:%d, fd #%d, errno :%d, EAGAIN:%d", n,fd,errno,EAGAIN);
      

    if(n < 0) {
      D("M");
      if(errno == -EAGAIN || errno == -EWOULDBLOCK) {
	return 0;
      }
      else {
	pxy_agent_close(agent);
	pxy_agent_remove(agent);
	return -1;
      }
    }

    agent->buf_sent += n;
    b = b->next;
    i++;
  }

  pxy_agent_buffer_recycle(agent);

  return 0;
}


int
pxy_agent_downstream(pxy_agent_t *agent)
{
  return pxy_agent_send2(agent,agent->fd);
}

int 
agent_echo_read_test(pxy_agent_t *agent)
{
  char *c;
  int i = 0;
  buffer_t *b = agent->buffer;
  
  while((c = buffer_read(b,i)) != NULL) {

    if(*c == 'z') {
	
      char *c1 = buffer_read(b,i+1);
      char *c2 = buffer_read(b,i+2);

      if(c1!=NULL && c2!=NULL && *c1 == '\r' && *c2 =='\n'){
	agent->buf_parsed = i+2+1;
	i+=2; 
      }
    }

    i++;
  }

  D("the agent->offset:%d,agent->sent:%d,agent->parsed:%d",
    agent->buf_offset,
    agent->buf_sent,
    agent->buf_parsed);
      
  if(agent->buf_parsed > agent->buf_sent &&
     pxy_agent_downstream(agent) < 0){
    D("down finish < 0");
    pxy_agent_remove(agent);
    pxy_agent_close(agent);
  }
  return 0;
}

int
pxy_agent_data_received(pxy_agent_t *agent)
{
  /*
   * parse the data, and send the packet to the upstream
   *
   * proto format:
   * 00|len|cmd|content|00
   */

  int idx,n,i = 0;
  char *c;

  n = agent->buf_offset - agent->buf_sent;

  if(n){
    idx = agent->buf_sent;

    int cmd = -1,len = 0,s = 0;
    
    
    while((i++ < n) && (c=buffer_read(agent->buffer,idx)) != NULL) {
      
      /*parse state machine*/
      switch(s){
      case 0:
	if(*c != 0)
	  return -1;
	s++; idx++;
	break;
      case  1:
	len = (int)*c;
	s++; idx++;
	break;
      case  2:
	cmd = (int)*c;
	s++; idx += len-3;
	break;
      case  3:
	if(*c !=0)
	  return -1;

	/*on message parse finish*/
	s=0; idx++;
	agent->buf_parsed = idx;
	break;
      }
    }

    pxy_agent_upstream(cmd,agent);
  }

  return 0;
}

int 
pxy_agent_buffer_recycle(pxy_agent_t *agent)
{
  int rn      = 0;
  size_t n    = agent->buf_sent;
  buffer_t *b = agent->buffer,*t;

  if(b){
    D("before recycle:the agent->offset:%d,agent->sent:%d,agent->parsed:%d",
      agent->buf_offset,
      agent->buf_sent,
      agent->buf_parsed);
  }

  while(b!=NULL && n >= b->len) {
    n  -= b->len;
    rn += b->len;

    t = b->next;
    buffer_release(b,worker->buf_pool,worker->buf_data_pool);
    b = t;

    D("b:%p,n:%d",b,n);
  }

  agent->buffer     = b;
  agent->buf_sent   -= rn;
  agent->buf_parsed -= rn;

  D("after recycle:the agent->offset:%d,agent->sent:%d,agent->parsed:%d",
    agent->buf_offset,
    agent->buf_sent,
    agent->buf_parsed);
   
  return rn;
}

void 
pxy_agent_close(pxy_agent_t *agent)
{
  D("pxy_agent_close fired");
  buffer_t *b;

  if(agent->fd > 0){
    /* close(agent->fd); */
    D("close the socket");
    close(agent->fd);
  }

  while(agent->buffer){
    b = agent->buffer;
    agent->buffer = b->next;

    if(b->data){
      mp_free(worker->buf_data_pool,b->data);
    }
    mp_free(worker->buf_pool,b);
  }

  mp_free(worker->agent_pool,agent);
  D("pxy_agent_close finished");
}

buffer_t* 
agent_get_buf_for_read(pxy_agent_t *agent)
{
  buffer_t *b = buffer_fetch(worker->buf_pool,worker->buf_data_pool);

  if(b == NULL) {
    D("no buf available"); 
    return NULL;
  }

  if(agent->buffer == NULL) {
    agent->buffer = b;
  }
  else {
    buffer_append(b,agent->buffer);
  }

  return b;
}

int 
pxy_agent_prepare_buf(pxy_agent_t *agent,struct iovec *iov,int iovn)
{
  return -1;
}


int
pxy_agent_upstream(int cmd,pxy_agent_t *agent)
{
  D("UP");
  return pxy_agent_send2(agent,worker->bfd);
}


pxy_agent_t *
pxy_agent_new(mp_pool_t *pool,int fd,int userid)
{
  pxy_agent_t *agent = mp_alloc(pool);
  if(!agent){
    D("no mempry for agent"); 
    goto failed;
  }

  agent->fd         = fd;
  agent->user_id    = userid;
  agent->buf_sent   = 0;
  agent->buf_parsed = 0;
  agent->buf_offset = 0;
  agent->buf_list_n = 0;

  return agent;

 failed:
  if(agent){
    mp_free(worker->agent_pool,agent);
  }
  return NULL;
}

void
agent_recv_client(ev_t *ev,ev_file_item_t *fi)
{
  int n;
  buffer_t *b;
  pxy_agent_t *agent = fi->data;

  if(!agent){
    W("fd has no agent,ev->data is NULL,close the fd");
    close(fi->fd); return;
  }

  while(1) {

    b = agent_get_buf_for_read(agent);

    if(b == NULL) {
      pxy_agent_close(agent);
      pxy_agent_remove(agent);
    }

    n = recv(fi->fd,b->data,BUFFER_SIZE,0);
    D("recv %d bytes",n);

    if(n < 0){
      if(errno == EAGAIN || errno == EWOULDBLOCK) {
	break;
      }
      else {
	D("read error,errno is %d",errno);
	goto failed;
      }
    }

    if(n == 0){
      D("socket fd #%d closed by peer",fi->fd);
      goto failed;
    }

    b->len = n;

    if(n < BUFFER_SIZE) {
      break;
    }
  }

  if(agent_echo_read_test(agent) < 0){
    pxy_agent_close(agent);
  }

  return;

 failed:
  pxy_agent_close(agent);
  pxy_agent_remove(agent);
  return;
}

