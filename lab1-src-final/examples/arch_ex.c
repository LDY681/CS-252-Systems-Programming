#include <stdio.h>

int main() {
  /* The values depend on the compiler settings and target system 
   *
   * If the -m32 flag is used on the 64-bit machine the binary will be
   * compiled as 32-bit instead of 64-bit
   *
   * Make sure to always use sizeof() to calculated sizes instead of
   * hardcoding values like int=4, pointer=8, etc.
   */
  printf("int: %zu\n", sizeof(int));
  printf("long: %zu\n", sizeof(long));
  printf("void*: %zu\n", sizeof(void *));
}
