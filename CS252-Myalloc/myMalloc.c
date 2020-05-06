#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "myMalloc.h"
#include "printing.h"

/* Due to the way assert() prints error messges we use out own assert function
 * for deteminism when testing assertions
 */
#ifdef TEST_ASSERT
  inline static void assert(int e) {
    if (!e) {
      const char * msg = "Assertion Failed!\n";
      write(2, msg, strlen(msg));
      exit(1);
    }
  }
#else
  #include <assert.h>
#endif

/*
 * Mutex to ensure thread safety for the freelist
 */
static pthread_mutex_t mutex;

/*
 * Array of sentinel nodes for the freelists
 */
header freelistSentinels[N_LISTS];

/*
 * Pointer to the second fencepost in the most recently allocated chunk from
 * the OS. Used for coalescing chunks
 */
header * lastFencePost;

/*
 * Pointer to maintian the base of the heap to allow printing based on the
 * distance from the base of the heap
 */ 
void * base;

/*
 * List of chunks allocated by  the OS for printing boundary tags
 */
header * osChunkList [MAX_OS_CHUNKS];
size_t numOsChunks = 0;

/*
 * direct the compiler to run the init function before running main
 * this allows initialization of required globals
 */
static void init (void) __attribute__ ((constructor));

// Helper functions for manipulating pointers to headers
static inline header * get_header_from_offset(void * ptr, ptrdiff_t off);
static inline header * get_left_header(header * h);
static inline header * ptr_to_header(void * p);

//Customized Helper Function

//Check if a free block is in final freelist, if not, remove it
bool freeblock_check(header *hdr);	
//Call this if a block is not in freelist and you want to insert it
void insert_freelist(header *hdr);	
//The size to be allocated, including size of header
size_t calc_allocate_size(size_t raw_size);
//Get the index of appropriate freelist
int get_list_index(size_t size);

// Helper functions for allocating more memory from the OS
static inline void initialize_fencepost(header * fp, size_t left_size);
static inline void insert_os_chunk(header * hdr);
static inline void insert_fenceposts(void * raw_mem, size_t size);
static header * allocate_chunk(size_t size);

// Helper functions for freeing a block
static inline void deallocate_object(void * p);

// Helper functions for allocating a block
static inline header * allocate_object(size_t raw_size);

// Helper functions for verifying that the data structures are structurally 
// valid
static inline header * detect_cycles();
static inline header * verify_pointers();
static inline bool verify_freelist();
static inline header * verify_chunk(header * chunk);
static inline bool verify_tags();

static void init();

static bool isMallocInitialized;

/**
 * @brief Helper function to retrieve a header pointer from a pointer and an 
 *        offset
 *
 * @param ptr base pointer
 * @param off number of bytes from base pointer where header is located
 *
 * @return a pointer to a header offset bytes from pointer
 */
static inline header * get_header_from_offset(void * ptr, ptrdiff_t off) {
	return (header *)((char *) ptr + off);
}

/**
 * @brief Helper function to get the header to the right of a given header
 *
 * @param h original header
 *
 * @return header to the right of h
 */
header * get_right_header(header * h) {
	return get_header_from_offset(h, get_size(h));
}

/**
 * @brief Helper function to get the header to the left of a given header
 *
 * @param h original header
 *
 * @return header to the right of h
 */
inline static header * get_left_header(header * h) {
  return get_header_from_offset(h, -h->left_size);
}

/**
 * @brief Fenceposts are marked as always allocated and may need to have
 * a left object size to ensure coalescing happens properly
 *
 * @param fp a pointer to the header being used as a fencepost
 * @param left_size the size of the object to the left of the fencepost
 */
inline static void initialize_fencepost(header * fp, size_t left_size) {
	set_state(fp,FENCEPOST);
	set_size(fp, ALLOC_HEADER_SIZE);
	fp->left_size = left_size;
}

/**
 * @brief Helper function to maintain list of chunks from the OS for debugging
 *
 * @param hdr the first fencepost in the chunk allocated by the OS
 */
inline static void insert_os_chunk(header * hdr) {
  if (numOsChunks < MAX_OS_CHUNKS) {
    osChunkList[numOsChunks++] = hdr;
  }
}

/**
 * @brief given a chunk of memory insert fenceposts at the left and 
 * right boundaries of the block to prevent coalescing outside of the
 * block
 *
 * @param raw_mem a void pointer to the memory chunk to initialize
 * @param size the size of the allocated chunk
 */
inline static void insert_fenceposts(void * raw_mem, size_t size) {
  // Convert to char * before performing operations
  char * mem = (char *) raw_mem;

  // Insert a fencepost at the left edge of the block
  header * leftFencePost = (header *) mem;
  initialize_fencepost(leftFencePost, ALLOC_HEADER_SIZE);

  // Insert a fencepost at the right edge of the block
  header * rightFencePost = get_header_from_offset(mem, size - ALLOC_HEADER_SIZE);
  initialize_fencepost(rightFencePost, size - 2 * ALLOC_HEADER_SIZE);
}

/**
 * @brief Allocate another chunk from the OS and prepare to insert it
 * into the free list
 *
 * @param size The size to allocate from the OS
 *
 * @return A pointer to the allocable block in the chunk (just after the 
 * first fencpost)
 */
static header * allocate_chunk(size_t size) {
  void * mem = sbrk(size);
  
  insert_fenceposts(mem, size);
  header * hdr = (header *) ((char *)mem + ALLOC_HEADER_SIZE);
  set_state(hdr, UNALLOCATED);
  set_size(hdr, size - 2 * ALLOC_HEADER_SIZE);
  hdr->left_size = ALLOC_HEADER_SIZE;
  return hdr;
}


size_t calc_allocate_size(size_t raw_size) {
	size_t rounded_size = ((raw_size + ALLOC_HEADER_SIZE + 7 )/8)*8 ;
	if (rounded_size < sizeof(header)) {
		return sizeof(header);
	}
	return rounded_size;
}

int get_list_index(size_t size) {
	if (size > (N_LISTS - 1) * 8) {
		return N_LISTS - 1;
	}
		return (size / 8) - 1;
}

bool freeblock_check(header* hdr) {

	int listidx = get_list_index(get_size(hdr) - ALLOC_HEADER_SIZE);

	//If unallocated block is not in final freelist, remove it and insert again
	if (listidx != N_LISTS - 1) {
		return false;
	}

	return true;

}

void insert_freelist(header * hdr){
	//insert to appropirate freelist
	int listidx = get_list_index(get_size(hdr) - ALLOC_HEADER_SIZE);
	header * freelist = &freelistSentinels[listidx];

	//If freelist is empty, freelist->prev = header
	if (freelist->next == freelist) {
		freelist->prev = hdr;
	}

	freelist->next->prev = hdr;
	hdr->prev = freelist;
	hdr->next = freelist->next;
	freelist->next = hdr;

	return;
}

/**
 * @brief Helper allocate an object given a raw request size from the user
 *
 * @param raw_size number of bytes the user needs
 *
 * @return A block satisfying the user's request
 */
static inline header * allocate_object(size_t raw_size) {
	if (raw_size == 0){
		return NULL;
	}

	//Calculate the size to be allocated, including size of header
	size_t allocated_size = calc_allocate_size(raw_size);

	//Find the appropriate free list to look for a block to allocate
	int listidx = get_list_index(allocated_size - ALLOC_HEADER_SIZE) ;

	header * freelist = &freelistSentinels[listidx];
	
	//Use first non-empty list
	while (freelist->next == freelist && listidx < N_LISTS -1){
		listidx++;
		freelist = &freelistSentinels[listidx];
	}
	
	header *current;
	for (current = freelist->next; current != freelist || listidx != N_LISTS -1 ; current = current->next){
		size_t current_size = get_size (current);
		
		//Case1: No need to break the chunk into two parts
		if (current_size == allocated_size || (current_size - allocated_size) < sizeof(header) ){

			//Update current block's state to ALLOCATED
			set_state(current, ALLOCATED);

			//Remove current block from freelist
			current->next->prev = current->prev;
			current->prev->next = current->next;

			//Return current->data
			return (header*)current->data;	

		} //Case2: Need to break the chunk into two
		else{

			bool fl = freeblock_check(current);

			//Update current block's size
			set_size(current, get_size(current) - allocated_size);
			
			//Get to the point of split & make newHeader
			char * newpointer = (char *)current + get_size(current);
			header * newHeader = (header *)newpointer;

			//Assign values to newheader
			set_size(newHeader, allocated_size);
			newHeader->left_size = get_size(current);

			//Update newHeader's state to ALLOCATED
			set_state(newHeader, ALLOCATED);
			
			//Get the rightHeader
			char * rightpointer = (char *)newHeader + get_size(newHeader);
			header * rightHeader = (header*)rightpointer;
			
			//Update rightHeader's left_size
			rightHeader->left_size = get_size(newHeader);
			
			//Call this if a block is in freelist and you did some modfications on it
			if ( !(fl == true && get_list_index(get_size(current) - ALLOC_HEADER_SIZE) == N_LISTS -1) ) {
				current->next->prev = current->prev;
				current->prev->next = current->next;
				insert_freelist(current);
			}
			
			//Return newheader->data
			return (header*)newHeader->data;
		}
	}

	//Case3: Memory requested cannot be fulfilled, allocate new memory chunk

	//Variable declaration & their positions 
	//last_block | last_fencepost || left_fencepost | newHeader | right_fencepost

	header * newHeader = allocate_chunk(ARENA_SIZE);
	header * left_fencepost = get_header_from_offset(newHeader, -ALLOC_HEADER_SIZE);
	header * right_fencepost = get_header_from_offset(newHeader, get_size(newHeader));

	//***last_fencepost is the left header of left_fencepost, not the actual last_fencepost*** 
	//***However lastFencePost, the global variable is***
	header * last_fencepost = get_header_from_offset(left_fencepost, -ALLOC_HEADER_SIZE);
		
	//CaseA: If two chunks are adjacent, coalesce them
	if (last_fencepost == lastFencePost) {

		//CaseAA: If last_block in previous chunk is UNALLOCATED, coalesce them
		header * last_block = get_left_header(last_fencepost);
		if (get_state(last_block) == UNALLOCATED) {

			bool fl = freeblock_check(last_block);


			//Update last_block's size = last_block + newHeader + 2*fencepost
			set_size(last_block, get_size(last_block) + get_size(newHeader) + 2 * ALLOC_HEADER_SIZE);

			//Update the state of last_block
			set_state(last_block, UNALLOCATED);
			
			//Update right_fencepost's left_size
			right_fencepost->left_size = get_size(last_block);

			//Call this if a block is in freelist and you did some modfications on it
			if (!(fl == true && get_list_index(get_size(last_block) - ALLOC_HEADER_SIZE) == N_LISTS - 1)) {
				last_block->next->prev = last_block->prev;
				last_block->prev->next = last_block->next;
				insert_freelist(last_block);
			}

			//Update lastFencePost
			lastFencePost = right_fencepost;

			//Call allocate again
			return allocate_object(raw_size);
			
		
		}	//Case AB: If last_block is ALLOCATED
		else {

			//Update the size of last_fencepost = newHeader + 2*fencepost
			set_size(last_fencepost, get_size(newHeader) + 2 * ALLOC_HEADER_SIZE);

			//Update right_fencepost's left_size
			right_fencepost->left_size = get_size(last_fencepost);

			//Update lasr_fencepost's state
			set_state(last_fencepost, UNALLOCATED);

			//Call this if a block is not in freelist and you want to insert it
			insert_freelist(last_fencepost);

			//Update lastFencePost 
			lastFencePost = right_fencepost;

			//Call allocate again
			return allocate_object(raw_size);
		}

	}	//CaseB: If two chunks are not adjacent
	else {

		//Call this if a block is not in freelist and you want to insert it
		insert_freelist(newHeader);

		//Update lastFencePost & insert last_block to os chunk
		lastFencePost = right_fencepost;
		insert_os_chunk(left_fencepost);

		//Call allocate again
		return allocate_object(raw_size);
	}

}
/**
 * @brief Helper to get the header from a pointer allocated with malloc
 *
 * @param p pointer to the data region of the block
 *
 * @return A pointer to the header of the block
 */
static inline header * ptr_to_header(void * p) {
  return (header *)((char *) p - ALLOC_HEADER_SIZE); //sizeof(header));
}

/**
 * @brief Helper to manage deallocation of a pointer returned by the user
 *
 * @param p The pointer returned to the user by a call to malloc
 */
static inline void deallocate_object(void * p) {
	
	//If pointer is NULL, no-op 
	if (p == NULL) {
		return;
	}

	//Make flags, 0 = allocated or fencepost, 1 = unallocated
	int leftflag = 0;
	int rightflag = 0;

	//Make current header from void *p
	header * currHeader = get_header_from_offset((char*)p, -ALLOC_HEADER_SIZE);

	//Check for double_free
	if (get_state(currHeader) == UNALLOCATED) {
		printf("Double Free Detected\n");
		printf("Assertion Failed!\n");
		assert(true);
		exit(1);
	}

	set_state(currHeader, UNALLOCATED);

	//Make neighbor headers
	header * leftHeader = get_left_header(currHeader);
	header * rightHeader = get_right_header(currHeader);
	//Check flag
	if (get_state(leftHeader) == UNALLOCATED) {
		leftflag = 1;
	}
	if (get_state(rightHeader) == UNALLOCATED) {
		rightflag = 1;
	}

	//Case1: Coalesce with both sides
	if (leftflag && rightflag) {

		bool fl = freeblock_check(leftHeader);
		set_state(leftHeader, UNALLOCATED);

		//Update left header's size
		set_size(leftHeader, get_size(leftHeader) + get_size(currHeader) + get_size(rightHeader));

		//Remove rightheader from the freelist
		rightHeader->next->prev = rightHeader->prev;
		rightHeader->prev->next = rightHeader->next;

		//Update rightrightheader's left_size
		header * rightrightheader = get_right_header(rightHeader);
		rightrightheader->left_size = get_size(leftHeader);

		//Call this if a block is in freelist and you did some modfications on it
		if (!(fl == true && get_list_index(get_size(leftHeader)-ALLOC_HEADER_SIZE) == N_LISTS - 1)) {
			leftHeader->next->prev = leftHeader->prev;
			leftHeader->prev->next = leftHeader->next;
			insert_freelist(leftHeader);
		}
	
	}	//Case2: Coalesce with left
	else if (leftflag) {

		bool fl = freeblock_check(leftHeader);

		set_state(leftHeader, UNALLOCATED);

		//Update left header's size
		set_size(leftHeader, get_size(leftHeader) + get_size(currHeader));

		//Update right header's left_size
		rightHeader->left_size = get_size(leftHeader);

		//Call this if a block is in freelist and you did some modfications on it
		if (!(fl == true && get_list_index(get_size(leftHeader) - ALLOC_HEADER_SIZE) == N_LISTS - 1)) {
			leftHeader->next->prev = leftHeader->prev;
			leftHeader->prev->next = leftHeader->next;
			insert_freelist(leftHeader);
		}

	}	//Case3: Coalesce with the right
	else if (rightflag) {

		//Change the currHeader's state
		set_state(currHeader, UNALLOCATED);

		//Update rightrightheader's left_size
		header * rightrightHeader = get_right_header(rightHeader);
		rightrightHeader->left_size = get_size(currHeader) + get_size(rightHeader);

		//Update currHeader's size
		set_size(currHeader, get_size(currHeader) + get_size(rightHeader));

		//Remove rightheader from the freelist
		rightHeader->next->prev = rightHeader->prev;
		rightHeader->prev->next = rightHeader->next;

		//Call this if a block is not in freelist and you want to insert it
		insert_freelist(currHeader);

	}	//Case4: No colescing
	else {
		//Change currHeader's state
		set_state(currHeader, UNALLOCATED);

		//Call this if a block is not in freelist and you want to insert it
		insert_freelist(currHeader);

	}

	return;
	(void) p;
	assert(false);
	exit(1);

}

/**
 * @brief Helper to detect cycles in the free list
 * https://en.wikipedia.org/wiki/Cycle_detection#Floyd's_Tortoise_and_Hare
 *
 * @return One of the nodes in the cycle or NULL if no cycle is present
 */
static inline header * detect_cycles() {
  for (int i = 0; i < N_LISTS; i++) {
    header * freelist = &freelistSentinels[i];
    for (header * slow = freelist->next, * fast = freelist->next->next; 
         fast != freelist; 
         slow = slow->next, fast = fast->next->next) {
      if (slow == fast) {
        return slow;
      }
    }
  }
  return NULL;
}

/**
 * @brief Helper to verify that there are no unlinked previous or next pointers
 *        in the free list
 *
 * @return A node whose previous and next pointers are incorrect or NULL if no
 *         such node exists
 */
static inline header * verify_pointers() {
  for (int i = 0; i < N_LISTS; i++) {
    header * freelist = &freelistSentinels[i];
    for (header * cur = freelist->next; cur != freelist; cur = cur->next) {
      if (cur->next->prev != cur || cur->prev->next != cur) {
        return cur;
      }
    }
  }
  return NULL;
}

/**
 * @brief Verify the structure of the free list is correct by checkin for 
 *        cycles and misdirected pointers
 *
 * @return true if the list is valid
 */
static inline bool verify_freelist() {
  header * cycle = detect_cycles();
  if (cycle != NULL) {
    fprintf(stderr, "Cycle Detected\n");
    print_sublist(print_object, cycle->next, cycle);
    return false;
  }

  header * invalid = verify_pointers();
  if (invalid != NULL) {
    fprintf(stderr, "Invalid pointers\n");
    print_object(invalid);
    return false;
  }

  return true;
}

/**
 * @brief Helper to verify that the sizes in a chunk from the OS are correct
 *        and that allocated node's canary values are correct
 *
 * @param chunk AREA_SIZE chunk allocated from the OS
 *
 * @return a pointer to an invalid header or NULL if all header's are valid
 */
static inline header * verify_chunk(header * chunk) {
	if (get_state(chunk) != FENCEPOST) {
		fprintf(stderr, "Invalid fencepost\n");
		print_object(chunk);
		return chunk;
	}
	
	for (; get_state(chunk) != FENCEPOST; chunk = get_right_header(chunk)) {
		if (get_size(chunk)  != get_right_header(chunk)->left_size) {
			fprintf(stderr, "Invalid sizes\n");
			print_object(chunk);
			return chunk;
		}
	}
	
	return NULL;
}

/**
 * @brief For each chunk allocated by the OS verify that the boundary tags
 *        are consistent
 *
 * @return true if the boundary tags are valid
 */
static inline bool verify_tags() {
  for (size_t i = 0; i < numOsChunks; i++) {
    header * invalid = verify_chunk(osChunkList[i]);
    if (invalid != NULL) {
      return invalid;
    }
  }

  return NULL;
}

/**
 * @brief Initialize mutex lock and prepare an initial chunk of memory for allocation
 */
static void init() {
  // Initialize mutex for thread safety
  pthread_mutex_init(&mutex, NULL);

#ifdef DEBUG
  // Manually set printf buffer so it won't call malloc when debugging the allocator
  setvbuf(stdout, NULL, _IONBF, 0);
#endif // DEBUG

  // Allocate the first chunk from the OS
  header * block = allocate_chunk(ARENA_SIZE);

  header * prevFencePost = get_header_from_offset(block, -ALLOC_HEADER_SIZE);
  insert_os_chunk(prevFencePost);

  lastFencePost = get_header_from_offset(block, get_size(block));

  // Set the base pointer to the beginning of the first fencepost in the first
  // chunk from the OS
  base = ((char *) block) - ALLOC_HEADER_SIZE; //sizeof(header);

  // Initialize freelist sentinels
  for (int i = 0; i < N_LISTS; i++) {
    header * freelist = &freelistSentinels[i];
    freelist->next = freelist;
    freelist->prev = freelist;
  }

  // Insert first chunk into the free list
  header * freelist = &freelistSentinels[N_LISTS - 1];
  freelist->next = block;
  freelist->prev = block;
  block->next = freelist;
  block->prev = freelist;
}

/* 
 * External interface
 */
void * my_malloc(size_t size) {
  pthread_mutex_lock(&mutex);
  header * hdr = allocate_object(size); 
  pthread_mutex_unlock(&mutex);
  return hdr;
}

void * my_calloc(size_t nmemb, size_t size) {
  return memset(my_malloc(size * nmemb), 0, size * nmemb);
}

void * my_realloc(void * ptr, size_t size) {
  void * mem = my_malloc(size);
  memcpy(mem, ptr, size);
  my_free(ptr);
  return mem; 
}

void my_free(void * p) {
  pthread_mutex_lock(&mutex);
  deallocate_object(p);
  pthread_mutex_unlock(&mutex);
}

bool verify() {
  return verify_freelist() && verify_tags();
}
