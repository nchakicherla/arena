#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <time.h>

#include "arena.h"

#define BIG_NUMBER 100000

int main(void) {

	// setAllocator(malloc);

	Arena p;
	initArena(&p);

	uint64_t *arr = palloc(&p, BIG_NUMBER * sizeof(uint32_t));
	for(uint64_t i = 0; i < BIG_NUMBER; i++) {
		arr[i] = i * i;
		printf("%" PRIu64 ",%" PRIu64 "\n", i, arr[i]);
	}

	termArena(&p);
	return 0;
}
