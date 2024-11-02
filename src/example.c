#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <time.h>

#include "arena.h"

#define BIGNUM 100000

int main(void) {

	Arena p;
	initArena(&p);

	uint64_t *arr = palloc(&p, BIGNUM * sizeof(uint32_t));
	for(uint64_t i = 0; i < BIGNUM; i++) {
		arr[i] = i * i;
		printf("%" PRIu64 ",%" PRIu64 "\n", i, arr[i]);
	}

	termArena(&p);
	return 0;
}
