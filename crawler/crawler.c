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
#include <string.h>
#include "webpage.h"
#include "queue.h"


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
    
    while ((pos = webpage_getNextURL(wp, pos, &result)) > 0) {
        if(IsInternalURL(result)){            
					webpage_t *wcurr = webpage_new(result, 1, NULL);
          qput(qp, wcurr);
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
