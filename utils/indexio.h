#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include "webpage.h"
#include "hash.h"
#include "queue.h"

typedef struct doc {
	int id;
	int count;
} doc_t;

typedef struct wordentry {
	char *word;
	queue_t *docs;
} wordentry_t;

int32_t indexsave(hashtable_t *index, char *dirnm);


hashtable_t *indexload(char *dirnm);
