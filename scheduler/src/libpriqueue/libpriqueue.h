/** @file libpriqueue.h
 */

#ifndef LIBPRIQUEUE_H_
#define LIBPRIQUEUE_H_

/**
  Priqueue Data Structure
*/
typedef struct _priqueue_t
{
	/*number of elements stored in queue*/
	int size;
	
	/*array to hold elements*/
	void** heap;
	
	/*comparator to be used on insert*/
	int (*comparator)(const void *p, const void *q);
	
} priqueue_t;


void   priqueue_init     (priqueue_t *q, int(*comparer)(const void *, const void *));

int    priqueue_offer    (priqueue_t *q, void *ptr);
void * priqueue_peek     (priqueue_t *q);
void * priqueue_poll     (priqueue_t *q);
void * priqueue_at       (priqueue_t *q, int index);
int    priqueue_remove   (priqueue_t *q, void *ptr);
void * priqueue_remove_at(priqueue_t *q, int index);
int    priqueue_size     (priqueue_t *q);

void   priqueue_destroy  (priqueue_t *q);

void swap(priqueue_t *q, int index1, int index2);
int bubbleUp(priqueue_t *q, int index);
void trickleDown(priqueue_t *q, int index);

#endif /* LIBPQUEUE_H_ */
