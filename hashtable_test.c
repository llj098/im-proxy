
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
      printf("found,data is : %s,t->used:%d\n", (char*)data,table->used);
    else
      printf("not found\n");
  }

  for(i=0; i<90; i++){
    ht_remove(table,i,free);
  }

  for (i=0; i<100; ++i) {
    void* data = ht_get(table,i);

    if(data)
      printf("after removed.found,data is : %s\n", (char*)data);
    else
      printf("after removed not found\n");
  }

  return 1;
}
