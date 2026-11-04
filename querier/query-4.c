/* 
 * query.c --- reads queries from the user and prints them back normalized
 * 
 * Author: Andrea Soto
 * Created: 11-03-2026
 * Version: 3.0
 * 
 * Description: Prompts with "> " and reads one line at a time. Each line
 * is split into words on spaces and tabs. If every word is alphabetic,
 * the words are printed back in lower case, separated by single spaces;
 * otherwise "[invalid query]" is printed. A line with no words prints
 * nothing. The program exits on EOF (Ctrl-D).
 *
 * Now, it takes a COMPLEX query and it creates a ranked list of the documents that
 * have all of the words in the query
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "indexio.h"
#include "hash.h"
#include "pageio.h"


#define MAXQUERYWORDS 512
#define MAXWORDLENGTH 128
#define DELIMITERS " \t\n"

// please change here for your indexname
#define INDEXFILE "../indexer/index-depth1" 
#define IDNUM 7
#define DIRNAME "../pages"

bool isStringAlpha(const char *str) {

	if (str[0] == '\0') return false;

	for (int i = 0; str[i] != '\0'; i++) {
		if (!isalpha((unsigned char)str[i])) return false;
	}
	return true;
}

void lowercaseString(char *str) {

	if (str[0] == '\0') return;

	for (int i = 0; str[i] != '\0'; i++) {
		str[i] = tolower((unsigned char)str[i]);
	}
}

bool hasMoreThan3(const char *str){

	return (strlen(str)>=3);

}

// index is a hashtable containing word entries which has a word and a queue
bool searchIndex(void* elementp, const void* searchkeyp){

	wordentry_t *we = (wordentry_t *) elementp;
	char *elem_str = (char *) we->word;
	char *searchkey_str = (char *) searchkeyp;

	return ( strcmp(elem_str, searchkey_str)==0);
}

bool searchDocs (void* elementp, const void* keyp){

	doc_t *doc = (doc_t *) elementp;
	int *key = (int*) keyp;

	return (doc->id == *key);
}

int indexValue (hashtable_t *htp, char *str, int *docid){ //include document ID for loop

	wordentry_t *we = hsearch(htp, searchIndex, str, strlen(str));
	int count = 0;
	// this is just for 1 document so we will only have 1 doc_t
	if (we != NULL){

	doc_t *doc = qsearch(we->docs, searchDocs, docid); // change docid  here to loop
  if(doc != NULL) count = doc->count;
	}
	return count;

}

static void printquery(int nwords, char **query){

	for (int i = 0; i < nwords; i++){
		printf("%s ", query[i]);
	}
	printf("\n");

}

	

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

int main(void) {
 
	char line[MAXQUERYWORDS * MAXWORDLENGTH];
	char *query[MAXQUERYWORDS];
 
	/* load the index once, before reading any queries */
	hashtable_t *index = indexload(INDEXFILE);
	if (index == NULL) {
		fprintf(stderr, "query: could not load index from %s\n", INDEXFILE);
		exit(EXIT_FAILURE);
	}
 
	printf("> ");
	while (fgets(line, sizeof(line), stdin) != NULL) {
 
		/* start fresh for every line */
		int nwords = 0;
		bool valid = true;
 
		/* split the line into words, checking each one */
		char *word = strtok(line, DELIMITERS);
		while (word != NULL) {
			/* too many words, or a word with digits/punctuation */
			if (nwords == MAXQUERYWORDS || !isStringAlpha(word)) {
				valid = false;
				break;
			}
			lowercaseString(word);
			query[nwords] = word;
			if ( strcmp(query[nwords], "or") == 0) {
				
			nwords++;
			word = strtok(NULL, DELIMITERS);
		}
 
		// only decide what to print once the whole line is checked
		if (!valid) {
			printf("[invalid query]\n");
		}
		else if (nwords > 0) {

			printquery(nwords, query); //print query back
			for (int j = 1; j <= IDNUM; j++){ //loop for amount of webpages
				int minCount = -1;   /* -1 means no word has been counted yet */
				bool hasAll = true;
				for (int i = 0; i < nwords; i++) {
					if (hasMoreThan3(query[i]) && strcmp(query[i], "and") != 0 && strcmp(query[i], "or") != 0) {
						int currCount = indexValue(index, query[i], &j);
						if (currCount == 0) hasAll = false; 
						if (minCount == -1 || currCount < minCount) minCount = currCount;
					}
				}
				if (hasAll){ //has to have all words to rank
					webpage_t *pagep = pageload(j, DIRNAME);
					char *url = webpage_getURL(pagep);
					printf("rank: %d : doc: %d : %s \n", minCount, j, url);
				}
			}
			/* if every word was skipped (e.g. "is it or"), print nothing */
			//make sure to remove the print statements of the previous ones and add new ones
			//should print back the words in the querie and
			//rank: <rN> : doc: <docIDN> : <urlN> \n
		}
 
		printf("> ");
	}
 
	printf("\n");   /* so the shell prompt starts on its own line after ^D */
 
	/* free the index here*/
	happly(index, freeentry);
	hclose(index);
	return 0;
}
