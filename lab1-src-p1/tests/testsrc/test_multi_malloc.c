#include <stdlib.h>

#include "testing.h"

int main() {
  initialize_test(__FILE__);

  mallocing_loop(NULL, 64, 5, print_status, false);

  finalize_test();
}
