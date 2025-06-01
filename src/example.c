#include <stdio.h>
#include <inttypes.h>
#include <stdalign.h>

#include "rkp_arena.h"

#define BIGNUM 20000

int main(void) {

	rkp_arena p;
	rkp_arena_init(&p);

	uint64_t **arr = rkp_arena_alloc(&p, BIGNUM * sizeof(uint64_t*), alignof(uint64_t *));

	for(uint64_t i = 0; i < BIGNUM; i++) {

		arr[i] = rkp_arena_alloc(&p, BIGNUM * sizeof(uint64_t), alignof(uint64_t));

		for(uint64_t j = 0; j < BIGNUM; j++) {
			arr[i][j] = i * j;
		}
	}
	rkp_arena_print_info(&p);
	rkp_arena_term(&p);
	return 0;
}
