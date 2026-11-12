#pragma once
/*
 * lhash.h -- public interface to the locked-hash module
 * 
 * Allowes for a hash table that many threads can use at the same
 * time, where each operation locks the hash, performs the matching
 * hash-module operation, and unlocks itm only allowing one thread to
 * modify the hash table at a time.
 */
#include <stdint.h>
#include <stdbool.h>

/* the locked representation of a hashtable hidden */
typedef void lhashtable_t;

/* lhopen -- opens a locked, empty hash table with initial size hsize */
lhashtable_t *lhopen(uint32_t hsize);

/* lhclose -- closes a locked hash table */
void lhclose(lhashtable_t *lhtp);

/* lhput -- puts an entry into a locked hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t lhput(lhashtable_t *lhtp, void *ep, const char *key, int keylen);

/* lhapply -- applies a function to every entry in hash table */
void lhapply(lhashtable_t *lhtp, void (*fn)(void* ep));

/* lhsearch -- searchs locked hash table for an entry under a
 * designated key using a designated search fn -- returns a pointer to
 * the entry or NULL if not found
 */
void *lhsearch(lhashtable_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen);

/* lhremove -- removes and returns an entry from a locked hashtable
 * under a designated key using a designated search fn -- returns a
 * pointer to the entry or NULL if not found
 */
void *lhremove(lhashtable_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen);

