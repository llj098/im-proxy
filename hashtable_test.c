
#include "hashtable.h"
#include <string.h>
#include <stdlib.h>


int 
main(int len,char** args)
{
  ht_table_t *table = ht_create();
  int i;

  for (i=0; i<100; ++i) {
    char *ch = MALLOC(1024);
    sprintf(ch,"Hello World-%d",i);
    ht_set(table,i,ch,free);
  }

  for (i=0; i<100; ++i) {
    
    void* data = ht_get(table,i);

    if(data)
      printf("found,data is : %s\n", (char*)data);
    else
      printf("not found\n");

  }
  return 1;
}
