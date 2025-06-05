# arena allocator
~~(not recommended due to lack of alignment)~~ <br>
updated to support alignment for cross-platform compatibility. speed untested <br>

simple arena allocator in C99 with allocation alignment using `alignof()` macro

[Inspired by [Untangling Lifetimes: The Arena Allocator](https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator)]

#### Interface
- Functions take `rkp_arena` references as arguments
- **`int rkp_arena_init(rkp_arena *arena)`** initializes an arena, typically stack-allocated as shown in the example below. Data pages come from `malloc`
- **`void *rkp_arena_alloc(rkp_arena *arena, size_t size, size_t alignment)`** is used like `malloc`
- **`void *rkp_arena_zalloc(rkp_arena *arena, size_t size, size_t alignment)`** is used like `malloc`, but zeroes using `memset`
- **`void *rkp_arena_grow_alloc(void *ptr, size_t old_size, size_t new_size, rkp_arena *arena)`** can grow allocations and copy contents, somewhat like `realloc` but only supporting increases in size
- **`char *rkp_arena_new_str(char *str, rkp_arena *arena)`** is like `strcpy` but allocates the new null-terminated string within the specified arena
- **`int rkp_arena_reset(rkp_arena *arena)`** will free all pages, and initialize a new page using the last page size.

#### Failure Behavior
- `exit()` is called if `malloc()` fails

#### Example
```
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
```

#### Settings
- **`#define MEMORY_HOG_FACTOR 8`** will be multiplied by requested allocation size to determine new page size, in case the current page is not large enough. Larger values result in fewer overall calls to `malloc`. Default value `8`
- **`#define DEF_BLOCK_SIZE 4096`** is the first page size upon initialization using `initArena()`. Default value `4096`

#### Miscellaneous
- **`rkp_arena_get_bytes_used`**, **`rkp_arena_get_bytes_allocd`**, and **`rkp_arena_print_info`** functions can be used to retrieve or print usage information at runtime
