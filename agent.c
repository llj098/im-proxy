
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
  
  if(!agent)
    return -1;
  

  
  /*
  int idx = agent->parse_idx;
  buffer_t *buffer = agent->buffer;
  char* c = NULL;
  
  if(!buffer)
    return 0;
  
  */
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


int 
pxy_agent_prepare_buf(pxy_agent_t *agent,struct iovec *iov,int iovn)
{
  return -1;
}


int
pxy_agent_upstream(pxy_agent_t *agent)
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





