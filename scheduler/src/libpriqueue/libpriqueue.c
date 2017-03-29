/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
	/*initialize size to 0*/
	q->size = 0;
	
	/*allocate array to hold our heap*/
	q->heap = malloc(sizeof(void *) * 100);
	
	/*initialize using comparator passed in*/
	q->comparator = comparer;
}

/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
	/*add new element to back*/
	q->heap[q->size] = ptr;

	/*increase size*/
	q->size++;

	/*qsort the array*/
	qsort(q->heap, q->size, sizeof(void*), q->comparator);
	
	for (int i = 0; i < q->size; i++)
	{
		if (q->heap[i] == ptr)
		{
			return i;
		}
	}
	
	/*shouldn't happen*/
	return -1;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
	if (q->size == 0)
	{
		return NULL;
	}
	else
	{
		return q->heap[0];
	}
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
	if (q->size == 0)
	{
		return NULL;
	}
	else
	{
		return priqueue_remove_at(q, 0);
	}
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
	if (q->size == 0 || index >= q->size)
	{
		return NULL;
	}
	else
	{
		return q->heap[index];
	}
}


/**
  Removes all instances of ptr from the queue. 
  
  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
	int count = 0;
	
	/*probe all elements of array*/
	for (int i = 0; i < q->size; i++)
	{
		/*we found a value so we must delete*/
		if (ptr == q->heap[i])
		{
			/*replace with last value in array*/
			q->heap[i] = q->heap[q->size-1];
			
			/*decrement size*/
			q->size--;
			
			/*decrement i so we check the again value as there
			is a small chance this value was replaced with value*/
			i--;
			
			count++;
		}
	}
	
	/*qsort again to make sure order is upheld*/
	qsort(q->heap, q->size, sizeof(void*), q->comparator);
	
	return count;
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
	/*temp holds value at index*/
	void* temp = q->heap[index];
	
	/* replace this index with last index*/
    q->heap[index] = q->heap[q->size-1];
 
    /*Decrease heap size by 1*/
    q->size--;
 
    /*qsort to restore order*/
	qsort(q->heap, q->size, sizeof(void*), q->comparator);
	
	/*return value at index*/
	return temp;
}


/**
  Returns the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->size;
}


/**
  Destroys and frees all the memory associated with q->
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
	/*free all data in queue*/
	free(q->heap);
}
