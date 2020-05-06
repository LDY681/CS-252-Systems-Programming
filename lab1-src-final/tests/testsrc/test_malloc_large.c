#include "testing.h"

#define NALLOCS 20000

int main() {
  initialize_test(__FILE__);

  void * ptrs[NALLOCS];
  mallocing_loop(ptrs, 16384, NALLOCS, print_status, false);

  tags_print(print_object);

  finalize_test();
}
