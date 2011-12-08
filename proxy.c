/*
 * Copyright liulijin<llj098@gmail.com>
 */ 

#include <stdio.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "config.h"

#define MAX_EVENTS 1000

int 
start_worker()
{
  struct epoll_event ev,events[MAX_EVENTS];
  int i,lsock,csock,epollfd,nfds;

  epollfd = epoll_creat(10);
  if(epollfd < 0)
    return -1;

  ev.events = EPOLLIN;
  ev.data.fd = lsock;

  if(epoll_ctl(epollfd,EPOLL_CTL_ADD,lsock,&ev) < 0){
    return -1;
  }

  while(true){

    nfds = epoll_wait(epollfd,events,MAX_EVENTS,-1);

    if(nfds < 0)
     return -1;

    for(i=0;i<nfds;i++){
      if(events[n].data.fd == lsock){

	//do accept work
	csock = accept(lsock);
	
	if(csock<0){
	  return -1;
	}

	setnonblocking(csock);
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = csock;

	if(epoll_ctl(epollfd,EPOLL_CTL_ADD,csock,
		     &ev) < 0){
	  return -1;
	}
      }
      else{
	 //do receive
	
      }
    }
  }
  

  //try to obtain the lock:
  //todo...

  


}


int 
spawn_worker()
{
  int i = 0;

  for(;i<4;i++){

    int p = fork();

    if(p < 0) {
	printf("%s","forkerror");
    }
    else if(p == 0){/*child*/
      printf("%d\n",getpid());
    }
    else{/*parent*/
      printf("%d\n",getpid());
    }
  }

  return 1;

}

int 
create_shm()
{
  int p = shmget(IPC_PRIVATE,sizeof(4),0600/*user read&writer*/);
  void* ptr;
  if(p>0) {
    printf("shm created, id %d\n",p);
    ptr = shmat(p,0,0);
    ptr = "123";
    printf("shmat %lx,%s",ptr,ptr);
  }
  
  return p;
}

int start()
{
  return create_shm();
  return spawn_worker();


  int s = 1;
  int listen_fd =socket(AF_INET,SOCK_STREAM,0);

	
  if(listen_fd < 0)
    return -1;
	
  struct sockaddr_in addr1;
  addr1.sin_family = AF_INET;
  addr1.sin_port = htons(CLIENT_LISTEN_PORT);
  //addr1.sin_addr.s_addr = 

	
  s = bind(listen_fd, 
	   (struct sockaddr*)&addr1,
	   sizeof(addr1)
	   );


  if(s < 0)
    return -1;

  listen(listen_fd,5);


	
  return s;
}

int 
main(char* args,int len)
{
  int s = start();
  char st[2];
  //printf("%s\n%d","STARTED",s);
  scanf("%s",&st);
}


