#include <stdlib.h>

#include "testing.h"

#define NALLOCS 10000
#define NLOOPS 4

int main() {
  initialize_test(__FILE__);

  for (int j = 0; j < NLOOPS; j++) {
    void * ptrs[NALLOCS];
    mallocing_loop(ptrs, 10000, NALLOCS, print_status, false);

    tags_print(print_object);

    for (int i = j % 2; i < NALLOCS; i+=2) {
      freeing(ptrs[i], 8, NULL, true);
    }
  }

  finalize_test();
}
