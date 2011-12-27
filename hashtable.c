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


  if(t->used > t->len*0.7){
    if(!ht_resize(t)){
      return -1;
    }
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
    t->used ++;
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
  
  if(k == node->key.key && node->used){
    return node->data;
  } 

  
  list_for_each(iter,&(node->list)){

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
  node = (ht_node_t*)(t->nodes) + pos;
  
  list_for_each(iter,&(node->list)){

    if(k == node->key.key){
      t->used--;

      d = node->data;
      node->used = 0;
      //ht_node_init(node,0,0,NULL);
      break;
    }

    if(iter){
      n = list_entry(iter,ht_node_t,list);

      if(n && n->key.key ==k){
	d = n->data;
	t->alloced--;
	list_del(iter);
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
  int len,pos,i;
  uint32_t hash;
  ht_node_t *node,*n,**nn;

  len = t->len << 1;

  if(len<0) {
    return -1;
  }

  nn = (ht_node_t**)pxy_calloc(len*sizeof(ht_node_t));

  for(i=0; i<len; i++) {
    node = (ht_node_t*)nn + i;
    ht_node_init(node,0,0,NULL);
  }

  /*recalcute the hash*/
  for(i=0; i< t->len; i++) {

    node = (ht_node_t*)t->nodes + i;
    hash = node->key.hash;
    pos = hash % len;

    n = (ht_node_t*)nn + pos;

    n->key.key = node->key.key;
    n->key.hash = hash;
    n->data = node->data;

    if(!list_empty(&(node->list))) { 
      list_combine(&n->list,node->list.next);
    }
  }

  FREE(t->nodes);
  t->nodes = nn;
  t->len = len;

  for(i=0; i< t->len; i++) {
    node = (ht_node_t*)(t->nodes) + i;
  }
  return 1;
}
