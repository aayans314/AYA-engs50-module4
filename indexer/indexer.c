//step 5
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <dirent.h>

#include "webpage.h"
#include "queue.h"
#include "hash.h"
#include "pageio.h"
#include "indexio.h"
 
#define HTSIZE 1000   /* number of slots in the index hash table */
 

 
// static int total = 0;   /* running total used by sumwords() */
 
/* NormalizeWord(): convert word to lowercase in place; returns 1 if the word
 * is valid (at least 3 characters, all letters), 0 otherwise
 */
int NormalizeWord(char *word) {
	int i = 0;
	int valid = 1;
 
	if (strlen(word) < 3) {
		valid = 0;
	} else {
		while (word[i] != '\0') {
			if (isalpha((unsigned char)word[i]) == 0) {
				valid = 0;
			} else {
				word[i] = (char)tolower((unsigned char)word[i]);
			}
			i++;
		}
	}
	return valid;
}
 
/* wordmatch(): search function for the index; true if the entry's word
 * equals the key
 */
static bool wordmatch(void *elementp, const void *keyp) {
	wordentry_t *we = (wordentry_t *)elementp;
 
	return strcmp(we->word, (const char *)keyp) == 0;
}
 
/* docmatch(): search function for a word's document queue; true if the
 * document's id equals the key (a pointer to an int)
 */
static bool docmatch(void *elementp, const void *keyp) {
	doc_t *dp = (doc_t *)elementp;
 
	return dp->id == *(const int *)keyp;
}
 
/* adddoccount(): add one document's count to the running total */
// static void adddoccount(void *elementp) {
// 	doc_t *dp = (doc_t *)elementp;
 
// 	total = total + dp->count;
// }
 
/* addentrycount(): add up the counts in one word's document queue */
// static void addentrycount(void *elementp) {
// 	wordentry_t *we = (wordentry_t *)elementp;
 
// 	qapply(we->docs, adddoccount);
// }
 
/* freeentry(): free one word entry: every document in its queue, the
 * queue itself, the word, and the entry (used with happly)
 */
static void freeentry(void *elementp) {
	wordentry_t *we = (wordentry_t *)elementp;
	doc_t *dp;
 
	while ((dp = (doc_t *)qget(we->docs)) != NULL) {
		free(dp);
	}
	qclose(we->docs);
	free(we->word);
	free(we);
}
 
/* sumwords(): return the sum of every count in every document queue */
// static int sumwords(hashtable_t *index) {
// 	total = 0;
// 	happly(index, addentrycount);
// 	return total;
// }
 
/* newentry(): create an entry for word with an empty document queue;
 * returns NULL on failure
 */
static wordentry_t *newentry(char *word) {
	wordentry_t *we = malloc(sizeof(wordentry_t));
 
	if (we != NULL) {
		we->word = word;
		we->docs = qopen();
		if (we->docs == NULL) {
			free(we);
			we = NULL;
		}
	}
	return we;
}
 
/* addword(): record one occurrence of word in document id. If the word is
 * new, the index takes ownership of the word string; otherwise the string
 * is freed. Returns 0 on success, nonzero on failure.
 */
static int32_t addword(hashtable_t *index, char *word, int id) {
	int len = (int)strlen(word);
	wordentry_t *we = hsearch(index, wordmatch, word, len);
	doc_t *dp = NULL;
	int32_t result = 0;
 
	/* find the word's entry, or make one */
	if (we != NULL) {
		free(word);
	} else {
		we = newentry(word);
		if (we == NULL) {
			free(word);
			result = 1;
		} else if (hput(index, we, word, len) != 0) {
			freeentry(we);
			we = NULL;
			result = 1;
		}
	}
 
	/* find this document in the word's queue, or add it */
	if (we != NULL) {
		dp = qsearch(we->docs, docmatch, &id);
		if (dp == NULL) {
			dp = malloc(sizeof(doc_t));
			if (dp != NULL) {
				dp->id = id;
				dp->count = 0;
				if (qput(we->docs, dp) != 0) {
					free(dp);
					dp = NULL;
				}
			}
		}
		if (dp != NULL) {
			dp->count = dp->count + 1;
		} else {
			result = 1;
		}
	}
	return result;
}


int main(int argc, char *argv[]) {
	char *pagedir;
	char *indexnm;
	DIR *dp;
	webpage_t *page;
	hashtable_t *index;
	char *word = NULL;
	int pos;
	int id;

	if (argc != 3) {
		printf("usage: indexer <pagedir> <indexnm>, use ../pages for pagedir\n");
		exit(EXIT_FAILURE);
	}
	pagedir = argv[1];
	indexnm = argv[2];

	/* check the page directory exists */
	dp = opendir(pagedir);
	if (dp == NULL) {
		printf("page directory %s does not exist\n", pagedir);
		exit(EXIT_FAILURE);
	}
	closedir(dp);

	index = hopen(HTSIZE);
	if (index == NULL) {
		printf("failed to create the index\n");
		exit(EXIT_FAILURE);
	}

	/* load pages 1, 2, 3, ... until one is missing */
	for (id = 1; (page = pageload(id, pagedir)) != NULL; id++) {
		pos = 0;
		while ((pos = webpage_getNextWord(page, pos, &word)) > 0) {
			if (NormalizeWord(word)) {
				if (addword(index, word, id) != 0) {
					printf("failed to add a word to the index\n");
				}
			} else {
				free(word);
			}
			word = NULL;
		}
		webpage_delete(page);
	}

	if (indexsave(index, indexnm) != 0) {
		printf("failed to save the index to %s\n", indexnm);
		happly(index, freeentry);
		hclose(index);
		exit(EXIT_FAILURE);
	}
	happly(index, freeentry);
	hclose(index);
	exit(EXIT_SUCCESS);
}