#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>
#include <string.h>

#define MEMORY_HOG_FACTOR 8
#define DEFAULT_PAGE_SIZE 1024

struct s_Page;

typedef struct s_Arena {
	size_t bytes_used;
	size_t bytes_allocd;

	void *next_free;
	size_t next_free_size;
	size_t last_block_size;
	struct s_Page *last_block;
	struct s_Page *first_block;
} Arena;

typedef struct s_Page {
	void *data;
	size_t data_size;
	struct s_Page *next;
} Page;

int initArena(Arena *pool);

int resetArena(Arena *pool);

int termArena(Arena *pool);

void *palloc(Arena *pool, size_t size);

void *pzalloc(Arena *pool, size_t size);

void *pGrowAlloc(void *ptr, size_t old_size, size_t new_size, Arena *pool);

char *pNewStr(char *str, Arena *pool);

size_t getBytesUsed(Arena *pool);

size_t getBytesAllocd(Arena *pool);

void printArenaInfo(Arena *pool);

static Page *newInitPage(size_t block_size) {
	
	Page *block = malloc(sizeof(Page));
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

int initArena(Arena *pool) {

	size_t block_size = DEFAULT_PAGE_SIZE;

	pool->first_block = newInitPage(block_size);

	pool->bytes_used = 0;
	pool->bytes_allocd = sizeof(Arena) + sizeof(Page) + block_size;
	pool->next_free = pool->first_block->data;
	pool->next_free_size = pool->first_block->data_size;
	pool->last_block_size = block_size;
	pool->last_block = pool->first_block;
	return 0;
}

int termArena(Arena *pool) {

	Page *curr = pool->first_block;
	Page *next = NULL;

	while(curr) {
		next = curr->next;
		free(curr->data);
		free(curr);
		curr = next;
	}
	return 0;
}

int resetArena(Arena *pool) {

	Page *curr = pool->first_block;
	Page *next = NULL;

	while(curr) {
		next = curr->next;
		free(curr->data);
		free(curr);
		curr = next;
	}

	pool->first_block = newInitPage(pool->last_block_size);
	pool->last_block = pool->first_block;

	pool->next_free = pool->first_block->data;
	pool->next_free_size = pool->first_block->data_size;

	pool->bytes_used = 0;
	pool->bytes_allocd = sizeof(Arena) + sizeof(Page) + pool->last_block_size;
	return 0;
}

void *palloc(Arena *pool, size_t size) {
	
	if(pool->next_free_size < size) {
		Page *last_block = pool->last_block;
		Page *new_block = NULL;

		size_t new_block_size = pool->last_block_size;

		while(new_block_size < size * MEMORY_HOG_FACTOR) {
			new_block_size = new_block_size * 2;
		}

		new_block = newInitPage(new_block_size);
		if(!new_block) {
			return NULL;
		}
		last_block->next = new_block;
		last_block = new_block;

		pool->last_block = new_block;
		pool->last_block_size = new_block_size;
		pool->next_free = (char *)last_block->data + size;
		pool->next_free_size = new_block_size - size;

		pool->bytes_used += size;
		pool->bytes_allocd += sizeof(Page) + new_block_size;

		return last_block->data;
	}

	void *output = pool->next_free;
	pool->next_free = (char *)pool->next_free + size;
	pool->next_free_size  = pool->next_free_size - size;

	pool->bytes_used += size;

	return output;
}

void *pzalloc(Arena *pool, size_t size) {
	void* output = palloc(pool, size);
	for(size_t i = 0; i < size; i++) {
		((char *)output)[i] = '\0';
	}
	return output;
}

void *pGrowAlloc(void *ptr, size_t old_size, size_t new_size, Arena *pool) {
	void *output_ptr = palloc(pool, new_size);
	memcpy(output_ptr, ptr, old_size);
	return output_ptr;
}

char *pNewStr(char *str, Arena *pool) {
	
	char *output = NULL;
	size_t len = strlen(str);

	output = palloc(pool, len + 1);

	for(size_t i = 0; i < len; i++) {
		output[i] = str[i];
	}
	output[len] = '\0';
	return output;
}

size_t getBytesUsed(Arena *pool) {
	return pool->bytes_used;
}

size_t getBytesAllocd(Arena *pool) {
	return pool->bytes_allocd;
}

void printArenaInfo(Arena *pool) {
	printf("\nArena INFO - \n");
	printf("\tUSED: %zu, (%f MB)\n", getBytesUsed(pool), (double)getBytesUsed(pool) / (1024 * 1024));
	printf("\tALLOCD: %zu, (%f MB)\n", getBytesAllocd(pool), (double)getBytesAllocd(pool) / (1024 * 1024));
	return;
}

#endif // ARENA_H
