
#include "hashtable.h"
#include <string.h>
#include <stdlib.h>

void dump_table(ht_table_t *t)
{
  ht_node_t *node,*n;
  list_head_t *head,*iter;
  int i;

  printf("dump table:%d\n\n",(int)t);
  for(i=0;i<t->len;i++){
    node = (ht_node_t*)t->nodes + i;
    head = &(node->list);

    printf("node#%d-%d,key:%d,hash:%d,used:%d,val:%s",
	   i,
	   (int)node,
	   node->key.key,
	   node->key.hash,
	   node->used,
	   (char*)node->data);

    list_for_each(iter,head){
      n = list_entry(iter,ht_node_t,list);
      
      printf("->node#%d-%d,key:%d,hash:%d,used:%d,val:%s",
	     i,
	     (int)n,
	     n->key.key,
	     n->key.hash,
	     n->used,
	     (char*)n->data);
      
    }
    
    printf("\n");
  }
}

int 
main(int len,char** args)
{
  ht_table_t *table = ht_create();
  int i;

  for (i=0; i<13; ++i) {
    char *ch = MALLOC(1024);
    sprintf(ch,"Hello World-%d",i);

    if(!ht_set(table,i,ch,free))
      printf("set failed for:%d\n",i);
  }

  //dump_table(table);

  for(;i<30;i++){
    char *ch = MALLOC(1024);
    sprintf(ch,"Hello World-%d",i);

    if(!ht_set(table,i,ch,free))
      printf("set failed for:%d\n",i);
  }

  dump_table(table);

  for (i=0; i<100; ++i) {
    void* data = ht_get(table,i);

    if(data)
      printf("found,data is : %s\n", (char*)data);
    else
      printf("not found:%d\n",i);
  }

  /*
  for(i=0; i<5; i++){
    ht_remove(table,i,free);
  }

  for (i=0; i<10; ++i) {
    void* data = ht_get(table,i);

    if(data)
      printf("after removed.found,data is : %s\n", (char*)data);
    else
      printf("after removed not found:%d\n",i);
  }
  */
  return 1;
}
