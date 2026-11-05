/*
 * query-5.c --- Module 6, Step 5: the complete querier
 *
 * usage: query <pageDirectory> <indexFile> [-q]
 *
 * Builds on query-4.c. Loads the index in indexFile (made by the indexer
 * from the pages in pageDirectory), then reads queries one line at a time
 * and prints the matching documents in rank order. Queries are words with
 * optional 'and' / 'or' operators; 'and' (or just a space) binds tighter
 * than 'or'. With -q, no prompt is printed, so queries can be read from a
 * file:  query pages index -q < myqueries.txt > myoutput
 *
 * Changes from query-4.c:
 *  - page directory, index file, and -q come from the command line
 *  - the number of pages is counted from the page directory (no IDNUM)
 *  - only documents with rank > 0 are printed; otherwise "no documents match"
 *  - URLs are read from the first line of each page file (no leaked pageloads)
 *  - the query is echoed back in full, operators included
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "indexio.h"
#include "hash.h"
#include "queue.h"
 
#define MAXQUERYWORDS 512
#define MAXWORDLENGTH 128
#define MAXLINE (MAXQUERYWORDS * MAXWORDLENGTH)
#define MAXPATH 512
#define MAXURL 1024
#define DELIMITERS " \t\n\r"
 
/* one parsed query */
typedef struct query {
	char *words[MAXQUERYWORDS];     /* the words, without 'and' / 'or' */
	int nwords;
	char *display[MAXQUERYWORDS];   /* everything typed, for echoing back */
	int ndisplay;
	int groupStart[MAXQUERYWORDS];  /* words[groupStart[g]..groupEnd[g]-1] */
	int groupEnd[MAXQUERYWORDS];    /* are the g'th group of and'ed words */
	int ngroups;                    /* groups are separated by 'or' */
} query_t;
 
/* usage(): print the usage statement and exit with failure */
static void usage(void) {
	printf("usage: query <pageDirectory> <indexFile> [-q]\n");
	exit(EXIT_FAILURE);
}
 
/* isStringAlpha(): true if str is non-empty and contains only letters */
static bool isStringAlpha(const char *str) {
	bool ok = (str[0] != '\0');
 
	for (int i = 0; str[i] != '\0'; i++) {
		if (!isalpha((unsigned char)str[i])) {
			ok = false;
		}
	}
	return ok;
}
 
/* lowercaseString(): convert str to lower case in place */
static void lowercaseString(char *str) {
	for (int i = 0; str[i] != '\0'; i++) {
		str[i] = (char)tolower((unsigned char)str[i]);
	}
}
 
/* searchIndex(): search function for the index; matches a word entry */
static bool searchIndex(void *elementp, const void *searchkeyp) {
	wordentry_t *we = (wordentry_t *)elementp;
 
	return strcmp(we->word, (const char *)searchkeyp) == 0;
}
 
/* searchDocs(): search function for a word's queue; matches a doc id */
static bool searchDocs(void *elementp, const void *keyp) {
	doc_t *doc = (doc_t *)elementp;
 
	return doc->id == *(const int *)keyp;
}
 
/* indexValue(): how many times word occurs in document docid (0 if never) */
static int indexValue(hashtable_t *htp, char *word, int docid) {
	wordentry_t *we = hsearch(htp, searchIndex, word, (int)strlen(word));
	doc_t *doc;
	int count = 0;
 
	if (we != NULL) {
		doc = qsearch(we->docs, searchDocs, &docid);
		if (doc != NULL) {
			count = doc->count;
		}
	}
	return count;
}
 
/* freeentry(): free one word entry and its document queue (with happly) */
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
 
/* isdirectory(): true if dirname exists, is a directory, and is readable */
static bool isdirectory(const char *dirname) {
	struct stat sb;
 
	return stat(dirname, &sb) == 0 && S_ISDIR(sb.st_mode) &&
	       access(dirname, R_OK | X_OK) == 0;
}
 
/* countpages(): number of crawler pages 1, 2, 3, ... readable in pagedir */
static int countpages(const char *pagedir) {
	char path[MAXPATH];
	int n = 0;
	bool more = true;
 
	while (more) {
		snprintf(path, MAXPATH, "%s/%d", pagedir, n + 1);
		if (access(path, R_OK) == 0) {
			n++;
		} else {
			more = false;
		}
	}
	return n;
}
 
/* geturl(): copy the URL (first line) of page id in pagedir into url;
 * returns true on success
 */
static bool geturl(const char *pagedir, int id, char *url, int size) {
	char path[MAXPATH];
	FILE *fp;
	bool ok = false;
 
	snprintf(path, MAXPATH, "%s/%d", pagedir, id);
	fp = fopen(path, "r");
	if (fp != NULL) {
		if (fgets(url, size, fp) != NULL) {
			url[strcspn(url, "\n")] = '\0';
			ok = true;
		}
		fclose(fp);
	}
	return ok;
}
 
/* parsequery(): split line into words and check the grammar
 *   query       ::= <andsequence> [or <andsequence>]...
 *   andsequence ::= <word> [ [and] <word>]...
 * Fills in q. Returns false for an invalid query (non-letters, or an
 * operator at the start, at the end, or next to another operator).
 * A blank line is valid with no words.
 */
static bool parsequery(char *line, query_t *q) {
	char *word;
	bool valid = true;
	bool expectWord = true;   /* true at the start and after an operator */
 
	q->nwords = 0;
	q->ndisplay = 0;
	q->ngroups = 1;
	q->groupStart[0] = 0;
 
	word = strtok(line, DELIMITERS);
	while (word != NULL && valid) {
		if (q->ndisplay == MAXQUERYWORDS || !isStringAlpha(word)) {
			valid = false;
		} else {
			lowercaseString(word);
			q->display[q->ndisplay] = word;
			q->ndisplay++;
 
			if (strcmp(word, "and") == 0 || strcmp(word, "or") == 0) {
				if (expectWord) {
					valid = false;   /* operator at the start or after another */
				} else if (strcmp(word, "or") == 0) {
					q->groupEnd[q->ngroups - 1] = q->nwords;
					q->groupStart[q->ngroups] = q->nwords;
					q->ngroups++;
				}
				expectWord = true;
			} else {
				q->words[q->nwords] = word;
				q->nwords++;
				expectWord = false;
			}
			word = strtok(NULL, DELIMITERS);
		}
	}
 
	if (valid && expectWord && q->ndisplay > 0) {
		valid = false;   /* query ends with an operator */
	}
	q->groupEnd[q->ngroups - 1] = q->nwords;
	return valid;
}
 
/* printquery(): echo the query back, operators included */
static void printquery(query_t *q) {
	for (int i = 0; i < q->ndisplay; i++) {
		printf("%s%s", (i > 0) ? " " : "", q->display[i]);
	}
	printf("\n");
}
 
/* rankdocs(): rank every page 1..npages. Within an and-group the rank is
 * the smallest count; the ranks of or'ed groups are added together. Words
 * shorter than 3 letters are not in the index, so they are skipped.
 */
static void rankdocs(hashtable_t *index, query_t *q, int *ranks, int npages) {
	for (int id = 1; id <= npages; id++) {
		int total = 0;
 
		for (int g = 0; g < q->ngroups; g++) {
			int minCount = -1;   /* -1 means no word counted yet */
 
			for (int k = q->groupStart[g]; k < q->groupEnd[g]; k++) {
				if (strlen(q->words[k]) >= 3) {
					int count = indexValue(index, q->words[k], id);
					if (minCount == -1 || count < minCount) {
						minCount = count;
					}
				}
			}
			if (minCount > 0) {
				total = total + minCount;
			}
		}
		ranks[id - 1] = total;
	}
}
 
/* printresults(): print the documents with rank > 0, highest rank first */
static void printresults(int *ranks, int npages, const char *pagedir) {
	bool *printed = calloc(npages, sizeof(bool));
	char url[MAXURL];
	int nfound = 0;
	int best = 0;
 
	if (printed == NULL) {
		printf("query: out of memory\n");
	} else {
		while (best != -1) {
			best = -1;
			for (int d = 0; d < npages; d++) {
				if (!printed[d] && ranks[d] > 0 &&
				    (best == -1 || ranks[d] > ranks[best])) {
					best = d;
				}
			}
			if (best != -1) {
				if (!geturl(pagedir, best + 1, url, MAXURL)) {
					strcpy(url, "(url unavailable)");
				}
				printf("rank: %d : doc: %d : %s\n", ranks[best], best + 1, url);
				printed[best] = true;
				nfound++;
			}
		}
		if (nfound == 0) {
			printf("no documents match\n");
		}
		free(printed);
	}
}
 
int main(int argc, char *argv[]) {
	static char line[MAXLINE];
	static query_t q;
	char *pagedir;
	char *indexfile;
	bool quiet = false;
	int npages;
	int *ranks;
	hashtable_t *index;
 
	/* check the arguments */
	if (argc == 4 && strcmp(argv[3], "-q") == 0) {
		quiet = true;
	} else if (argc != 3) {
		usage();
	}
	pagedir = argv[1];
	indexfile = argv[2];
 
	if (strlen(pagedir) > MAXPATH - 20) {
		fprintf(stderr, "query: page directory name is too long\n");
		exit(EXIT_FAILURE);
	}
	if (!isdirectory(pagedir)) {
		fprintf(stderr, "query: %s is not an accessible directory\n", pagedir);
		exit(EXIT_FAILURE);
	}
	npages = countpages(pagedir);
	if (npages == 0) {
		fprintf(stderr, "query: %s has not been crawled (no page 1)\n", pagedir);
		exit(EXIT_FAILURE);
	}
	if (access(indexfile, R_OK) != 0) {
		fprintf(stderr, "query: cannot read index file %s\n", indexfile);
		exit(EXIT_FAILURE);
	}
 
	/* load the index once, before reading any queries */
	index = indexload(indexfile);
	if (index == NULL) {
		fprintf(stderr, "query: could not load index from %s\n", indexfile);
		exit(EXIT_FAILURE);
	}
	ranks = malloc(npages * sizeof(int));
	if (ranks == NULL) {
		fprintf(stderr, "query: out of memory\n");
		happly(index, freeentry);
		hclose(index);
		exit(EXIT_FAILURE);
	}
 
	/* read and answer queries until end-of-file */
	if (!quiet) {
		printf("> ");
	}
	while (fgets(line, MAXLINE, stdin) != NULL) {
		if (!parsequery(line, &q)) {
			printf("[invalid query]\n");
		} else if (q.ndisplay > 0) {
			printquery(&q);
			rankdocs(index, &q, ranks, npages);
			printresults(ranks, npages, pagedir);
		}
		if (!quiet) {
			printf("> ");
		}
	}
	if (!quiet) {
		printf("\n");   /* so the shell prompt starts on its own line after ^D */
	}
 
	free(ranks);
	happly(index, freeentry);
	hclose(index);
	exit(EXIT_SUCCESS);
}
