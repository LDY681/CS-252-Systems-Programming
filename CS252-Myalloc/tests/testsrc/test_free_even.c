#include "testing.h"

#define NALLOCS 10

int main() {
  initialize_test(__FILE__);
  void * ptrs[NALLOCS];

  mallocing_loop(ptrs, 8, NALLOCS, print_status, false);
  for (int i = 0; i < NALLOCS; i+=2) {
    freeing(ptrs[i], 8, print_status, false);
  }

  finalize_test();
}
