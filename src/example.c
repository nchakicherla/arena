#include <stdio.h>
#include <inttypes.h>

#include "arena.h"

#define BIGNUM 20000

int main(void) {

	Arena p;
	initArena(&p);

	uint64_t **arr = palloc(&p, BIGNUM * sizeof(uint64_t*));

	for(uint64_t i = 0; i < BIGNUM; i++) {

		arr[i] = palloc(&p, BIGNUM * sizeof(uint64_t));

		for(uint64_t j = 0; j < BIGNUM; j++) {
			arr[i][j] = i * j;
		}
	} 
	printArenaInfo(&p);
	termArena(&p);
	return 0;
}
