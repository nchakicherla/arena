#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>
#include <string.h>

#define MEMORY_HOG_FACTOR 8
#define DEFAULT_PAGE_SIZE 4096

struct s_Page;

typedef void *(*alloc_fn)(size_t);

static alloc_fn allocator = malloc;

typedef struct s_Arena {
	size_t bytes_used;
	size_t bytes_allocd;

	void *next_free;
	size_t next_free_size;
	size_t last_block_size;
	struct s_Page *last_page;
	struct s_Page *first_page;
} Arena;

typedef struct s_Page {
	void *data;
	size_t data_size;
	struct s_Page *next;
} Page;

static int setAllocator(alloc_fn fn_ptr);

static int initArena(Arena *pool);

static int resetArena(Arena *pool);

static int termArena(Arena *pool);

static void *palloc(Arena *pool, size_t size);

static void *pzalloc(Arena *pool, size_t size);

static void *pGrowAlloc(void *ptr, size_t old_size, size_t new_size, Arena *pool);

static char *pNewStr(char *str, Arena *pool);

static size_t getBytesUsed(Arena *pool);

static size_t getBytesAllocd(Arena *pool);

static void printArenaInfo(Arena *pool);

static int setAllocator(alloc_fn fn_ptr) {
	
	void *validation = NULL;
	if(!(validation = fn_ptr(8))) {
		return 1;
	}
	free(validation);

	allocator = fn_ptr;
	return 0;
}

static Page *newInitPage(size_t block_size) {
	
	Page *page = allocator(sizeof(Page));
	if(!page) {
		fprintf(stderr, "page alloc failed! exiting...\n");
		exit(1);
	}

	page->data = allocator(block_size);
	if(!page->data) {
		fprintf(stderr, "page data alloc failed! exiting...\n");
		exit(2);
	}

	page->data_size = block_size;
	page->next = NULL;
	
	return page;
}

static int initArena(Arena *pool) {

	size_t block_size = DEFAULT_PAGE_SIZE;

	pool->first_page = newInitPage(block_size);

	pool->bytes_used = 0;
	pool->bytes_allocd = sizeof(Arena) + sizeof(Page) + block_size;
	pool->next_free = pool->first_page->data;
	pool->next_free_size = pool->first_page->data_size;
	pool->last_block_size = block_size;
	pool->last_page = pool->first_page;
	return 0;
}

static int termArena(Arena *pool) {

	Page *curr = pool->first_page;
	Page *next = NULL;

	while(curr) {
		next = curr->next;
		free(curr->data);
		free(curr);
		curr = next;
	}
	return 0;
}

static int resetArena(Arena *pool) {

	Page *curr = pool->first_page;
	Page *next = NULL;

	while(curr) {
		next = curr->next;
		free(curr->data);
		free(curr);
		curr = next;
	}

	pool->first_page = newInitPage(pool->last_block_size);
	pool->last_page = pool->first_page;

	pool->next_free = pool->first_page->data;
	pool->next_free_size = pool->first_page->data_size;

	pool->bytes_used = 0;
	pool->bytes_allocd = sizeof(Arena) + sizeof(Page) + pool->last_block_size;
	return 0;
}

static void *palloc(Arena *pool, size_t size) {
	
	if(pool->next_free_size < size) {
		Page *last_page = pool->last_page;
		Page *new_block = NULL;

		size_t new_block_size = pool->last_block_size;

		while(new_block_size < size * MEMORY_HOG_FACTOR) {
			new_block_size = new_block_size * 2;
		}

		new_block = newInitPage(new_block_size);
		if(!new_block) {
			return NULL;
		}
		last_page->next = new_block;
		last_page = new_block;

		pool->last_page = new_block;
		pool->last_block_size = new_block_size;
		pool->next_free = (char *)last_page->data + size;
		pool->next_free_size = new_block_size - size;

		pool->bytes_used += size;
		pool->bytes_allocd += sizeof(Page) + new_block_size;

		return last_page->data;
	}

	void *output = pool->next_free;
	pool->next_free = (char *)pool->next_free + size;
	pool->next_free_size  = pool->next_free_size - size;

	pool->bytes_used += size;

	return output;
}

static void *pzalloc(Arena *pool, size_t size) {
	void* output = palloc(pool, size);
	for(size_t i = 0; i < size; i++) {
		((char *)output)[i] = '\0';
	}
	return output;
}

static void *pGrowAlloc(void *ptr, size_t old_size, size_t new_size, Arena *pool) {
	void *output_ptr = palloc(pool, new_size);
	memcpy(output_ptr, ptr, old_size);
	return output_ptr;
}

static char *pNewStr(char *str, Arena *pool) {
	
	char *output = NULL;
	size_t len = strlen(str);

	output = palloc(pool, len + 1);

	for(size_t i = 0; i < len; i++) {
		output[i] = str[i];
	}
	output[len] = '\0';
	return output;
}

static size_t getBytesUsed(Arena *pool) {
	return pool->bytes_used;
}

static size_t getBytesAllocd(Arena *pool) {
	return pool->bytes_allocd;
}

static void printArenaInfo(Arena *pool) {
	printf("\nArena INFO - \n");
	printf("\tUSED: %zu, (%f MB)\n", getBytesUsed(pool), (double)getBytesUsed(pool) / (1024 * 1024));
	printf("\tALLOCD: %zu, (%f MB)\n", getBytesAllocd(pool), (double)getBytesAllocd(pool) / (1024 * 1024));
	return;
}

#endif // ARENA_H
