
#include "proxy.h"

extern pxy_worker_t *worker;


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
pxy_agent_buffer_recycle(pxy_agent_t *agent,int n)
{
  if(!agent)
    return -1;

  int rn;
  
  while(n > BUFFER_SIZE){
    list_remove(&agent->buffer->list);
    buffer_release(agent->buffer,worker->buf_pool,worker->buf_data_pool);
    rn += BUFFER_SIZE;
  }

  
  return rn;
}

void 
pxy_agent_close(pxy_agent_t *agent)
{
  buffer_t *b;

  if(agent->fd) close(agent->fd);

  if(agent->buffer) {
    buffer_for_each(b,agent->buffer){
      if(b){
	if(b->data){
	  mp_free(worker->buf_data_pool,b->data);
	}
	mp_free(worker->buf_pool,b);
      }
    }

    mp_free(worker->buf_pool,agent->buffer);
  }

  mp_free(worker->agent_pool,agent);
}

int 
pxy_agent_prepare_buf(pxy_agent_t *agent,struct iovec *iov,int iovn)
{
  return -1;
}


int
pxy_agent_upstream(int cmd,pxy_agent_t *agent)
{
  if(!agent)
    return -1;

  int n,p,s,i,writen;

  n = 1;
  p = agent->buf_parsed;
  s = agent->buf_sent;


  while( (p-s) > BUFFER_SIZE){ n++; p -= BUFFER_SIZE;}

  struct iovec iov[n];
  
  for(i=0;i<n;i++){

    if(i == 0){

      iov[i].iov_base = ((char *)(agent->buffer->data))+ p;
      iov[i].iov_len = BUFFER_SIZE - p;
      
      continue;
    }

    if(i == n){

      iov[i].iov_base = agent->buffer->data;
      iov[i].iov_len = p % BUFFER_SIZE;
      
      continue;
    }
    
    iov[i].iov_base = agent->buffer->data;
    iov[i].iov_len = BUFFER_SIZE;
  }
  
  writen = writev(worker->bfd,iov,n);
  
  if(writen > 0){
    pxy_agent_buffer_recycle(agent,writen);
  }

  return writen;
}





