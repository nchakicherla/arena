#ifndef RKP_ARENA_H
#define RKP_ARENA_H

#include <stdlib.h>
#include <string.h>

struct s_rkp_arena_block;

typedef struct s_rkp_arena {
	size_t bytes_used;
	size_t bytes_allocd;

	void *next_free;
	size_t next_free_size;
	size_t last_block_size;
	struct s_rkp_arena_block *last_block;
	struct s_rkp_arena_block *first_block;
} rkp_arena;

typedef struct s_rkp_arena_block {
	void *data;
	size_t data_size;
	struct s_rkp_arena_block *next;
} rkp_arena_block;

int rkp_arena_init(rkp_arena *arena);
int rkp_arena_term(rkp_arena *arena);
int rkp_arena_reset(rkp_arena *arena);
void *rkp_arena_alloc(rkp_arena *arena, size_t size, size_t alignment);
void *rkp_arena_zalloc(rkp_arena *arena, size_t size, size_t alignment);
void *rkp_arena_grow_alloc(void *ptr, size_t old_size, size_t new_size, rkp_arena *arena);
char *rkp_arena_new_str(char *str, rkp_arena *arena);
size_t rkp_arena_get_bytes_used(rkp_arena *arena);
size_t rkp_arena_get_bytes_allocd(rkp_arena *arena);
void rkp_arena_print_info(rkp_arena *arena);

#endif // RKP_ARENA_H
