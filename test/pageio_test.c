/* 
 * pageio_test.c --- 
 * 
 * Author: Andrea Soto
 * Created: 10-20-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include "webpage.h"
#include "pageio.h"

int main (void){
	webpage_t *test_page = NULL;
	webpage_t *retest_page = NULL;
	int id = 1;
	char *pagedir = "../pages";
	char *new_pagedir = "./pages";

	test_page = pageload(id, pagedir);
	if (test_page == NULL){
		printf("pageload failed to return a pointer\n");
		exit(EXIT_FAILURE);
	}

	if (pagesave(test_page, id, new_pagedir) != 0){
		printf("pagesave failed to return 0\n");
		exit(EXIT_FAILURE);
	}

	retest_page = pageload(id, new_pagedir);
	if (retest_page == NULL){
		printf("pageload failed to return a pointer to retest\n");
		exit(EXIT_FAILURE);
	}

	if ((strcmp(webpage_getURL(test_page), webpage_getURL(retest_page)) == 0) && (webpage_getDepth(test_page) == webpage_getDepth(retest_page)) && (strcmp(webpage_getHTML(test_page), webpage_getHTML(retest_page)) == 0)){
		printf("SUCCESS: both loaded pages are the same\n");
	}else {
		printf("pages were not the same\n");
		exit(EXIT_FAILURE);
	}

	webpage_delete(test_page);
	webpage_delete(retest_page);

	return 0;
}

	




	
