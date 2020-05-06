#include "testing.h"
#include "malloc.h"

int main() {
  initialize_test(__FILE__);

  mallocing_loop(NULL, ARENA_SIZE - 3 * ALLOC_HEADER_SIZE, 3, print_object, false);
  mallocing(8, print_status, false);

  finalize_test();
}
