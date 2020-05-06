#include "testing.h"

int main() {
  initialize_test(__FILE__);

  void * ptr = mallocing(8, print_status, false);
  void * ptr2 = mallocing(8, print_status, false);
  freeing(ptr, 8, print_status, false);
  freeing(ptr2, 8, print_status, false);

  finalize_test();
}
