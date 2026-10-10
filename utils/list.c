/* 
 * list.c --- 
 * 
 * Author: Yash Bharadwaj
 * Created: 10-02-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */
#include <stdlib.h>
#include <string.h>
#include "list.h"

static car_t *front = NULL;

int32_t lput(car_t *cp) {
	int32_t result = 1;

	if (cp != NULL) {
		cp->next = front;
		front = cp;
		result = 0;
	}
	return result;
}

car_t *lget() {
	car_t *cp = front;

	if (front != NULL) {
		front = front->next;
		cp->next = NULL;
	}
	return cp;
}

void lapply(void (*fn)(car_t *cp)) {
	car_t *p;

	if (fn != NULL) {
		for (p = front; p != NULL; p = p->next) {
			fn(p);
		}
	}
}

car_t *lremove(char *platep) {
	car_t *prev = NULL;
	car_t *curr = NULL;

	if (platep != NULL) {
		curr = front;
		while (curr != NULL && strcmp(curr->plate, platep) != 0) {
			prev = curr;
			curr = curr->next;
		}
		if (curr != NULL) {
			if (prev == NULL) {
				front = curr->next;
			} else {
				prev->next = curr->next;
			}
			curr->next = NULL;
		}
	}
	return curr;
}

