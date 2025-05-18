# arena allocator
(not recommended due to lack of alignment)

single-header arena allocator in C99

[Inspired by [Untangling Lifetimes: The Arena Allocator](https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator)]

#### Interface
- Functions take `Arena` references as arguments
- `int initArena(*arena)` initializes an arena, typically stack-allocated as shown in the example below
- `void *palloc(*arena, size)` is used like `malloc`
- `void *pzalloc(*arena, size)` is used like `malloc`, but zeroes using `memset`
- `void *pGrowAlloc(*arena, *ptr, old_size, new_size)`  can grow allocations and copy contents, somewhat like `realloc` but only supporting increases in size
- `char *pNewStr(*arena, *str)` is like `strcpy` but allocates the new null-terminated string in the arena
- `int resetArena(*arena)` will free all pages, and initialize a new page using the last page size

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
- `MEMORY_HOG_FACTOR` will be multiplied by requested allocation size to determine new page size, in case the current page is not large enough. Larger values result in fewer overall calls to `malloc`. Default value `8`
- `DEFAULT_PAGE_SIZE` is the first page size upon initialization using `initArena()`. Default value `4096`

#### Miscellaneous
- `int setAllocator(fn_ptr)` allows overriding `malloc` if desired. Replacement must be of the form `void *(*alloc_fn)(size_t)` (like `malloc`)
- `getBytesUsed(*arena)`, `getBytesAllocd(*arena)`, and `printArenaInfo(*arena)` can be used to retrieve or print usage information at runtime
