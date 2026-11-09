#pragma once
/*
 * lqueue.h -- public interface to the locked-queue module
 *
 * A locked queue is a queue that many threads can use at the same time.
 * Each operation locks the queue, performs the matching queue-module
 * operation, and unlocks it, so only one thread changes the queue at once.
 */
#include <stdint.h>
#include <stdbool.h>

/* the locked-queue representation is hidden from users of the module */
typedef void lqueue_t;

/* lqopen(): create an empty locked queue; returns NULL on failure */
lqueue_t *lqopen(void);

/* lqclose(): deallocate a locked queue (as qclose() does for a queue);
 * no other thread may be using it when it is closed
 */
void lqclose(lqueue_t *lqp);

/* lqput(): put an element at the end of the locked queue;
 * returns 0 if successful, nonzero otherwise
 */
int32_t lqput(lqueue_t *lqp, void *elementp);

/* lqget(): remove and return the first element, or NULL if it is empty */
void *lqget(lqueue_t *lqp);

/* lqapply(): apply a function to every element of the locked queue */
void lqapply(lqueue_t *lqp, void (*fn)(void *elementp));

/* lqsearch(): search the locked queue using a supplied boolean function
 * (as qsearch() does); returns a pointer to the element, or NULL if not
 * found. The element stays in the queue, so another thread may remove it.
 */
void *lqsearch(lqueue_t *lqp,
               bool (*searchfn)(void *elementp, const void *keyp),
               const void *skeyp);
