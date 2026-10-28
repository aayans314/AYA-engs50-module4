
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"
#include "pageio.h"
 
#define HTSIZE 1000   /* number of slots in the index hash table */
 
/* one entry in the index: a word and how many times it occurs */
typedef struct wordcount {
	char *word;
	int count;
} wordcount_t;
 
static int total = 0;   /* running total used by sumwords() */
 
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
	wordcount_t *wc = (wordcount_t *)elementp;
 
	return strcmp(wc->word, (const char *)keyp) == 0;
}
 
/* addcount(): add one entry's count to the running total (used with happly) */
static void addcount(void *elementp) {
	wordcount_t *wc = (wordcount_t *)elementp;
 
	total = total + wc->count;
}
 
/* freewordcount(): free one entry and its word (used with happly) */
static void freewordcount(void *elementp) {
	wordcount_t *wc = (wordcount_t *)elementp;
 
	free(wc->word);
	free(wc);
}
 
/* sumwords(): return the sum of the counts of every word in the index */
static int sumwords(hashtable_t *index) {
	total = 0;
	happly(index, addcount);
	return total;
}
 
/* addword(): record one occurrence of word in the index. If the word is
 * new, the index takes ownership of the word string; otherwise the string
 * is freed. Returns 0 on success, nonzero on failure.
 */
static int32_t addword(hashtable_t *index, char *word) {
	int len = (int)strlen(word);
	wordcount_t *wc = hsearch(index, wordmatch, word, len);
	int32_t result = 0;
 
	if (wc != NULL) {
		wc->count = wc->count + 1;
		free(word);
	} else {
		wc = malloc(sizeof(wordcount_t));
		if (wc == NULL) {
			free(word);
			result = 1;
		} else {
			wc->word = word;
			wc->count = 1;
			if (hput(index, wc, word, len) != 0) {
				freewordcount(wc);
				result = 1;
			}
		}
	}
	return result;
}
 
int main(void) {
	int id = 1;
	char *dirname = "../pages";
	webpage_t *page;
	hashtable_t *index;
	char *word = NULL;
	int pos = 0;
 
	page = pageload(id, dirname);
	if (page == NULL) {
		printf("failed to load webpage %d from %s\n", id, dirname);
		exit(EXIT_FAILURE);
	}
 
	index = hopen(HTSIZE);
	if (index == NULL) {
		printf("failed to create the index\n");
		webpage_delete(page);
		exit(EXIT_FAILURE);
	}
 
	while ((pos = webpage_getNextWord(page, pos, &word)) > 0) {
		if (NormalizeWord(word)) {
			if (addword(index, word) != 0) {
				printf("failed to add a word to the index\n");
			}
		} else {
			free(word);
		}
		word = NULL;
	}
 
	printf("sum of word counts in page %d: %d\n", id, sumwords(index));
 
	happly(index, freewordcount);
	hclose(index);
	webpage_delete(page);
	exit(EXIT_SUCCESS);
}
