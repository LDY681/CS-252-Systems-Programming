
#include <stdlib.h>
#include <stdio.h>
#include "testing.h"

int allocations = 10000;

int
main( int argc, char **argv )
{
  initialize_test(__FILE__);

  tags_print(print_object);

  //test performs many small allocations, up to 2MB
  int i;
  for ( i = 0; i < allocations; i++ ) {
	  char * p1 = (char *) mallocing(100, NULL, false );
  }
  char * p2 = (char *) mallocing(64, print_status, false);
  char * p3 = (char *) mallocing(8, print_status, false);

  tags_print(print_object);

  finalize_test();
}

