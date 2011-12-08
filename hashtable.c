#include "hashtable.h"
#include "proxy.h"

//int ht_set(ht_table_t* t,ht_key_t* key,void* val) { return -1; }
//void* ht_get(ht_table_t* t,ht_key_t* key) { return NULL; }
//void ht_remove(ht_table_t* t,ht_key_t* key) { }

int 
ht_resize_tmp(ht_table_t* t)
{
  int len,sz,pos,nnlast;
  uint32_t hash;
  ht_node_t* node,n;

  len = t->len;
  len = len << 1;

  if(len<0) {
    L("Error, Max Length");
    return -1;
  }

  sz = sizeof(ht_node_t)*len;
  ht_node_t* nn = (ht_node_t*)calloc(sz);

  /*recalcute the hash*/
  for(node=t->nodes;t;t++){
    if((node != NULL) && (node->key != NULL)){

      hash = node->key->hash;
      pos = hash % len;
      
      ht_node_cp(node,nn[pos]);
      
      while(node->next){
	
	n = node->next;
	nnlast = (ht_node_t*)calloc(sizeof(ht_node_t));
	ht_node_cp(n,nnlast);

	nnlast = nnlast->next;
	node=node->next;
      }
    }
  }
}
