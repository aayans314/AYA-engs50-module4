/*
 * lqueue.c -- implementation of the locked-queue module
 *
 * Each locked queue holds an ordinary queue (from the queue module) and a
 * mutex. Every operation locks the mutex, calls the queue module through
 * its interface, and unlocks the mutex. No queue code is reimplemented.
 */
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "queue.h"
#include "lqueue.h"

/* the hidden representation of a locked queue */
typedef struct lqueue {
	queue_t *qp;            /* the ordinary queue that holds the elements */
	pthread_mutex_t lock;   /* only the thread holding this may use qp */
} lqueue_struct_t;

lqueue_t *lqopen(void) {
	lqueue_struct_t *lq = malloc(sizeof(lqueue_struct_t));

	if (lq != NULL) {
		lq->qp = qopen();
		if (lq->qp == NULL) {
			free(lq);
			lq = NULL;
		} else if (pthread_mutex_init(&lq->lock, NULL) != 0) {
			qclose(lq->qp);
			free(lq);
			lq = NULL;
		}
	}
	return (lqueue_t*)lq;
}

void lqclose(lqueue_t *lqp) {
	lqueue_struct_t *lq = (lqueue_struct_t *)lqp;

	if (lq != NULL) {
		qclose(lq->qp);
		pthread_mutex_destroy(&lq->lock);
		free(lq);
		lq = NULL;
	}
}

int32_t lqput(lqueue_t *lqp, void *elementp) {
	lqueue_struct_t *lq = (lqueue_struct_t *)lqp;
	int32_t result = 1;

	if (lq != NULL) {
		pthread_mutex_lock(&lq->lock);
		result = qput(lq->qp, elementp);
		pthread_mutex_unlock(&lq->lock);
	}
	return result;
}

void *lqget(lqueue_t *lqp) {
	lqueue_struct_t *lq = (lqueue_struct_t *)lqp;
	void *elementp = NULL;

	if (lq != NULL) {
		pthread_mutex_lock(&lq->lock);
		elementp = qget(lq->qp);
		pthread_mutex_unlock(&lq->lock);
	}
	return elementp;
}

void lqapply(lqueue_t *lqp, void (*fn)(void *elementp)) {
	lqueue_struct_t *lq = (lqueue_struct_t *)lqp;

	if (lq != NULL && fn != NULL) {
		pthread_mutex_lock(&lq->lock);
		qapply(lq->qp, fn);
		pthread_mutex_unlock(&lq->lock);
	}
}

void *lqsearch(lqueue_t *lqp,
               bool (*searchfn)(void *elementp, const void *keyp),
               const void *skeyp) {
	lqueue_struct_t *lq = (lqueue_struct_t *)lqp;
	void *elementp = NULL;

	if (lq != NULL && searchfn != NULL) {
		pthread_mutex_lock(&lq->lock);
		elementp = qsearch(lq->qp, searchfn, skeyp);
		pthread_mutex_unlock(&lq->lock);
	}
	return elementp;
}
