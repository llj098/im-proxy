#include "sysinc.h"

#define EV_WRITABLE 1
#define EV_READABLE 2
#define EV_ALL 3

#define EV_TIME 1
#define EV_FILE 2

#define EV_CTL_ADD 1
#define EV_CTL_DEL 2

#define EV_COUNT 10000

struct ev_s;

typedef void ev_time_func(struct ev_s* ev,void* data);
typedef void ev_file_func(struct ev_s* ev,int fd,int mask);

typedef struct ev_time_item_s{
  int id;
  void* data;
  struct ev_time_item_s* next;
  ev_time_func* func;
}ev_time_item_t;

typedef struct ev_file_item_s{
  int fd;
  int mask;
  void* data;
  ev_file_func* wfunc;
  ev_file_func* rfunc;
}ev_file_item_t;

typedef struct ev_fired_s{
  int fd;
  int mask;
}ev_fired_t;

typedef struct ev_s{
  int fd;
  void *data;
  ev_time_item_t* ti;
}ev_t;

#define ev_get_time(__s,__m)			\
  ({						\
    struct timeval __tv;			\
    gettimeofday(&__tv,NULL);			\
    *__s = __tv.tv_sec;				\
    *__m = __tv.tv_usec/1000;			\
  })						\

#define ev_add_ms_to_now(__ms,__s,__m)		\
  ({						\
    long __cur_s,__cur_m,__when_s,__when_m;	\
    ev_get_time(&__cur_s,&__cur_m);		\
    __when_s = __cur_s + __ms/1000;		\
    __when_m =__cur_m + __ms%1000;		\
    if(__when_m > 1000){			\
      __when_s ++;				\
      __when_m-=100;				\
    }						\
    *__s = __when_s;				\
    *__m = __when_m;				\
  })						


#define ev_file_item_new(__fd,__d,__rf,__wf)			\
  ({								\
    ev_file_item_t* __fi;					\
    __fi = (ev_file_item_t*)malloc(sizeof(ev_file_item_t*));	\
    __fi->fd = __fd;						\
    __fi->wfunc = __wf;						\
    __fi->rfunc = __rf;						\
    __fi->data = __d;						\
    __fi;							\
  })								\


ev_t* ev_create();

int ev_file_item_ctl(ev_t* ev,int op,ev_file_item_t* item);
int ev_time_item_ctl(ev_t* ev,int op,ev_time_item_t* item);

void ev_main(ev_t* ev);
