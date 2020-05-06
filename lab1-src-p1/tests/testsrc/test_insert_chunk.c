#include <unistd.h>

#include "testing.h"
#include "malloc.h"

int main() {
  initialize_test(__FILE__);

  mallocing(ARENA_SIZE - 3 * ALLOC_HEADER_SIZE - 48, print_status, false);
  printf("Calling sbrk to allocate memory between malloc's chunks\n");
  sbrk(1024);
  mallocing(ARENA_SIZE - 3 * ALLOC_HEADER_SIZE - 48, print_status, false);

  finalize_test();
}
