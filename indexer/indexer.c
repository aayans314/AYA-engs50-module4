/* 
 * indexer.c --- 
 * 
 * Author: Andrea Soto
 * Created: 10-22-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"
#include "pageio.h"

int NormalizeWord(char *word){
	int i = 0;
	int valid = 1;

	if (strlen(word) < 3){
		valid = 0;
	} else {
		while (word[i] != '\0'){
			if (isalpha((unsigned char)word[i]) == 0){
				valid = 0;
			} else if ((word[1] >= 'A' && word[i] <= 'Z')){
				word[i] = (char)tolower((int)word[i]);
			}
		i++;
		}
	}

	return valid;
}

int main(void){
	webpage_t *page1 = NULL;
	int id = 1;
	int pos = 0;
	char *dirname = "../pages";
	char *word; 
	
	page1 = pageload(id, dirname);
	if (page1 == NULL){
		printf("failed to load webpage 1\n");
		exit(EXIT_FAILURE);
	}

	if (freopen("output.txt", "w", stdout) == NULL){
			printf("Couldn't redirect output\n");
			exit(EXIT_FAILURE);
	}
	
	while ((pos = webpage_getNextWord(page1, pos, &word)) > 0){
		if (NormalizeWord(word)){
				printf("%s\n", word);
			}
	}	

	webpage_delete(page1);
	
	return 0;
}
