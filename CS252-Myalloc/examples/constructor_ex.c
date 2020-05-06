#include <stdio.h>

// Tell the compiler to run the init function before main
void init (void) __attribute__ ((constructor));

void init() {
  printf("Hello World!\n");
}

int main() {
  return 0;
}
