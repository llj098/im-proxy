/*
 * Copyrtight liulijin<llj098@gmail.com>
 */ 

#include <stdio.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "config.h"


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


