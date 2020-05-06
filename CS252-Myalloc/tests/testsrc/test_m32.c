#include <stdio.h>

#include "testing.h"

#define NALLOCS 10

int main() {
  printf("Testing the allocator compiled as a 32-bit Binary\n");

  initialize_test(__FILE__);
  void * ptrs[NALLOCS];

  mallocing_loop(ptrs, 8, NALLOCS, print_status, false);
  for (int i = NALLOCS - 1; i > 0; i-=2) {
    freeing(ptrs[i], 8, print_status, false);
  }

  finalize_test();
}
