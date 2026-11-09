/*
 * lqtest2.c -- dual-threaded test of the locked queue
 *
 * Part 1: two threads put N elements each into the same queue at the same
 *         time. Every element must arrive, and each thread's elements must
 *         stay in the order that thread put them.
 * Part 2: the queue is filled with 2N elements, then two threads get from
 *         it at the same time until it is empty. Every element must be
 *         taken exactly once.
 * Without the lock, these usually lose elements, duplicate them, or crash.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "lqueue.h"

#define N 100000   /* elements per thread */

/* one element: which thread made it, and its position in that thread's order */
typedef struct item {
	int thread;
	int seq;
} item_t;

/* what each thread is told, and what it reports back */
typedef struct worker {
	int id;         /* 0 or 1 */
	lqueue_t *lqp;  /* the shared locked queue */
	int failures;   /* how many operations failed */
	int got;        /* how many elements it took (part 2) */
	int *seen;      /* seen[seq] counts how often each element was taken */
} worker_t;

/* the starting gate: main holds this lock while it creates the threads,
 * and each thread waits for it before starting, so both run at once
 */
static pthread_mutex_t gate = PTHREAD_MUTEX_INITIALIZER;

/* waitatgate(): wait until main opens the starting gate */
static void waitatgate(void) {
	pthread_mutex_lock(&gate);
	pthread_mutex_unlock(&gate);
}

/* check(): if cond is false, print msg and exit with failure */
static void check(bool cond, const char *msg) {
	if (!cond) {
		printf("FAILED: %s\n", msg);
		exit(EXIT_FAILURE);
	}
}

/* producer(): put N items into the shared queue */
static void *producer(void *arg) {
	worker_t *w = (worker_t *)arg;

	waitatgate();
	for (int i = 0; i < N; i++) {
		item_t *ip = malloc(sizeof(item_t));
		if (ip == NULL) {
			w->failures++;
		} else {
			ip->thread = w->id;
			ip->seq = i;
			if (lqput(w->lqp, ip) != 0) {
				free(ip);
				w->failures++;
			}
		}
	}
	return NULL;
}

/* consumer(): take items from the shared queue until it is empty */
static void *consumer(void *arg) {
	worker_t *w = (worker_t *)arg;
	item_t *ip;

	waitatgate();
	while ((ip = (item_t *)lqget(w->lqp)) != NULL) {
		w->seen[ip->seq]++;
		w->got++;
		free(ip);
	}
	return NULL;
}

int main(void) {
	lqueue_t *lqp = lqopen();
	pthread_t tid[2];
	worker_t w[2];
	int next[2] = {0, 0};   /* the seq expected next from each thread */
	int *seen = calloc(2 * N, sizeof(int));
	item_t *ip;
	int total = 0;

	check(lqp != NULL, "lqopen returned NULL");
	check(seen != NULL, "calloc failed");

	/* ---- part 1: two threads put at the same time ---- */
	pthread_mutex_lock(&gate);
	for (int t = 0; t < 2; t++) {
		w[t].id = t;
		w[t].lqp = lqp;
		w[t].failures = 0;
		w[t].got = 0;
		w[t].seen = seen;
		check(pthread_create(&tid[t], NULL, producer, &w[t]) == 0,
		      "pthread_create failed");
	}
	pthread_mutex_unlock(&gate);   /* open the gate: both threads start */
	for (int t = 0; t < 2; t++) {
		check(pthread_join(tid[t], NULL) == 0, "pthread_join failed");
		check(w[t].failures == 0, "a producer's lqput failed");
	}

	while ((ip = (item_t *)lqget(lqp)) != NULL) {
		check(ip->thread == 0 || ip->thread == 1, "got a corrupted element");
		check(ip->seq == next[ip->thread],
		      "a thread's elements came out of order, or were lost");
		next[ip->thread]++;
		total++;
		free(ip);
	}
	check(next[0] == N && next[1] == N, "not every element put was found");
	check(total == 2 * N, "wrong number of elements in the queue");
	printf("part 1 passed: %d elements put concurrently by 2 threads\n", total);

	/* ---- part 2: two threads get at the same time ---- */
	for (int i = 0; i < 2 * N; i++) {
		ip = malloc(sizeof(item_t));
		check(ip != NULL, "malloc failed");
		ip->thread = 0;
		ip->seq = i;
		check(lqput(lqp, ip) == 0, "lqput failed");
	}
	pthread_mutex_lock(&gate);
	for (int t = 0; t < 2; t++) {
		w[t].got = 0;
		check(pthread_create(&tid[t], NULL, consumer, &w[t]) == 0,
		      "pthread_create failed");
	}
	pthread_mutex_unlock(&gate);   /* open the gate: both threads start */
	for (int t = 0; t < 2; t++) {
		check(pthread_join(tid[t], NULL) == 0, "pthread_join failed");
	}

	check(w[0].got + w[1].got == 2 * N, "the consumers took the wrong number of elements");
	for (int i = 0; i < 2 * N; i++) {
		check(seen[i] == 1, "an element was taken twice, or never");
	}
	check(lqget(lqp) == NULL, "queue should be empty");
	printf("part 2 passed: %d elements taken concurrently (%d + %d)\n",
	       w[0].got + w[1].got, w[0].got, w[1].got);

	free(seen);
	lqclose(lqp);
	exit(EXIT_SUCCESS);
}
