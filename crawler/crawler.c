/* 
 * crawler.c --- 
 * 
 * Author: Yash Bharadwaj
 * Created: 10-16-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"
#include "pageio.h"

#define HTSIZE 100     /* number of slots in the hash table of visited URLs */

/* usage(): print the usage statement and exit with failure */
static void usage(void) {
	printf("usage: crawler <seedurl> (https://thayer.github.io/engs50/) <pagedir> <maxdepth>\n");
	exit(EXIT_FAILURE);
}

/* urlmatch(): search function for the hash table; each element is a URL
 * string, so it matches when it equals the key
 */
static bool urlmatch(void *elementp, const void *keyp) {
	bool found = false;

	if (elementp != NULL && keyp != NULL) {
		found = (strcmp((char *)elementp, (const char *)keyp) == 0);
	}
	return found;
}

/* copystring(): return a malloc'd copy of s, or NULL if malloc fails */
static char *copystring(const char *s) {
	char *copy = malloc(strlen(s) + 1);

	if (copy != NULL) {
		strcpy(copy, s);
	}
	return copy;
}

/* isdirectory(): true if dirname exists, is a directory, and we can
 * create files in it
 */
static bool isdirectory(const char *dirname) {
	struct stat sb;

	return stat(dirname, &sb) == 0 && S_ISDIR(sb.st_mode) &&
	       access(dirname, W_OK | X_OK) == 0;
}

/* addurl(): if url has not been seen before, remember it in the hash table
 * and queue a new webpage for it at the given depth. Does nothing if the
 * url was already seen. Returns 0 on success, nonzero on failure.
 */
static int32_t addurl(char *url, int depth, queue_t *qp, hashtable_t *ht) {
	int len = (int)strlen(url);
	char *urlcopy;
	webpage_t *newpage;
	int32_t result = 0;

	if (hsearch(ht, urlmatch, url, len) == NULL) {
		urlcopy = copystring(url);
		newpage = webpage_new(url, depth, NULL);

		if (urlcopy == NULL || newpage == NULL) {
			free(urlcopy);
			if (newpage != NULL) {
				webpage_delete(newpage);
			}
			result = 1;
		} else if (hput(ht, urlcopy, urlcopy, len) != 0) {
			free(urlcopy);
			webpage_delete(newpage);
			result = 1;
		} else if (qput(qp, newpage) != 0) {
			webpage_delete(newpage);   /* urlcopy now belongs to the hash table */
      free(hremove(ht, urlmatch, url, len));   /* take it back out of the table */
			result = 1;
		}
	}
	return result;
}

int main(int argc, char *argv[]) {
	char *seedurl;
	char *pagedir;
	char *endp;
	long maxdepth;
	queue_t *qp;
	hashtable_t *ht;
	webpage_t *page;
	char *url;
	int pos;
	int depth;
	int id = 1;   /* id of the next page to save */

	/* check the arguments */
	if (argc != 4) {
		usage();
	}
	seedurl = argv[1];
	pagedir = argv[2];

	maxdepth = strtol(argv[3], &endp, 10);
	if (argv[3][0] == '\0' || *endp != '\0' || maxdepth < 0) {
		printf("crawler: maxdepth must be a non-negative integer\n");
		usage();
	}
  // mkdir(pagedir, 0777);   /* create pagedir if it doesn't exist yet, uncomment to enable any folder */
	if (!isdirectory(pagedir)) {
		printf("crawler: %s is not an existing, writable directory\n", pagedir);
		usage();
	}
	if (!IsInternalURL(seedurl)) {
		printf("crawler: seedurl must begin with %s\n", INTERNAL_URL_PREFIX);
		usage();
	}

	/* set up the queue of pages to crawl and the table of URLs seen */
	qp = qopen();
	ht = hopen(HTSIZE);
	if (qp == NULL || ht == NULL) {
		printf("crawler: could not create the queue or hash table\n");
		exit(EXIT_FAILURE);
	}
	if (addurl(seedurl, 0, qp, ht) != 0) {
		printf("crawler: could not add the seed URL\n");
		exit(EXIT_FAILURE);
	}

	/* crawl: take each page off the queue, fetch it, save it, and
	 * (if it is not too deep) queue the new internal pages it links to
	 */
	while ((page = (webpage_t *)qget(qp)) != NULL) {
		if (!webpage_fetch(page)) {
			printf("could not fetch %s -- skipping it\n", webpage_getURL(page));
		} else {
			if (pagesave(page, id, pagedir) == 0) {
				printf("saved %d: %s\n", id, webpage_getURL(page));
				id++;
			} else {
				printf("could not save %s\n", webpage_getURL(page));
			}

			depth = webpage_getDepth(page);
			if (depth < maxdepth) {
				pos = 0;
				url = NULL;
				while ((pos = webpage_getNextURL(page, pos, &url)) > 0) {
					if (IsInternalURL(url) && addurl(url, depth + 1, qp, ht) != 0) {
						printf("could not queue %s\n", url);
					}
					free(url);
					url = NULL;
				}
			}
		}
		webpage_delete(page);
	}
	printf("crawler: saved %d pages\n", id - 1);

	/* clean up: free every saved URL string, then close everything */
	happly(ht, free);
	hclose(ht);
	qclose(qp);
	exit(EXIT_SUCCESS);
}
