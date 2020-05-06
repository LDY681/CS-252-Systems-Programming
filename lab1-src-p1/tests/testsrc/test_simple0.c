
#include <stdlib.h>
#include <stdio.h>
#include "testing.h"

int
main( int argc, char **argv )
{
  initialize_test(__FILE__);
  printf("\n---- Running test0 ---\n");

  //simple test that does nothing requiring malloc()
  int i = 2;
  int j = 3;
  int k = i + j;

  finalize_test();
}
