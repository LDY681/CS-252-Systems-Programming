#include <string.h>

#include "testing.h"

int main() {
  initialize_test(__FILE__);

  void * ptr = mallocing(8, print_status, false);
  strcpy((char *) ptr - 8, "Invalid write");
  freeing(ptr, 8, print_status, false);

  finalize_test();
}
