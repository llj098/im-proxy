#include "proxy.h"


typedef struct connection_buffer_s{
  void* data;
  struct connection_buffer_s* next;
}connection_buffer_t;

typedef struct connection_s{
  int userid;
  uint32_t msg_count;
  connection_buffer_t* buffer;
}connection_t;


connection_t*
connection_new()
{
  connection_t* c = malloc(sizeof(*c));
  return c;
}
