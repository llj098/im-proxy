
#include "hashtable.h"


int 
main(int len,char** args)
{
  ht_table_t *table = ht_create();

  char *content = "HELLO WORLD";
  
  
  ht_set(table,1,content);

  
  printf("%s\n",(char*)ht_get(table,1));

  return 1;
}
