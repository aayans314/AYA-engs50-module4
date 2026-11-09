/*
 * lqtest1.c -- single-threaded test of the locked queue: every operation
 * behaves exactly like the matching queue operation
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lqueue.h"

static int count = 0;   /* how many times countone() has been called */

/* check(): if cond is false, print msg and exit with failure */
static void check(bool cond, const char *msg) {
	if (!cond) {
		printf("FAILED: %s\n", msg);
		exit(EXIT_FAILURE);
	}
}

/* countone(): count each element (used with lqapply) */
static void countone(void *elementp) {
	(void)elementp;
	count = count + 1;
}

/* intmatch(): search function; true if the element's int equals the key */
static bool intmatch(void *elementp, const void *keyp) {
	return *(int *)elementp == *(const int *)keyp;
}

int main(void) {
	lqueue_t *lqp = lqopen();
	int a = 1, b = 2, c = 3;
	int key;

	check(lqp != NULL, "lqopen returned NULL");

	/* an empty locked queue */
	check(lqget(lqp) == NULL, "lqget from an empty queue should return NULL");
	lqapply(lqp, countone);
	check(count == 0, "lqapply on an empty queue should not call the function");
	key = 1;
	check(lqsearch(lqp, intmatch, &key) == NULL,
	      "lqsearch of an empty queue should return NULL");

	/* put three elements */
	check(lqput(lqp, &a) == 0, "lqput to an empty queue failed");
	check(lqput(lqp, &b) == 0, "lqput to a non-empty queue failed");
	check(lqput(lqp, &c) == 0, "lqput to a non-empty queue failed");

	/* apply and search */
	lqapply(lqp, countone);
	check(count == 3, "lqapply should call the function once per element");
	key = 2;
	check(lqsearch(lqp, intmatch, &key) == &b, "lqsearch did not find an element");
	key = 9;
	check(lqsearch(lqp, intmatch, &key) == NULL,
	      "lqsearch found an element that is not there");

	/* get returns elements in first-in, first-out order */
	check(lqget(lqp) == &a, "first lqget returned the wrong element");
	check(lqget(lqp) == &b, "second lqget returned the wrong element");
	check(lqget(lqp) == &c, "third lqget returned the wrong element");
	check(lqget(lqp) == NULL, "queue should be empty");

	/* NULL arguments are handled safely */
	check(lqput(NULL, &a) != 0, "lqput to a NULL queue should fail");
	check(lqget(NULL) == NULL, "lqget from a NULL queue should return NULL");
	check(lqsearch(NULL, intmatch, &key) == NULL,
	      "lqsearch of a NULL queue should return NULL");
	check(lqsearch(lqp, NULL, &key) == NULL,
	      "lqsearch with a NULL function should return NULL");
	lqapply(NULL, countone);   /* should do nothing */
	lqapply(lqp, NULL);        /* should do nothing */
	lqclose(NULL);             /* should do nothing */

	lqclose(lqp);
	exit(EXIT_SUCCESS);
}
