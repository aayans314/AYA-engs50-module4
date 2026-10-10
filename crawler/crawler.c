/* 
 * crawler.c --- 
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
#include "webpage.h"
#include "queue.h"

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

int main(void){

    webpage_t *wp = webpage_new("https://thayer.github.io/engs50/", 0, NULL);
    if(!webpage_fetch(wp)){
        printf("Failed to fetch\n");
        exit(EXIT_FAILURE);
    }

    int pos = 0;
    char *result;

    queue_t *qp = qopen();
    bool (*searchfn)(void*, const void*) = searchwp;

    while ((pos = webpage_getNextURL(wp, pos, &result)) > 0) {
        if(IsInternalURL(result)){            
            if(qsearch(qp, searchfn, result) == NULL){
                webpage_t *wcurr = webpage_new(result, 1, NULL);
                qput(qp, wcurr);
            }
        }
        free(result);
    }

    void (*printpages)(void*) = printqueue;

    qapply(qp, printpages);
    qapply(qp, webpage_delete);
    webpage_delete(wp);
    qclose(qp);
    exit(EXIT_SUCCESS);
}