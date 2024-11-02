#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <time.h>

#define IDX_MAX 	(SIZE_MAX - 1)
#define IDX_ERR 	(SIZE_MAX)

#include "arena.h"

#define BIGNUM 100000

int main(void) {

	Arena p;
	initArena(&p);

	uint32_t *arr = palloc(&p, BIGNUM);
	for(size_t i = 0; i < BIGNUM; ++i) {
		arr[i] = i * i;
		printf("%" PRId32 "\n", arr[i]);
	}

	termArena(&p);
	return 0;
}
