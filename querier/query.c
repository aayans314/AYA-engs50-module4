/* 
 * query.c --- reads queries from the user and prints them back normalized
 * 
 * Author: Aayan Shah
 * Created: 10-29-2026
 * Version: 1.0
 * 
 * Description: Prompts with "> " and reads one line at a time. Each line
 * is split into words on spaces and tabs. If every word is alphabetic,
 * the words are printed back in lower case, separated by single spaces;
 * otherwise "[invalid query]" is printed. A line with no words prints
 * nothing. The program exits on EOF (Ctrl-D).
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define MAXQUERYWORDS 512
#define MAXWORDLENGTH 128
#define DELIMITERS " \t\n"

bool isStringAlpha(const char *str) {

	if (str[0] == '\0') return false;

	for (int i = 0; str[i] != '\0'; i++) {
		if (!isalpha((unsigned char)str[i])) return false;
	}
	return true;
}

void lowercaseString(char *str) {

	if (str[0] == '\0') return;

	for (int i = 0; str[i] != '\0'; i++) {
		str[i] = tolower((unsigned char)str[i]);
	}
}

int main(void) {

	char line[MAXQUERYWORDS * MAXWORDLENGTH];
	char *query[MAXQUERYWORDS];

	printf("> ");
	while (fgets(line, sizeof(line), stdin) != NULL) {

		int nwords = 0;
		bool valid = true;

		/* split the line into words, checking each one */
		char *word = strtok(line, DELIMITERS);
		while (word != NULL) {
			/* too many words, or a word with digits/punctuation */
			if (nwords == MAXQUERYWORDS || !isStringAlpha(word)) {
				valid = false;
				break;
			}
			lowercaseString(word);
			query[nwords] = word;
			nwords++;
			word = strtok(NULL, DELIMITERS);
		}

		/* only decide what to print once the whole line is checked */
		if (!valid) {
			printf("[invalid query]\n");
		}
		else if (nwords > 0) {
			for (int i = 0; i < nwords; i++) {
				if (i > 0) printf(" ");
				printf("%s", query[i]);
			}
			printf("\n");
		}

		printf("> ");
	}

	printf("\n");
	return 0;
}