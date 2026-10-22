/* 
 * pageio.c --- 
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
#include <inttypes.h>
#include <unistd.h>
#include <webpage.h>

#define MAXPATH 512    /* longest file path pagesave() will build */
#define MAXURL 1024    /* longest url size */
/*
 * pagesave -- save the page in filename id in directory dirnm
 *
 * returns: 0 for success; nonzero otherwise
 *
 * The suggested format for the file is:
 *   <url>
 *   <depth>
 *   <html-length>
 *   <html>
 */
int32_t pagesave(webpage_t *pagep, int id, char *dirnm) {
	char path[MAXPATH];
	FILE *fp;
	int n;
	int32_t result = 1;

	if (pagep != NULL && dirnm != NULL) {
		n = snprintf(path, MAXPATH, "%s/%d", dirnm, id);
		if (n > 0 && n < MAXPATH) {
			fp = fopen(path, "w");
			if (fp != NULL) {
				fprintf(fp, "%s\n", webpage_getURL(pagep));
				fprintf(fp, "%d\n", webpage_getDepth(pagep));
				fprintf(fp, "%d\n", webpage_getHTMLlen(pagep));
				fprintf(fp, "%s", webpage_getHTML(pagep));
				fclose(fp);
				result = 0;
			}
		}
	}
	return result;
}
/* 
 * pageload -- loads the numbered filename <id> in direcory <dirnm>
 * into a new webpage
 *
 * returns: non-NULL for success; NULL otherwise
 */
webpage_t *pageload(int id, char *dirnm){
	char path[MAXPATH], url[MAXURL], *html;
	FILE *fp;
	int n, depth, length, ch;
	
	webpage_t *result = NULL;

	if (dirnm != NULL){
		n = snprintf(path, MAXPATH, "%s/%d", dirnm, id);
		if (n > 0 && n < MAXPATH) {
			fp = fopen(path, "r");
			if (fp != NULL) {
				if (fscanf(fp, "%s %d %d", url, &depth, &length) == 3){
					if ((ch = fgetc(fp)) != EOF){
						html = malloc((size_t)length+1);
						for (int i = 0; i < length; i++){
						  if ((ch = fgetc(fp)) != EOF) {
								html[i] = (char)ch;
							}
						}
						html[length] = '\0';
						if ((int)sizeof(html) == length){
							result = webpage_new(url, depth, html);
						}
						free(html);
					}
				}
			}
			fclose(fp);
		}
	}

	return result;
}
