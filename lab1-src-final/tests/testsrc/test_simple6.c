
#include <stdlib.h>
#include <stdio.h>
#include "testing.h"

int allocations =10000;

int
main( int argc, char **argv )
{
  initialize_test(__FILE__);

  tags_print(print_object);
  
  //this test requests many small blocks, and then frees every other block.
  //this should result in NO coalescing
  char * ptrs[15410];
  int i;
  for (i = 0; i < allocations; i++ ) {
	  char * p1 = (char *) mallocing(100, NULL, false );
	  ptrs[i] = p1;
  }
  tags_print(print_object);
  for(i = 0; i<allocations; i = i+2){
	  freeing(ptrs[i], 100, NULL, false);
  }
  tags_print(print_object);
  finalize_test();
}

