/* 
 * crawler_5.c --- 
 * 
 * Author: Aayan Shah
 * Created: 10-15-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"

// match the element by its url
bool searchwp(void* elementp,const void* keyurl){
    if(elementp != NULL && keyurl != NULL){
        webpage_t *wpelement = (webpage_t*) elementp;
        if(strcmp(webpage_getURL(wpelement),keyurl) == 0){
            return true;
        }
    }
    return false;
}

void printqueue(void *elements){
    webpage_t * wp = (webpage_t*) elements;
    printf("Found URL: %s\n", webpage_getURL(wp));
}


int32_t pagesave(webpage_t *pagep, int id, char *dirname){

	// initialize directory and stuff to put into the file
	mkdir(dirname, 0770);

	int depth = webpage_getDepth(pagep);
	char *url = webpage_getURL(pagep);
	char *content = webpage_getHTML(pagep);
	int length = webpage_getHTMLlen(pagep);

	// make the necessary string buffers

	char *path = NULL;
	sprintf(path, "%s/%d", dirname, id);
	FILE *fp = fopen(path, "w");

	if (fp == NULL){
		return(1);
	}
	
	fprintf(fp, "%s\n", url);
	fprintf(fp, "%d\n", depth);
	fprintf(fp, "%d\n", length);
	fprintf(fp, "%s", content);

	fclose(fp);
	return(0);
}

int main(void){

    webpage_t *wp = webpage_new("https://thayer.github.io/engs50/", 0, NULL);
    if(!webpage_fetch(wp)){
        printf("Failed to fetch\n");
        exit(EXIT_FAILURE);
    }

    int pos = 0;
    char *result;

		hashtable_t *ht = hopen(10);
    queue_t *qp = qopen();
    bool (*searchfn)(void*, const void*) = searchwp;

    while ((pos = webpage_getNextURL(wp, pos, &result)) > 0) {
        if(IsInternalURL(result)){            
					if (hsearch(ht, searchfn, result, (int)strlen(result)) == NULL){ //if result isn't in hashtable (hasn't been visited)
						webpage_t *wcurr = webpage_new(result, 1, NULL);
					  qput(qp, wcurr);
						hput(ht, wcurr, result, (int)strlen(result)); //put the webpage in the hashtable
					}
        }
        free(result);
    }

    void (*printpages)(void*) = printqueue;

		if (pagesave(wp, 1, "../pages") != 0){
			printf("Failed to save the page.\n");
			exit(EXIT_FAILURE);
		}
		
    qapply(qp, printpages);
    qapply(qp, webpage_delete);
    webpage_delete(wp);
    qclose(qp);
		hclose(ht);
    exit(EXIT_SUCCESS);
}
