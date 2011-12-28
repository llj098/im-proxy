#include "proxy.h"


int 
_ht_set(ht_table_t *t,uint32_t k,void *v)
{
  uint32_t pos,hash;
  ht_node_t *node,*n;

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


int 
ht_set(ht_table_t* t,uint32_t k,void* v,ht_clean clean)
{
  if(v == NULL || k <=0)
    return -1;

  ht_node_t *node;

  node = ht_get(t,k);
  if(node) {
    if(node->data)
      clean(node->data);

    node->data = v;
    return 1;
  }


  if(t->used > t->len*0.7){
    printf("resized\n");
    if(!ht_resize(t)){
      return -1;
    }
  }

  return _ht_set(t,k,v);

}


void* 
ht_get(ht_table_t* t,uint32_t k)
{ 
  if(!t || k <=0)
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

  if(k==9){
    printf("!!!!Here\n");

    printf("t->len:%d,hash:%d,pos:%d,k:%d,node:%d,node.p:%d,node.n:%d\n",
	   t->len,
	   hash,
	   pos,
	   k,
	   (int)node,
	   (int)node->list.prev,
	   (int)node->list.next);

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
  int oldlen,len,i;
  ht_node_t *node,**nn,**oldn;
  list_head_t *iter;

  len = t->len << 1;

  if(len<0) {
    return -1;
  }

  oldlen = t->len;
  oldn   = t->nodes;

  nn = pxy_calloc(len*sizeof(ht_node_t));
  ht_renew(t,nn,len,0);
  
  for(i=0; i<len; i++) {
    node = (ht_node_t*)nn + i;
    ht_node_init(node,0,0,NULL);
  }

  /*recalcute the hash*/
  for(i=0; i<oldlen; i++) {

    node = (ht_node_t*)oldn + i;
    if(node->key.key > 0)
      _ht_set(t,node->key.key,node->data);

    list_for_each(iter,&(node->list)){
      ht_node_t *n = list_entry(iter,ht_node_t,list);
      _ht_set(t,n->key.key,n->data);
      FREE(n);
    }
  }

  FREE(oldn);
  return 1;
}

