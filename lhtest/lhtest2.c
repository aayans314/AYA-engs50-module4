/*
 * lhtest2.c -- dual-threaded test of the locked hashtable
 *
 * Part 1: two threads put N elements each into the same key at the
 *         same time. Every element must be found
 * Part 2: two threads remove elements from the same key at the same time.
 *         Every element must be removed exactly once.
 * Without the lock, these operations interfere with one another.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include "lhash.h"

#define N 100000   /* elements per thread */

/* one element: which thread made it, its position, and its hash key */
typedef struct item {
	int thread;
	int seq;
	const char *key;
} item_t;

/* what each thread is told, and what it reports back */
typedef struct worker {
	int id;         /* 0 or 1 */
	lhashtable_t *lhtp;  /* the shared locked hashtable */
	int failures;   /* how many operations failed */
	int got;        /* how many elements it removed (part 2) */
	int *seen;      /* seen[thread*N + seq] counts removed elements */
} worker_t;

static const char *group = "items";
static int keylen;

/* the starting gate: main holds this lock while it creates the threads,
 * and each thread waits for it before starting, so both run at once
 */
static pthread_mutex_t gate = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t seenlock = PTHREAD_MUTEX_INITIALIZER;

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

/* keymatch(): search function; true if the element's key equals the
	 search key */
static bool keymatch(void *elementp, const void *keyp) {
	item_t *ip = (item_t *)elementp;
	
	return strcmp(ip->key, (const char *)keyp) == 0;
}

/* producer(): put N items into the shared hashtable */
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
			ip->key = group;
			if (lhput(w->lhtp, ip, group, keylen) != 0) {
				free(ip);
				w->failures++;
			}
		}
	}
	return NULL;
}

/* remover(): removes items from the shared hashtable until it is empty */
static void *remover(void *arg) {
	worker_t *w = (worker_t *)arg;
	item_t *ip;

	waitatgate();
	while ((ip = (item_t *)lhremove(w->lhtp, keymatch, group, keylen)) != NULL) {
		pthread_mutex_lock(&seenlock);
		w->seen[ip->thread * N + ip->seq]++;
		w->got++;
		pthread_mutex_unlock(&seenlock);
		free(ip);
	}
	return NULL;
}

int main(void) {
	lhashtable_t *lhtp = lhopen(100);
	pthread_t tid[2];
	worker_t w[2];
	int *seen = calloc(2 * N, sizeof(int));

	check(lhtp != NULL, "lhopen returned NULL");
	check(seen != NULL, "calloc failed");
	keylen = (int)strlen(group);
	
	/* ---- part 1: two threads put at the same time ---- */
	pthread_mutex_lock(&gate);
	for (int t = 0; t < 2; t++) {
		w[t].id = t;
		w[t].lhtp = lhtp;
		w[t].failures = 0;
		w[t].got = 0;
		w[t].seen = seen;
		check(pthread_create(&tid[t], NULL, producer, &w[t]) == 0,
		      "pthread_create failed");
	}
	pthread_mutex_unlock(&gate);   /* open the gate: both threads start */
	for (int t = 0; t < 2; t++) {
		check(pthread_join(tid[t], NULL) == 0, "pthread_join failed");
		check(w[t].failures == 0, "a producer's lhput failed");
	}

	/* verify that every inserted item can be found */
	for (int t = 0; t < 2; t++) {
		for (int i = 0; i < N; i++) {
			item_t wanted;
			wanted.thread = t;
			wanted.seq = i;
			wanted.key = group;
			/* the callback checks the hashkey, so it confirmed that the
				 shared key has entreies after the concurrent insertion. */
			check(lhsearch(lhtp, keymatch, wanted.key, keylen) != NULL,
						"lhsearch could not find an inserted element");
		}
	}
	printf("part 1 passed: %d elements put concurrently by 2 threads\n", 2 * N);

	/* ---- part 2: two threads get at the same time ---- */
	pthread_mutex_lock(&gate);
	for (int t = 0; t < 2; t++) {
		w[t].got = 0;
		check(pthread_create(&tid[t], NULL, remover, &w[t]) == 0,
		      "pthread_create failed");
	}
	pthread_mutex_unlock(&gate);   /* open the gate: both threads start */
	for (int t = 0; t < 2; t++) {
		check(pthread_join(tid[t], NULL) == 0, "pthread_join failed");
	}

	check(w[0].got + w[1].got == 2 * N, "the removers took the wrong number of elements");
	for (int i = 0; i < 2 * N; i++) {
		check(seen[i] == 1, "an element was removed twice, or never");
	}
	check(lhsearch(lhtp, keymatch, group, keylen) == NULL,
				"hashtable should be empty");
	printf("part 2 passed: %d elements removed concurrently (%d + %d)\n",
	       w[0].got + w[1].got, w[0].got, w[1].got);

	free(seen);
	lhclose(lhtp);
	exit(EXIT_SUCCESS);
}
