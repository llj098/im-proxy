
/*only for epoll now*/

#include "ev.h"
#include "sysinc.h"

ev_t*
ev_create(void* data)
{
  ev_t* ev;
  int fd = epoll_create(1/*kernel hint*/);

  if(fd > 0){
    ev = (ev_t*)malloc(sizeof(ev_t));
    ev->fd = fd;
    ev->data = data;
    ev->next_time_id = 0;
    ev->ti = NULL;
    ev->api_data=malloc(sizeof(struct epoll_event)*EV_COUNT);

    return ev;
  }

  return NULL;
}

int 
ev_file_item_ctl(ev_t* ev,int op,ev_file_item_t* item)
{
  struct epoll_event epev;

  epev.events = item->events;
  epev.data.fd = item->fd;
  epev.data.ptr = item;

  return epoll_ctl(ev->fd, op, item->fd, &epev);
}

int 
ev_time_item_ctl(ev_t* ev,int op,ev_time_item_t* item)
{
  if(op == EV_CTL_ADD){

    item->id = ev->next_time_id++;
    item->next = ev->ti;
    ev->ti = item;
    
    return 1;
  }

  return -1;
}

void
ev_main(ev_t* ev)
{
  while(!ev->stop){

    if(ev->ti){

      long now;
      ev_time_item_t* iter;

      ev_get_current_ms((&now));
      
      for(iter=(ev->ti); iter!=NULL; iter=iter->next){
	if((iter->ms) < now){
	  iter->func(ev,iter);
	}
      }

    }

    int i,j;
    struct epoll_event* e = ev->api_data;
     
    j = epoll_wait(ev->fd,e,EV_COUNT,100);

    for(i=0;i<j;i++){
      ev_file_item_t* fi = (ev_file_item_t*)e[j].data.ptr;

      if((e[j].events) | EPOLLIN)
	fi->rfunc(ev,fi);
    
      if(e[j].events | EPOLLOUT)
	fi->wfunc(ev,fi);
    }
  }
}
