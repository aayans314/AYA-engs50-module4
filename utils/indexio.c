#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include "hash.h"
#include "indexio.h"

#define HTSIZE 1000
#define MAXWORD 512

static FILE *outfp = NULL;

// for the queue
static void savedoc(void *elementp) {
	doc_t *dp = (doc_t *)elementp;

	fprintf(outfp, " %d %d", dp->id, dp->count);
}

// for the hash table
static void saveentry(void *elementp) {
	wordentry_t *we = (wordentry_t *)elementp;

	fprintf(outfp, "%s", we->word);
	qapply(we->docs, savedoc); // apply this to all docs inside "word"'s queue
	fprintf(outfp, "\n");
}

int32_t indexsave(hashtable_t *index, char *indexnm){

	int32_t result = 0;

	outfp = fopen(indexnm, "w");
	if (outfp == NULL) {
		result = 1;
	} else {
		happly(index, saveentry); // apply this to all word entries in the table
		fclose(outfp);
		outfp = NULL;
	}
	return result;
}


hashtable_t *indexload(char *indexnm){

    // basic initialization 
	FILE *fp = fopen(indexnm, "r");
	hashtable_t *index = NULL;
	char buf[MAXWORD];
	int id, count;

	if (fp == NULL) {
		return NULL;
	}
	index = hopen(HTSIZE);
	if (index == NULL) {
		fclose(fp);
		return NULL;
	}

    /* each pass reads one line: a word, then its id/count pairs */
	while (fscanf(fp, "%511s", buf) == 1) { // fitting the max word
		wordentry_t *we = malloc(sizeof(wordentry_t));
		int len = (int)strlen(buf);

		we->word = malloc(len + 1);
		strcpy(we->word, buf);
		we->docs = qopen();

		while (fscanf(fp, "%d %d", &id, &count) == 2) {
			doc_t *dp = malloc(sizeof(doc_t));
			dp->id = id;
			dp->count = count;
			qput(we->docs, dp);
		}
		hput(index, we, we->word, len);
	}
	fclose(fp);
	return index;
}
