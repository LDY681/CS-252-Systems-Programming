#include "testing.h"

int main() {
  initialize_test(__FILE__);

  mallocing(8, print_status, false);

  finalize_test();
}
