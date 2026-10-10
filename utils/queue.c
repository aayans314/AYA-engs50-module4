/* 
 * queue.c --- 
 * 
 * Author: Andrea Soto
 * Created: 10-06-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "queue.h"

/* private definition for element list */
typedef struct node {
	struct node *next;
	void *element;
} node_t;

/*private definition of queue pointers */
typedef struct queue {
	node_t *front;
	node_t *back;
} queue_t_priv;

/* create an empty queue */
queue_t* qopen(void) {
	queue_t_priv *qp;

	qp = (queue_t_priv*)malloc(sizeof(queue_t_priv));

	if (qp == NULL){
		return NULL;
	}

	qp->front = NULL;
	qp->back = NULL;

	return ((queue_t*)qp);
}

/* deallocate a queue, frees everything in it */
void qclose(queue_t *qp) {
	queue_t_priv *qp_priv;
	node_t *currp;
	node_t *nextp;

	if (qp == NULL){
		return;
	}
	
	qp_priv = (queue_t_priv*)qp;
	currp = qp_priv->front;

	while (currp != NULL) {
		nextp = currp->next;
		free(currp);
		currp = nextp;
	}

	free(qp_priv);
	
}

/* put element at the end of the queue
 * returns 0 is successful; nonzero otherwise 
 */
int32_t qput(queue_t *qp, void *elementp){
	int32_t result = 1;
	queue_t_priv *qp_priv;
	node_t *node_p, *curr_back;

	if (qp != NULL && elementp != NULL){

		qp_priv = (queue_t_priv*)qp;
		node_p = (node_t*)malloc(sizeof(node_t));

		if (node_p == NULL){
			return result;
		}
		
		node_p->element = (void*)elementp;
		node_p->next = NULL;
		
		if (qp_priv->back == NULL){
			qp_priv->front = node_p;
			qp_priv->back = node_p;

			result = 0;
		} else if (qp_priv->back != NULL){
			curr_back = qp_priv->back;
			curr_back->next = node_p;
			qp_priv->back = curr_back->next;
		
			result = 0;
		}
		
	}
		
	return result;
}

/* get the first first element from queue, removing it from the queue */
void* qget(queue_t *qp){
	queue_t_priv *qp_priv;
	node_t *curr_front;
	void *elementp = NULL;
	

	if (qp != NULL){

		qp_priv =(queue_t_priv*)qp;

		if (qp_priv->front != NULL){
			curr_front = qp_priv->front;
			elementp = curr_front->element;
			qp_priv->front = curr_front->next;

			free(curr_front);

			if (qp_priv->front == NULL){
				qp_priv->back = NULL;
			}
		}
		
	}
	
	return elementp;
	
}

/* apply a function to every element of the queue */
void qapply(queue_t *qp, void (*fn)(void* elementp)){
	queue_t_priv *qp_priv;
	node_t *curr_p;

	
	if (qp != NULL && fn != NULL){
		qp_priv = (queue_t_priv*)qp;
	
		for (curr_p = qp_priv->front; curr_p != NULL; curr_p = curr_p->next){
			fn(curr_p->element);
		}
	}
}

/* search a queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- elementp - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be 
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
 */
void* qsearch(queue_t *qp, 
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp){
	queue_t_priv *qp_priv;
	node_t *curr_p;
	
	if (qp != NULL && searchfn != NULL){
		qp_priv = (queue_t_priv*)qp;
		
		for (curr_p = qp_priv->front; curr_p != NULL; curr_p = curr_p->next){
			if (searchfn(curr_p->element,skeyp)){
				return (curr_p->element);
			}
		}

	}

	return NULL;

}

/* search a queue using a supplied boolean function (as in qsearch),
 * removes the element from the queue and returns a pointer to it or
 * NULL if not found
 */
void* qremove(queue_t *qp,
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp){
	queue_t_priv *qp_priv;
	node_t *curr_p, *prev_p;
	void *removed_element;

	if (qp != NULL && searchfn != NULL && skeyp != NULL){
		qp_priv = (queue_t_priv*)qp;

		prev_p = NULL;
		curr_p = qp_priv->front;

		while (curr_p != NULL){

			if (searchfn(curr_p->element,skeyp)){

				removed_element = curr_p->element;

				if (prev_p == NULL){

					qp_priv->front = curr_p->next;
				} else {

					prev_p->next = curr_p->next;
				}

				if (curr_p->next == NULL){

					qp_priv->back = prev_p;
				}

				free(curr_p);
				return(removed_element);
			}

			prev_p = curr_p;
			curr_p = curr_p->next;
		}
	}
	
	return NULL;
}

/* concatenates elements of q2 into q1
 * q2 is dealocated, closed, and unusable upon completion 
 */
void qconcat(queue_t *q1p, queue_t *q2p){
	queue_t_priv *q1p_priv, *q2p_priv;
	node_t *q1_back, *q2_front, *q2_back;

	if (q1p == NULL || q2p == NULL){
		return;
	}

	q1p_priv = (queue_t_priv*)q1p;
	q2p_priv = (queue_t_priv*)q2p;

	q1_back = q1p_priv->back;
	q2_front = q2p_priv->front;
	q2_back = q2p_priv->back;

	if (q2_front == NULL){
		free(q2p_priv);
		return;
	}

	if (q1_back == NULL) {
		q1p_priv->front = q2_front;
		q1p_priv->back = q2_back;
	} else {
		q1p_priv->back->next = q2_front;
		q1p_priv->back = q2_back;
	}

	free(q2p_priv);
}

