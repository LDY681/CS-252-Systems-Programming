#include "testing.h"
#include "malloc.h"

int main() {
  initialize_test(__FILE__);

  mallocing(ARENA_SIZE - 3 * ALLOC_HEADER_SIZE /*sizeof(header)*/, print_status, false);

  finalize_test();
}
