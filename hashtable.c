#include "proxy.h"

int 
ht_set(ht_table_t* t,uint32_t k,void* v,ht_clean clean)
{
  if(v == NULL)
    return -1;

  ht_node_t *node,*n;
  uint32_t pos,hash;

  node = ht_get(t,k);
  if(node) {

    if(node->data)
      clean(node->data);

    node->data = v;
    return 1;
  }

  hash = ht_hash_func(k);
  pos = hash % t->len;
  n = (ht_node_t*)(t->nodes)+ pos;
  
  if(n->used) { /*conflict*/

    t->alloced++;
    node = MALLOC(sizeof(*node));

    if(node){
      ht_node_init(node,hash,k,v);
      list_append(&(node->list),&(n->list));
      return 1;
    }

    return -1;
  }
  else{
    ht_node_init(n,hash,k,v);
    n->used = 1;
  }
 
  return 1;
}


void* 
ht_get(ht_table_t* t,uint32_t k)
{ 
  if(!t)
    return NULL;

  uint32_t hash,pos;
  ht_node_t *node,*n;
  list_head_t *iter;
  
  hash = ht_hash_func(k);
  pos = hash % t->len;
  node = (ht_node_t*)(t->nodes)+pos;
  
  
  list_for_each(iter,&(node->list)){

    if(k == node->key.key){
      return node->data;
    }
    
    n = list_entry(iter,ht_node_t,list);
    if(n && n->key.key == k){
      return n->data;
    }
  }

  return NULL; 
}


void 
ht_remove(ht_table_t* t,uint32_t k,ht_clean clean)
{
  if(!t)
    return;
  
  uint32_t hash,pos;
  ht_node_t *node,*n;
  void *d = NULL;
  list_head_t *iter;

  hash = ht_hash_func(k);
  pos = hash % t->len;
  node = (ht_node_t*)(t->nodes+pos);
  
  list_for_each(iter,&(node->list)){

    if(k == node->key.key){
      node->used = 0;
      d = node->data;
      break;
    }

    if(iter){
      n = list_entry(iter,ht_node_t,list);

      if(n && n->key.key ==k){
	d = n->data;
	FREE(n);
	break;
      }
    }

  }

  if(d){
    if(clean){ clean(d); }
    else{ FREE(d); }
  }
	
  return;
}


int 
ht_resize(ht_table_t* t)
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
