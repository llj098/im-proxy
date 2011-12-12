
/*only for epoll now*/

#include "proxy.h"


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

int 
ev_main(ev_t* ev)
{

  while(!ev->stop){

    if(ev->ti){

      long curms;
      struct ev_time_item_t *t,*prevti,*found,*iter;

      ev_get_current_ms((&curms));
      iter = prevti = t = ev->ti;

      do{
    
	t = iter;
	iter = iter->next;

	if(t->ms < curms){

	  //remove the event from event
	  if(prevti != t){
	    prevti->next = t->next;
	  }
	  else{
	    ev->ti = t->next;
	  }

	  /*add to found list*/
	  t->next = found;
	  found = t;
	}

	prevti = prevti->next;

      }while(iter);

      /*fire the events*/
      while(found){
	found->func(found->id,found->data);
	found = found->next;
      }
    }

    int i,j;
    j = epoll_wait(ev->fd,ev->events,EV_COUNT,100);

    for(i=0;i<j;i++){
      ev_file_item_t* fi = (ev_file_item_t*)ev->events[j].data.ptr;

      if(ev->events[j].events | EPOLLIN)
	fi->rfunc(ev,fi);
    
      if(ev->events[j].events | EPOLLOUT)
	fi->wfunc(ev,fi);
    }
  }
}




