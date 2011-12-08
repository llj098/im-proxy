#ifndef _HASHTABLE_H_

#define _HASHTABLE_H_
#define HT_INIT_SIZE 16

#include "proxy.h"


typedef struct ht_key_s{
  uint32_t hash;
  u_char* raw_key;
  size_t len;
}ht_key_t;

typedef struct ht_node_s{
  void* data;
  ht_key_t key;
  struct ht_node_s* next;
}ht_node_t;

typedef struct ht_table_s{ 
  int len;
  ht_node_t* nodes;
}ht_table_t;

int ht_resize(ht_table_t* t);
int ht_key_compare(ht_key_t k1,ht_key_t k2);
int ht_set(ht_table_t* t,ht_key_t k,void* data);
void* ht_get(ht_table_t* t,ht_key_t k);
void ht_remove(ht_table_t* t,ht_key_t k);


inline ht_node_cp(ht_node_t *src,ht_node_t *dst)
{
  dst->data = src->data;

  dst->key.hash = src->key.hash;
  dst->key.raw_key = src->key.raw_key;
  dst->key.len = src->key.len;
}

inline ht_key_t ht_key_init(u_char* key,size_t len)
{
  ht_key_t k;
  k.raw_key = key;
  k.len = len;

  return k;
}

inline int ht_init(ht_table_t* t)
{
  if(t == NULL)
	return -1;

  t->len = HT_INIT_SIZE;
  t->nodes = (ht_node_t*)calloc(sizeof(struct ht_node_s) 
							 * HT_INIT_SIZE);
  if(!t->nodes)
	return -1;

  return 0;
}


/* Copyright (C) Austin Appleby */
uint32_t
ht_murmur_hash2(u_char *data, size_t len)
{
  uint32_t  h, k;

  h = 0 ^ len;

  while (len >= 4) {
    k  = data[0];
    k |= data[1] << 8;
    k |= data[2] << 16;
    k |= data[3] << 24;

    k *= 0x5bd1e995;
    k ^= k >> 24;
    k *= 0x5bd1e995;

    h *= 0x5bd1e995;
    h ^= k;

    data += 4;
    len -= 4;
  }

  switch (len) {
  case 3:
    h ^= data[2] << 16;
  case 2:
    h ^= data[1] << 8;
  case 1:
    h ^= data[0];
    h *= 0x5bd1e995;
  }

  h ^= h >> 13;
  h *= 0x5bd1e995;
  h ^= h >> 15;

  return h;
}

#endif 
