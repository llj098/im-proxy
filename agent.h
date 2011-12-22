#ifndef _AGENT_H_
#define _AGENT_H_

#include "proxy.h"

typedef struct pxy_agent_s{
  struct buffer_s *buffer;
  int fd;
  int user_id;
  ssize_t buf_offset;/*all data len in buf*/
  ssize_t buf_parsed;
  ssize_t buf_sent;
}pxy_agent_t;


#define pxy_agent_new(__pool,__fd,__userid,__buffer)	\
  ({							\
    pxy_agent_t *__agent;				\
    __agent = mp_alloc((__pool));			\
    __agent->fd = (__fd);				\
    __agent->user_id = (__userid);			\
    __agent->buffer = (__buffer);			\
    __agent->buf_parsed = 0;				\
    __agent->buf_offset = 0;				\
    __agent->buf_sent = 0;				\
    __agent;						\
  })




#endif
