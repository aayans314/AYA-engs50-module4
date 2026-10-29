/* 
 * query.c --- returns user query in normalized manner
 * 
 * Author: Aayan Shah
 * Created: 10-29-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>


int MAXQUERYWORDS = 512;
int MAXWORDLENGTH = 128;

bool isStringAlpha(const char *str){

	if (str[0] == '\0') return false;

	for(int i = 0; str[i] != '\0'; i++){
		if(!isalpha((unsigned char) str[i])) return false;
	}
		return true;
}

	void lowercaseString(char *str){
		
	if (str[0] == '\0') return;

	for(int i = 0; str[i] != '\0'; i++){
		str[i] = tolower((unsigned char)str[i]);
	}
	}

int main(){

	printf("> ");
	char **query = (char **) malloc(MAXQUERYWORDS * sizeof(char *));
	char *plainquery = (char *) malloc(MAXQUERYWORDS * MAXWORDLENGTH * sizeof(char));
	scanf("%s",plainquery);
	int i = 0;
	while(plainquery != NULL){
		if (i==0) query[i]=strtok(plainquery, " ");
		while(query[i] != NULL){
			if (!isStringAlpha(query[i])){
					printf("Invalid Query");
					break;
				}
				else{
			lowercaseString(query[i]);
			i++;
			query[i] = strtok(NULL, " ");
		}

		for(int j = 0; j < sizeof(query); j++){
			printf("%s",query[j]);
		}
				
		}
			printf("\n> ");
			i=0;
			scanf("%s", plainquery);
		}
		return 0;
}
