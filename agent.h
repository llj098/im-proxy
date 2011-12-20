#ifndef _AGENT_H_
#define _AGENT_H_

#include "proxy.h"

typedef struct pxy_agent_s{
  struct buffer_s *buffer;
  int fd;
  int user_id;
  int parse_idx;
  int sent;
 //so on...
}pxy_agent_t;


#define pxy_agent_new(__pool,__fd,__userid,__buffer)	\
  ({							\
    pxy_agent_t *__agent;				\
    __agent = mp_alloc((__pool));			\
    __agent->fd = (__fd);				\
    __agent->user_id = (__userid);			\
    __agent->buffer = (__buffer);			\
    __agent->parse_idx = 0;				\
    __agent->sent = 0;					\
    __agent;						\
  })




#endif
