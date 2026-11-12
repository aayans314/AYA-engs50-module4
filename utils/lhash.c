/* 
 * lhash.c -- implementation of locked hash table as an indexed set of
 * queues.  
 * 
 */
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include "lhash.h"
#include "hash.h"

// hidden representation of a locked hashtable
typedef struct lhashtable{
	hashtable_t *htp; //ordinary hash to hold elements
	pthread_mutex_t lock; //lock to keep the ht locked to one thread at a time
} lhashtable_t_priv;

// Make a locked hash table i.e. just an array of non-locked queues
lhashtable_t *lhopen(uint32_t hsize){
  lhashtable_t_priv* lhp = NULL;

	if (hsize>0){
		lhp = (lhashtable_t_priv*)malloc(sizeof(lhashtable_t_priv));
		if(lhp != NULL && hsize>0){//open hashtable and initialize lock
			lhp->htp = hopen(hsize);
			if(lhp->htp == NULL){
				free(lhp);
				lhp = NULL;
			} else if (pthread_mutex_init(&lhp->lock, NULL) != 0){
				hclose(lhp->htp);
				free(lhp);
				lhp = NULL;
			}
		}
	}
	return (lhashtable_t*)lhp;
}

// delete the locked hash table
void lhclose(lhashtable_t *lhtp){
  lhashtable_t_priv *lhp = (lhashtable_t_priv*)lhtp;

	if (lhp != NULL){//close hashtable and destroy lock
		hclose(lhp->htp);
		pthread_mutex_destroy(&lhp->lock);
		free(lhp);
		lhp = NULL;
	}
}

/* lhput -- puts an entry into a locked  hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t lhput(lhashtable_t *lhtp, void *ep, const char *key, int keylen){
  lhashtable_t_priv *lhp = (lhashtable_t_priv*)lhtp;
	int result = 1;

	if (lhp != NULL && ep != NULL && key != NULL && keylen >0){//lock, put in hashtable, unlock
		pthread_mutex_lock(&lhp->lock);
		result = hput(lhp->htp, ep, key, keylen);
		pthread_mutex_unlock(&lhp->lock);
	}
	return result;
}

/* lhapply -- applies a function to every entry in the locked hash table */
void lhapply(lhashtable_t *lhtp, void (*fn)(void* ep)){
	lhashtable_t_priv *lhp = (lhashtable_t_priv*)lhtp;

	if (lhp != NULL && fn != NULL){//lock, apply to hashtable, unlock
		pthread_mutex_lock(&lhp->lock);
		happly(lhp->htp, fn);
		pthread_mutex_unlock(&lhp->lock);
	}
}

/* lhsearch -- searchs in a locked hashtable for an entry under a
 * designated key using a designated search fn -- returns a pointer to
 * the entry or NULL if not found
 */
void *lhsearch(hashtable_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen){
	lhashtable_t_priv *lhp = (lhashtable_t_priv*)lhtp;
	void *element_result = NULL;

	if (lhp != NULL && searchfn != NULL && key != NULL && keylen >0){//lock, search through hashtable, unlock
		pthread_mutex_lock(&lhp->lock);
		element_result = hsearch(lhp->htp, searchfn, key, keylen);
		pthread_mutex_unlock(&lhp->lock);
	}
	return element_result;
}

/* lhremove -- removes and returns an entry from a locked hashtable
 * under a designated key using a designated search fn -- returns a
 * pointer to the entry or NULL if not found
 */
void *lhremove(hashtable_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen){
	lhashtable_t_priv *lhp = (lhashtable_t_priv*)lhtp;
	void *element_result = NULL;

	if (lhp != NULL && searchfn != NULL && key != NULL && keylen >0){//lock, remove from hashtable, unlock
		pthread_mutex_lock(&lhp->lock);
		element_result = hremove(lhp->htp, searchfn, key, keylen);
		pthread_mutex_unlock(&lhp->lock);
	}
	return element_result;
}

