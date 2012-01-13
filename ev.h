#ifndef _EV_H_
#define _EV_H_

#define EV_READABLE 1
#define EV_WRITABLE 2
#define EV_ALL 3

#define EV_TIME 1
#define EV_FILE 2

/*same to epoll now*/
#define EV_CTL_ADD 1
#define EV_CTL_DEL 2
#define EV_CTL_MOD 3

#define EV_COUNT 10000

struct ev_s;
struct ev_file_item_s;

typedef void ev_time_func(struct ev_s* ev,void* data);
typedef void ev_file_func(struct ev_s* ev,struct ev_file_item_s* fi);

typedef struct ev_time_item_s{
  int id;
  long ms; /*for the time event we only handle msec*/
  void* data;
  struct ev_time_item_s* next;
  ev_time_func* func;
}ev_time_item_t;

typedef struct ev_file_item_s{
  int fd;
  int mask;
  int events;
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
  int next_time_id;
  ev_time_item_t* ti;
  void* api_data;
  int stop;
}ev_t;

#define ev_get_current_ms(__m)			\
  ({						\
    struct timeval __tv;			\
    gettimeofday(&__tv,NULL);			\
    *__m = __tv.tv_usec/1000;			\
  })						\

#define ev_file_item_new(__fd,__d,__rf,__wf,__e)		\
  ({								\
    ev_file_item_t* __fi;					\
    __fi = (ev_file_item_t*)malloc(sizeof(ev_file_item_t));	\
    __fi->fd = __fd;						\
    __fi->wfunc = __wf;						\
    __fi->rfunc = __rf;						\
    __fi->data = __d;						\
    __fi->events = __e;						\
    __fi;							\
  })								\

#define ev_time_item_new(__ev,__ms,__func,__data)	\
  ({							\
    ev_time_item_t* __ti;				\
    __ti->id=__ev->next_time_id;			\
    __ti->ms = __ms;					\
    __ti->func = __func;				\
    __ti->data = __data;				\
    __ti;						\
  })

ev_t* ev_create();
int ev_file_item_ctl(ev_t* ev,int op,ev_file_item_t* item);
int ev_time_item_ctl(ev_t* ev,int op,ev_time_item_t* item);
void ev_main(ev_t* ev);

#endif
