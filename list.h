#ifndef __LIST_H__
#define __LIST_H__

/*a link list like linux kernel's*/

typedef struct list_head_s{
    struct list_head_s *prev;
    struct list_head_s *next;
}list_head_t;


#define LIST_HEAD(x)				\
    struct list_head_s x = { &x,&x } 

#define LIST_HEAD_INIT(x) { &(x),&(x) } 
  

#define INIT_LIST_HEAD(x)			\
    ({						\
	(x)->next = (x); (x)->prev = (x);	\
    })

/*go back the 'offset' length ,
 *return the address of one entry(container)*/
#define list_entry(ptr,type,member)					\
    (type*)((char*)(ptr) - (unsigned long)&(((type*)0)->member))	\

#define list_for_each(iter,head)					\
    for( (iter)=(head)->next ; iter != (head) ; iter = iter->next)	\

#define list_for_each_entry(iter,member,head)				\
    for( iter = list_entry((head)->next,typeof(*iter),member) ;		\
	 &iter->member != (head) ;					\
	 iter = list_entry(iter->member.next,typeof(*iter),member))	\


static inline void __list_add(list_head_t *new,
			      list_head_t *prev,
			      list_head_t *next)
{
    new->next = next;
    new->prev = prev;
    next->prev = new;
    prev->next = new;
}

static inline void __list_del(list_head_t *prev,list_head_t *next)
{
    prev->next = next;
    next->prev = prev;
}

static inline void list_del(list_head_t *head)
{
    __list_del(head->prev,head->next);
    head->prev = (void*)0;
    head->next = (void*)0;
}

static inline void 
list_combine(list_head_t *head,list_head_t *new,list_head_t *tail)
{
    tail->next = head;
    new->prev = head;
    head->prev = tail;
    head->next = new;
}

static inline int list_empty(list_head_t *head)
{
    if(head->next == head)
	return 1;
  
    return 0;
}

#define list_append(entry,head)			\
    __list_add((entry), (head)->prev, (head))		

#define list_insert(entry,head)			\
    __list_add((entry), (head), (head)->next)


#define list_remove(entry)			\
    if((entry)->next == (entry)->prev){		\
	(entry)->prev = NULL;			\
	(entry)->next = NULL;			\
    }						\
    else{					\
	(entry)->prev->next= (entry)->next;	\
	(entry)->next->prev = (entry)->prev;	\
    }						\
  
  
  


#endif
