/*
 * queue.h - Bidirectional Queue: Refer to linux kernel queue definitions.
 *
 * Usage example：
 * struct data_s {
 * 	  vbar_queue_t queue;
 * 	  int data;
 * };
 *
 * int vbar_queue_usage_example()
 * {
 * 	  struct data_s *tmp = NULL;
 *
 * 	  //初始化队列
 * 	  vbar_queue_t head;
 * 	  vbar_queue_t *h = &head;
 * 	  vbar_queue_init(h);
 *
 * 	  //插入数据1
 * 	  tmp = (struct data_s *)calloc(sizeof(struct data_s), 1);
 * 	  if (tmp) {
 * 	  	 tmp->data = 1;
 * 	  	 vbar_queue_insert_tail(h, &(tmp->queue));
 * 	  }
 *
 * 	  //插入数据2
 * 	  tmp = (struct data_s *)calloc(sizeof(struct data_s), 1);
 * 	  if (tmp) {
 * 	  	 tmp->data = 2;
 * 	  	 vbar_queue_insert_tail(h, &(tmp->queue));
 * 	  }
 *
 * 	  //插入数据3
 * 	  tmp = (struct data_s *)calloc(sizeof(struct data_s), 1);
 * 	  if (tmp) {
 * 	  	 tmp->data = 3;
 * 	  	 vbar_queue_insert_tail(h, &(tmp->queue));
 * 	  }
 * 	  tmp = NULL;
 *
 *
 * 	  //遍历数据
 * 	  vbar_queue_t *pos, *n;
 * 	  vbar_queue_foreach(pos, n, h) {
 *       tmp = vbar_queue_data(pos, struct data_s, queue);
 * 	     fprintf(stderr, "data: %d\n", tmp->data);
 *    }
 *
 * 	  //释放队列
 * 	  vbar_queue_foreach(pos, n, h) {
 * 	  	 tmp = vbar_queue_data(pos, struct data_s, queue);
 * 	  	 free(tmp);
 * 	  }
 * 	  ee_queue_init(h);
 * 	  return 0;
 * }
 */
#ifndef __VBAR_QUEUE__
#define __VBAR_QUEUE__

#ifndef vbar_offsetof
#define vbar_offsetof(TYPE, MEMBER) ((size_t)&((TYPE *)0)->MEMBER)
#endif

typedef struct vbar_queue_s vbar_queue_t;
struct vbar_queue_s {
	vbar_queue_t *prev;
	vbar_queue_t *next;
};

/* Initialization queue */
#define vbar_queue_init(h)  \
	do {				    \
        if ((h) != NULL) {          \
    		(h)->prev = (h);\
    		(h)->next = (h);\
        }                   \
	} while (0)

#define vbar_queue_valid(h) ((h) && ((h)->next) && ((h)->prev))

/* Determine if the queue is empty */
#define vbar_queue_empty(h) (!vbar_queue_valid((h)) || (h) == (h)->prev)

/* Insert node from the front */
#define vbar_queue_insert_head(h, x)        \
	do {							        \
        if (vbar_queue_valid(h) && (x)) {   \
    		(x)->next = (h)->next;		    \
    		(x)->next->prev = (x);		    \
    		(x)->prev = (h);			    \
    		(h)->next = (x);			    \
        }                                   \
	} while (0)

#define vbar_queue_insert_after(h, x) vbar_queue_insert_head(h, x)

/* Insert node from the end */
#define vbar_queue_insert_tail(h, x)        \
	do {							        \
        if (vbar_queue_valid(h) && (x)) {   \
    		(x)->prev = (h)->prev;		    \
    		(x)->prev->next = x;		    \
    		(x)->next = h;				    \
    		(h)->prev = x;				    \
        }                                   \
	} while (0)

/* Get the head node pointed to by the head pointer */
#define vbar_queue_head(h) ((h)?((h)->next):NULL)

/* Get the last node */
#define vbar_queue_last(h) ((h)?((h)->prev):NULL)

/* Sentinel node */
#define vbar_queue_sentinel(h) (h)

/* Next node */
#define vbar_queue_next(q) vbar_queue_head(q)

/* Previous node */
#define vbar_queue_prev(q) vbar_queue_last(q)

/* Remove a node */
#define vbar_queue_remove(x)		    \
	do {							    \
        if (!vbar_queue_empty(x)) {      \
    		(x)->next->prev = (x)->prev;\
    		(x)->prev->next = (x)->next;\
    		(x)->prev = NULL;			\
    		(x)->next = NULL;			\
        }                               \
	} while (0)


/* Split a queue
 *	h: Head pointer
 *	q: Head pointer of the queue that needs to be split
 *	n: Header of another queue after the split is complete
 */
#define vbar_queue_split(h, q, n)       \
	do {						        \
        if (vbar_queue_valid(h) &&      \
                vbar_queue_valid(q) && \
                (n)) {                  \
    		(n)->prev = (h)->prev;	    \
    		(n)->prev->next = n;	    \
    		(n)->next = q;			    \
    		(h)->prev = (q)->prev;	    \
    		(h)->prev->next = h;	    \
    		(q)->prev = n;			    \
        }                               \
	} while (0)

/* Merge two queues into one queue */
#define vbar_queue_add(h, n)		    \
	do {							    \
        if (vbar_queue_valid(h) &&      \
                vbar_queue_valid(n)) {  \
    		(h)->prev->next = (n)->next;\
    		(n)->next->prev = (h)->prev;\
    		(h)->prev = (n)->prev;		\
    		(h)->prev->next = (h);		\
        }                               \
	} while (0)

/* According to the queue pointer, get the structure containing this queue
 * pointer
 *	q:queue pointer
 *	type: Returned data type
 *	link: The corresponding queue item name in the data item
 */
#define vbar_queue_data(q, type, link) ((q)?((type *)((char *)(q) - vbar_offsetof(type, link))):NULL)

#define vbar_queue_foreach(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

#define vbar_queue_foreach_safe(pos, n, head) \
	for ((pos) = ((head)?(head)->next:(head)), (n) = ((pos)?(pos)->next:(head)); \
                    (pos) != (head); (pos) = (n), (n) = ((pos)?(pos)->next:NULL))

#endif //__VBAR_QUEUE__


