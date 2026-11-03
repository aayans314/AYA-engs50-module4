/* 
 * query.c --- reads queries from the user and prints them back normalized
 * 
 * Author: Aayan Shah
 * Created: 10-29-2026
 * Version: 1.0
 * 
 * Description: Prompts with "> " and reads one line at a time. Each line
 * is split into words on spaces and tabs. If every word is alphabetic,
 * the words are printed back in lower case, separated by single spaces;
 * otherwise "[invalid query]" is printed. A line with no words prints
 * nothing. The program exits on EOF (Ctrl-D).
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "indexio.h"
#include "hash.h"


#define MAXQUERYWORDS 512
#define MAXWORDLENGTH 128
#define DELIMITERS " \t\n"

// please change here for your indexname
#define INDEXFILE "../indexer/index-depth1" 

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

int indexValue (hashtable_t *htp, char *str){

	wordentry_t *we = hsearch(htp, searchIndex, str, strlen(str));
	int count = 0;
	// this is just for 1 document so we will only have 1 doc_t
	if (we != NULL){
	int docid = 1;
	doc_t *doc = qsearch(we->docs, searchDocs, &docid); // change docid  here to loop
  if(doc != NULL) count = doc->count;
	}
	return count;

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
			nwords++;
			word = strtok(NULL, DELIMITERS);
		}
 
		// only decide what to print once the whole line is checked
		if (!valid) {
			printf("[invalid query]\n");
		}
		else if (nwords > 0) {
			int minCount = -1;   /* -1 means no word has been counted yet */
			for (int i = 0; i < nwords; i++) {
				if (hasMoreThan3(query[i]) && strcmp(query[i], "and") != 0 && strcmp(query[i], "or") != 0) {
					int currCount = indexValue(index, query[i]);
					printf("%s:%d ", query[i], currCount);
					if (minCount == -1 || currCount < minCount) minCount = currCount;
				}
			}
			/* if every word was skipped (e.g. "is it or"), print nothing */
			if (minCount != -1) printf("-- %d\n", minCount);
		}
 
		printf("> ");
	}
 
	printf("\n");   /* so the shell prompt starts on its own line after ^D */
 
	/* free the index here*/
	hclose(index);
	return 0;
}
