#include <stdlib.h>
#include <stdio.h>

#include "myMalloc.h"
#include "testing.h"

int main() {
  initialize_test(__FILE__);

  printf("Freeing a null pointer\n");
  freeing(NULL, 0, print_status, true);
  printf("No crash when freeing a NULL pointer\n\n");

  printf("Verify that malloc still works after freeing NULL\n");
  void * ptr = mallocing(8, print_status, false);
  freeing(ptr, 8, print_status, false);

  finalize_test();
}
