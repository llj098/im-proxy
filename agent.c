
#include "proxy.h"


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

