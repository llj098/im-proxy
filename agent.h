#ifndef _AGENT_H_
#define _AGENT_H_

#include "proxy.h"


typedef struct pxy_agent_s{
  struct buffer_s *buffer;
  int fd;
  int user_id;
  size_t buf_offset;/*all data len in buf*/
  size_t buf_parsed;
  size_t buf_sent;
  size_t buf_list_n; /* struct buffer_s count */
  list_head_t list;
}pxy_agent_t;

typedef struct message_s {
  uint32_t len;
  uint32_t cmd;
  char *body;
}message_t;

pxy_agent_t* pxy_agent_new(mp_pool_t *,int,int);
void pxy_agent_close(pxy_agent_t *);
int pxy_agent_data_received(pxy_agent_t *);
int pxy_agent_upstream(int ,pxy_agent_t *);
int pxy_agent_echo_test(pxy_agent_t *);
int pxy_agent_buffer_recycle(pxy_agent_t *);
int pxy_prepare_buffer(pxy_agent_t*,int,int*);
buffer_t* agent_get_buf_for_read(pxy_agent_t*);

void agent_recv_client(ev_t *,ev_file_item_t*);

#define pxy_agent_for_each(agent,alist)			\
  list_for_each_entry((agent),list,&(alist)->list)	

#define pxy_agent_append(agent,alist)		\
  list_append(&(agent)->list,&(alist)->list)		

#define pxy_agent_remove(agent)			\
  list_remove(&(agent)->list)

#endif
