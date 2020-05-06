#include <stdlib.h>
#include <stdio.h>

#include "myMalloc.h"
#include "testing.h"

int main() {
  initialize_test(__FILE__);

  void * ptr = mallocing(0, print_status, false);
  void * ptr2 = mallocing(8, print_status, false);
  freeing(ptr2, 8, print_status, false);
  if (ptr == NULL) {
    printf("SUCCESS\n");
  } else {
    printf("Expected NULL for an allocation of 0 bytes\n");
  }

  finalize_test();
}
