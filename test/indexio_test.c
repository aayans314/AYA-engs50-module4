/*
Author: aayan shah
Purpose: test the index io
*/

#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "queue.h"
#include "indexio.h"

// this test loads an index and then saves it immediately to a different file
// first argument is the index filepath and second is the output to store it in
// indexio_test_in and indexio_test_out constitute the same items
// TEST: SUCCESS

int main(int argc, char *argv[]) {
	hashtable_t *index;

	if (argc != 3) {
		printf("usage: indexio_test <infile> <outfile>\n");
		exit(EXIT_FAILURE);
	}
	index = indexload(argv[1]);
	if (index == NULL) {
		printf("failed to load %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	if (indexsave(index, argv[2]) != 0) {
		printf("failed to save %s\n", argv[2]);
		exit(EXIT_FAILURE);
	}
	hclose(index);
	exit(EXIT_SUCCESS);
}