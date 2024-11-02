# arena allocator
single-header arena allocator in C99

[Inspired by [Untangling Lifetimes: The Arena Allocator](https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator)]

#### Interface
- Functions take `Arena` references as arguments
- `void *palloc(*pool, size)` is used like `malloc`
- `void *pzalloc(*pool, size)` is used like `malloc`, but zeroes like `calloc`
- `void *pGrowAlloc(*pool, *ptr, old_size, new_size)`  can grow allocations and copy contents, somewhat like `realloc` but only supporting increases in size
- `char *pNewStr(*pool, *str)` is like `strcpy` but allocates the new null-terminated string in the arena
- `int resetArena(*pool)` will free all pages in the linked-list, and re-initialize a new first block using the largest page size before reset

#### Example
```
#include "arena.h"
#include <inttypes.h>

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
	termArena(&p);
	return 0;
}
```

#### Settings
- `MEMORY_HOG_FACTOR` will be multiplied by requested allocation size to determine new page size, in case the current page is not large enough. Larger values result in fewer overall calls to `malloc`.
- `DEFAULT_PAGE_SIZE` is set initially at `4096` and is the first page size upon initialization using `initArena()`

#### Miscellaneous
- `int setAllocator(fn_ptr)` allows overriding `malloc` if desired. Replacement must be of the form `void *(*alloc_fn)(size_t)` (like `malloc`)
- `getBytesUsed(*pool)`, `getBytesAllocd(*pool)`, and `printArenaInfo(*pool)` can be used to retrieve or print usage information at runtime