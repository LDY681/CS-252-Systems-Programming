#include "testing.h"
#include "malloc.h"

int main() {
  initialize_test(__FILE__);

  mallocing(ARENA_SIZE - 3 * ALLOC_HEADER_SIZE, print_status, false);
  mallocing(8, print_status, false);

  finalize_test();
}
