#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <stdbool.h>
#include <sys/types.h>

#define RELATIVE_POINTERS true

#ifndef ARENA_SIZE
// If not specified at compile time use the default arena size
#define ARENA_SIZE 4096
#endif

#ifndef N_LISTS
// If not specified at compile time use the default number of free lists
#define N_LISTS 59
#endif

/* Size of the header for an allocated block
 *
 * The size of the normal minus the size of the two free list pointers as
 * they are only maintained while block is free
 */
#define ALLOC_HEADER_SIZE (sizeof(header) - (2 * sizeof(header *)))

/* The minimum size request the allocator will service */
#define MIN_ALLOCATION 8

/**
 * @brief enum representing the allocation state of a block
 *
 * enums provice a method of specifying a set of named values
 * http://en.cppreference.com/w/c/language/enum
 */
enum state {
  UNALLOCATED = 0,
  ALLOCATED = 1,
  FENCEPOST = 2,
};

/*
 * The header contains all metadata about a block to be allocated
 * The size fields allow accessing the neighboring blocks in memory by
 * calculating their stating and ending addresses.
 *
 * When a block is free the free list pointer fields are set to the next
 * and previous blocks in the free list to allow faster traversal of the
 * freed blocks
 *
 * When a block is allocated the user's data starts by using the 16 bytes
 * reserved for the freelist pointers
 *
 * The zero length array at the end of the struct is the beginning of the
 * usable data in the block.
 *
 * FIELDS ALWAYS PRESENT
 * size_t size The size of the current block *including metadata*
 * size_t left_size The size of the block to the left (in memory)
 *
 * FIELDS PRESENT WHEN FREE
 * header * next The next block in the free list (only valid if free)
 * header * prev The previous block in the free list (only valid if free)
 *
 * FIELD PRESENT WHEN ALLOCATED
 * size_t[] canary magic value to detetmine if a block as been corrupted
 *
 * char[] data first byte of data pointed to by the list
 */
typedef struct header {
  size_t size_state;
  size_t left_size;
  union {
    // Used when the object is free
    struct {
      struct header * next;
      struct header * prev;
    };
    // Used when the object is allocated
    char data[0];
  };
} header;

// Helper functions for getting and storing size and state from header
// Since the size is a multiple of 8, the last 3 bits are always 0s.
// Therefore we use the 3 lowest bits to store the state of the object.
// This is going to save 8 bytes in all objects.

static inline size_t get_size(header * h) {
	return h->size_state & ~0x3;
}

static inline void set_size(header * h, size_t size) {
	h->size_state = size | (h->size_state & 0x3);
}

static inline enum  state get_state(header *h) {
	return (enum state) (h->size_state & 0x3);
}

static inline void set_state(header * h, enum state s) {
	h->size_state = (h->size_state & ~0x3) | s;
}

static inline void set_size_and_state(header * h, size_t size, enum state s) {
	h->size_state=(size & ~0x3)|(s &0x3);
}

#define MAX_OS_CHUNKS 1024

// Malloc interface
void * my_malloc(size_t size);
void * my_calloc(size_t nmemb, size_t size);
void * my_realloc(void * ptr, size_t size);
void my_free(void * p);

// Debug list verifitcation
bool verify();

// Helper to find a block's right neighbor
header * get_right_header(header * h);

/*
 * Global variables used in malloc that are needed by other C files
 *
 * extern tells the compiler that the variables exist in another file and
 * will be present when the final binary is linked
 */
extern void * base;
extern header freelistSentinels[];
extern char freelist_bitmap[];
extern header * osChunkList[];
extern size_t numOsChunks;

#endif // MY_MALLOC_H
