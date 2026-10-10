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

int main(void){
    printf("Hello ");
    webpage_t *wp = webpage_new("https://thayer.github.io/engs50/", 0, NULL);
    if(!webpage_fetch(wp)){
        printf("Failed to fetch\n");
        exit(EXIT_FAILURE);
    }

    int pos = 0;
    char *result;

    while ((pos = webpage_getNextURL(wp, pos, &result)) > 0) {
        printf("Found url: %s\t", result);
        if(IsInternalURL(result)) printf("[Internal]\n");
        else printf("[External]\n");
        free(result);
    }
    webpage_delete(wp);
    exit(EXIT_SUCCESS);
}