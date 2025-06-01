#include "rkp_arena.h"

#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdalign.h>

#define MEMORY_HOG_FACTOR 8
#define DEF_BLOCK_SIZE 4096

static rkp_arena_block *newInitBlock(size_t block_size) {
	
	rkp_arena_block *block =  malloc(sizeof(rkp_arena_block));
	if(!block) {
		fprintf(stderr, "block alloc failed! exiting...\n");
		exit(1);
	}

	block->data = malloc(block_size);
	if(!block->data) {
		fprintf(stderr, "block data alloc failed! exiting...\n");
		exit(2);
	}

	block->data_size = block_size;
	block->next = NULL;
	
	return block;
}

int rkp_arena_init(rkp_arena *arena) {
	size_t block_size = DEF_BLOCK_SIZE;

	arena->first_block = newInitBlock(block_size);

	arena->bytes_used = 0;
	arena->bytes_allocd = sizeof(rkp_arena) + sizeof(rkp_arena_block) + block_size;
	arena->next_free = arena->first_block->data;
	arena->next_free_size = arena->first_block->data_size;
	arena->last_block_size = block_size;
	arena->last_block = arena->first_block;
	return 0;
}

int rkp_arena_term(rkp_arena *arena) {

	rkp_arena_block *curr = arena->first_block;
	rkp_arena_block *next = NULL;

	while(curr) {
		next = curr->next;
		free(curr->data);
		free(curr);
		curr = next;
	}
	return 0;
}

int rkp_arena_reset(rkp_arena *arena) { // preserves last_block_size from pre-reset

	rkp_arena_block *curr = arena->first_block;
	rkp_arena_block *next = NULL;

	while(curr) {
		next = curr->next;
		free(curr->data);
		free(curr);
		curr = next;
	}

	arena->first_block = newInitBlock(arena->last_block_size);
	arena->last_block = arena->first_block;

	arena->next_free = arena->first_block->data;
	arena->next_free_size = arena->first_block->data_size;

	arena->bytes_used = 0;
	arena->bytes_allocd = sizeof(rkp_arena) + sizeof(rkp_arena_block) + arena->last_block_size;
	return 0;
}

void *rkp_arena_alloc(rkp_arena *arena, size_t size, size_t alignment) {
	
	// bump up per alignment
	size_t current = (size_t)arena->next_free;
	size_t aligned = (current + alignment - 1) & ~(alignment - 1);
	size_t padding = aligned - current;

	if(arena->next_free_size < size + padding) {
		rkp_arena_block *last_block = arena->last_block;
		rkp_arena_block *new_block = NULL;

		size_t new_block_size = arena->last_block_size;

		while(new_block_size < size * MEMORY_HOG_FACTOR) {
			new_block_size = new_block_size * 2;
		}

		new_block = newInitBlock(new_block_size);
		if(!new_block) {
			return NULL;
		}
		last_block->next = new_block;
		last_block = new_block;

		arena->last_block = new_block;
		arena->last_block_size = new_block_size;
		arena->next_free = (char *)last_block->data + size;
		arena->next_free_size = new_block_size - size;

		arena->bytes_used += size + padding;
		arena->bytes_allocd += sizeof(rkp_arena_block) + new_block_size;

		return last_block->data;
	}

	void *output = (void *)aligned;
	arena->next_free = (void *)(aligned + size);
	arena->next_free_size  = arena->next_free_size - (size + padding);

	arena->bytes_used += size;

	return output;
}

void *rkp_arena_zalloc(rkp_arena *arena, size_t size, size_t alignment) {
	void* output = rkp_arena_alloc(arena, size, alignment);
	for(size_t i = 0; i < size; i++) {
		((char *)output)[i] = '\0';
	}
	return output;
}

void *rkp_arena_grow_alloc(void *ptr, size_t old_size, size_t new_size, rkp_arena *arena) {
	void *output_ptr = rkp_arena_alloc(arena, new_size, 32); // use default alignment of 32
	memcpy(output_ptr, ptr, old_size);
	return output_ptr;
}

char *rkp_arena_new_str(char *str, rkp_arena *arena) {
	
	char *output = NULL;
	size_t len = strlen(str);

	output = rkp_arena_alloc(arena, len + 1, alignof(char));

	for(size_t i = 0; i < len; i++) {
		output[i] = str[i];
	}
	output[len] = '\0';
	return output;
}

size_t rkp_arena_get_bytes_used(rkp_arena *arena) {
	return arena->bytes_used;
}

size_t rkp_arena_get_bytes_allocd(rkp_arena *arena) {
	return arena->bytes_allocd;
}

void rkp_arena_print_info(rkp_arena *arena) {
	printf("\nMEMPOOL INFO - \n");
	printf("\tUSED: %zu, (%f MB)\n", rkp_arena_get_bytes_used(arena), (double)rkp_arena_get_bytes_used(arena) / (1024 * 1024));
	printf("\tALLOCD: %zu, (%f MB)\n", rkp_arena_get_bytes_allocd(arena), (double)rkp_arena_get_bytes_allocd(arena) / (1024 * 1024));
	return;
}
