/*author liulijin<llj098@gmail.com>*/ 

#include <stdio.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "config.h"


int start()
{
  int s = 1;
  int listen_fd =socket(AF_INET,SOCK_STREAM,0);

	
  if(listen_fd < 0)
    return -1;
	
  struct sockaddr_in addr1;
  addr1.sin_family = AF_INET;
  addr1.sin_port = htons(CLIENT_LISTEN_PORT);
  //addr1.sin_addr.s_addr = 

	
  s =bind(listen_fd, 
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
  printf("%s\n%d","STARTED",s);
}


