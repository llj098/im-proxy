#include "hashtable.h"

int 
ht_set(ht_table_t* t,uint32_t k,void* v)
{
  if(v == NULL)
    return -1;

  ht_node_t *node;
  int pos;

  node = ht_get(t,k);
  if(node) {
    return -2;
  }

  node = mp_alloc(t->pool);

  if(node) {
    node->key.key = k;
    node->key.hash = ht_hash_func(k);
    node->data = v;
    pos = node->key.hash % t->len;
    
    if(t->nodes[pos]){/*conflict*/
      node->next = t->nodes[pos];
    }

    t->nodes[pos] = node;
    return 1;
  }
  
  return -1;

}


void* 
ht_get(ht_table_t* t,uint32_t k)
{ 
  if(!t)
    return NULL;

  uint32_t hash,pos;
  ht_node_t *node;
  
  hash = ht_hash_func(k);
  pos = hash % t->len;
  node = t->nodes[pos];
  
  while(node){
    if(node->key.key == k)
      return node->data;
    else
      node = node->next;
  }

  return NULL; 
}


void* 
ht_remove(ht_table_t* t,uint32_t k)
{
  if(!t)
    return NULL;
  
  uint32_t hash,pos;
  ht_node_t *node;
  void *d = NULL;

  hash = ht_hash_func(k);
  pos = hash % t->len;
  node = t->nodes[pos];
  
  while(node){
    if(node->key.key == k) {
      d = node->data;
      mp_free(t->pool,node);
    }
    else
      node = node->next;
  }

  return d;
}


int 
ht_resize_tmp(ht_table_t* t)
{
  int len,pos;
  uint32_t hash;
  ht_node_t *node,*n,*nnlast;

  len = t->len;
  len = len << 1;

  if(len<0) {
    return -1;
  }

  
  //ht_node_t* nn = (ht_node_t*)calloc((size_t)len,sizeof(ht_node_t));
  ht_node_t* nn = (ht_node_t*)pxy_calloc(len*sizeof(ht_node_t));

  /*recalcute the hash*/
  for(node=(ht_node_t*)t->nodes;node;node++){
    if(node){

      hash = node->key.hash;
      pos = hash % len;
      
      ht_node_from(node,&(nn[pos]));
      
      while(node->next){
	
	n = node->next;
	nnlast = (ht_node_t*)calloc(1,sizeof(ht_node_t));
	ht_node_from(n,nnlast);

	nnlast = nnlast->next;
	node=node->next;
      }
    }
  }
  return 1;
}
