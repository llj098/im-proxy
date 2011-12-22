
#include "hashtable.h"


int 
main(int len,char** args)
{
  ht_table_t *table = ht_create();

  char *content = "HELLO WORLD";
  
  ht_set(table,1,content);
  
  void* data = ht_get(table,1);
  
  if(data)
    printf("found,data is : %s", (char*)data);
  else
    printf("not found");

  return 1;
}
