/* 
 * crawler.c --- for module 4
 * 
 * Author: Aayan Shah
 * Created: 10-10-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "webpage.h"
#include "queue.h"

// to pass into the qsearch function; match the element by its url
bool searchwp(void* elementp,const void* keyurl){
    if(elementp != NULL && keyurl != NULL){
        webpage_t *wpelement = (webpage_t*) elementp;
        if(strcmp(webpage_getURL(wpelement),keyurl) == 0){
            return true;
        }
    }
    return false;
}

// to pass into the qapply function; print the element's url
void printqueue(void *elements){
    webpage_t * wp = (webpage_t*) elements;
    printf("Found URL: %s\n", webpage_getURL(wp));
}

int main(void){
	// open the root
    webpage_t *wp = webpage_new("https://thayer.github.io/engs50/", 0, NULL);
    if(!webpage_fetch(wp)){
        printf("Failed to fetch\n");
        exit(EXIT_FAILURE);
    }

		// this is just following the description from the webpage.h file
    int pos = 0;
    char *result;

		// initialize the queue and search function pointer
    queue_t *qp = qopen();
    bool (*searchfn)(void*, const void*) = searchwp;

		// this is all from the example illustrated in webpage.h
    while ((pos = webpage_getNextURL(wp, pos, &result)) > 0) {
        if(IsInternalURL(result)){
					// if did not find the webpage's url in the queue, add it to the queue
            if(qsearch(qp, searchfn, result) == NULL){
                webpage_t *wcurr = webpage_new(result, 1, NULL);
                qput(qp, wcurr);
            }
        }
				// also following the webpage.h
        free(result);
    }

		// initialize pointer for the print function
    void (*printpages)(void*) = printqueue;

		// print function to each element of the queue
    qapply(qp, printpages);

		// freeing memory: all elements of the queue, the root webpage and the queue
    qapply(qp, webpage_delete);
    webpage_delete(wp);
    qclose(qp);
    exit(EXIT_SUCCESS);
}
