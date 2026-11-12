/*
 * lhtest1.c -- single-threaded test of the locked hastable: every operation
 * behaves exactly like the matching hashtable operation
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lhash.h"

static int count = 0;   /* how many times countone() has been called */

/* item_t: a genereic element used for testing*/
typedef struct item {
	const char *key;
	int value;
} item_t;

/* check(): if cond is false, print msg and exit with failure */
static void check(bool cond, const char *msg) {
	if (!cond) {
		printf("FAILED: %s\n", msg);
		exit(EXIT_FAILURE);
	}
}

/* countone(): count each element (used with lhapply) */
static void countone(void *elementp) {
	(void)elementp;
	count = count + 1;
}

/* keymatch(): search function; true if the element's key equals the
	 search key */
static bool keymatch(void *elementp, const void *keyp) {
	item_t *ip = (item_t *)elementp;
	
	return strcmp(ip->key, (const char *)keyp) == 0;
}

int main(void) {
	lhashtable_t *lhtp = lhopen(10);
	item_t a = {"numbers", 1};
	item_t b = {"numbers", 2};
	item_t c = {"numbers", 3};
	item_t d = {"other", 4};
	const char *key = "numbers";
	int keylen = (int)strlen(key);;

	check(lhtp != NULL, "lhopen returned NULL");
	check(lhopen(0) == NULL, "lhopen with size 0 should return NULL");
	
	/* an empty locked hashtable */
	lhapply(lhtp, countone);
	check(count == 0, "lhapply on an empty table should not call the function");
	check(lhsearch(lhtp, keymatch, key, keylen) == NULL,
	      "lhsearch of an empty table should return NULL");
	check(lhremove(lhtp, keymatch, key, keylen) == NULL,
	      "lhremove of an empty table should return NULL");

	/* put three elements under the same key */
	check(lhput(lhtp, &a, key, keylen) == 0, "lhput to an empty table failed");
	check(lhput(lhtp, &b, key, keylen) == 0, "lhput to a non-empty table failed");
	check(lhput(lhtp, &c, key, keylen) == 0, "lhput to a non-empty table failed");

	/* apply and search */
	lhapply(lhtp, countone);
	check(count == 3, "lhapply should call the function once per element");
	check(lhsearch(lhtp, keymatch, key, keylen) != NULL, "lhsearch did not find an element");

	/* remove returns a matching elements and removes it */
	check(lhremove(lhtp, keymatch, key, keylen) != NULL,
				"lhremove did not return an element");
	count = 0;
	lhapply(lhtp, countone);
	check(count == 2, "lhremove did not remove exactly one element");

	check(lhremove(lhtp, keymatch, key, keylen) != NULL,
				"second lhremove did not return an element");
	check(lhremove(lhtp, keymatch, key, keylen) != NULL,
				"third lhremove did not return an element");
	check(lhremove(lhtp, keymatch, key, keylen) == NULL,
				"lhremove found an element that was not there");
	check(lhsearch(lhtp, keymatch, key, keylen) == NULL,
				"lhsearch found an element after all matching elements were removed");

	/* a different key should show a different hashtable entry */
	check(lhput(lhtp, &d, "other", 5) == 0, "lhput for a second key failed");
	check(lhsearch(lhtp, keymatch, "other", 5) == &d, "lhsearch for a second key failed");
	check(lhsearch(lhtp, keymatch, key, keylen) == NULL, "elements stored under another key showed for first key");
	
	/* NULL and invalid arguments are handled safely */
	check(lhput(NULL, &a, key, keylen) != 0,
				"lhput to a NULL table should fail");
	check(lhput(lhtp, NULL, key, keylen) != 0,
				"lhput with a NULL element should fail");
	check(lhput(lhtp, &a, NULL, keylen) != 0,
				"lhput with a NULL key should fail");
	check(lhput(lhtp, &a, key, 0) != 0,
				"lhput with a 0 keylen should fail");
	check(lhsearch(NULL, keymatch, key, keylen) == NULL,
				"lhsearch of a NULL table should return NULL");
	check(lhsearch(lhtp, NULL, key, keylen) == NULL,
				"lhsearch with a NULL keymatch should return NULL");
	check(lhsearch(lhtp, keymatch, NULL, keylen) == NULL,
				"lhsearch with a NULL key should return NULL");
	check(lhsearch(lhtp, keymatch, key, 0) == NULL,
				"lhsearch with a 0 keylen should return NULL");
	check(lhremove(NULL, keymatch, key, keylen) == NULL,
				"lhremove of a NULL table should return NULL");
	check(lhremove(lhtp, NULL, key, keylen) == NULL,
				"lhremove with a NULL keymatch should return NULL");
	check(lhremove(lhtp, keymatch, NULL, keylen) == NULL,
				"lhremove with a NULL key should return NULL");
	check(lhremove(lhtp, keymatch, key, 0) == NULL,
				"lhremove with a 0 keylen should return NULL");
	
	lhapply(NULL, countone);   /* should do nothing */
	lhapply(lhtp, NULL);        /* should do nothing */
	lhclose(NULL);             /* should do nothing */

	lhclose(lhtp);
	printf("SUCCESS\n");
	exit(EXIT_SUCCESS);
}
