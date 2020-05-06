#include "testing.h"

int main() {
  initialize_test(__FILE__);

  void * ptr = mallocing(3, print_status, false);
  void * ptr2 = mallocing(2, print_status, false);
  mallocing(1, print_status, false);
  freeing(ptr, 3, print_status, false);
  freeing(ptr2, 2, print_status, false);

  finalize_test();
}
