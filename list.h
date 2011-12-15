#ifndef __LIST_H__
#define __LIST_H__

typedef struct list_head_s{
  struct list_head_s *prev;
  struct list_head_s *next;
}list_head_t;


#define LIST_HEAD(x)				\
  struct list_head_s x = { &x,&x } 

#define LIST_HEAD_INIT(x)				\
  (x) = { &(x),&(x) } 
  

#define INIT_LIST_HEAD(x)			\
  ({						\
    (x)->next = (x); (x)->prev = (x);		\
  })

/*go back the 'offset' length ,
 *return the address of one entry(container)*/
#define list_entry(ptr,type,member)					\
  ( (type*)((char*)(ptr) - (unsign long)&((type*)0)->(member)) )	\


#define list_for_each(iter,head)				\
  for( (iter)=(head)->next ; iter != (head); iter = iter->next)	\

static inline void __list_add(list_head_t *new,
			      list_head_t *prev,
			      list_head_t *next)
{
  new->next = next;
  new->prev = prev;
  next->prev = new;
  prev->next = new;
}


#define list_add_append(entry,head)		\
  __list_add(entry, head->prev, head)		

#define list_insert(entry,head)			\
  __list_add(entry, head, head->next)




#endif
