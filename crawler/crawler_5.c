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

int32_t pagesave(webpage_t *pagep, int id, char *dirname){

	// initialize directory and stuff to put into the file
	mkdir(dirname, 0770);

	int depth = webpage_getDepth(pagep);
	char *url = webpage_getURL(pagep);
	char *content = webpage_getHTML(pagep);
	int length = webpage_getHTMLlen(pagep);

	// make the necessary string buffers

	char *path;
	sprintf(path, "%s/%d", dirname, id);
	FILE *fp = fopen(path, w);

	fprintf(fp, "%s\n", url);
	fprintf(fp, "%d\n", depth);
	fprintf(fp, "%d\n", length);
	fprintf(fp, "%s", content);

	fclose(fp);
}
