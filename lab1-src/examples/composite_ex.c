#include <stdio.h>

/*
 * The struct contains an int AND an float
 */
struct s {
  int i;
  float f;
};

/*
 * The union contains an int OR a float
 */
union u {
  int i;
  float f;
};

int main() {
  // We can store both an int and a float and use both of them
  // This makes the size of the struct the size of BOTH an int and a float
  struct s s1;
  printf("sizeof(s1): %zu\n", sizeof(s1));
  s1.i = 42;
  s1.f = 42.0;
  printf("s1: {i: %d, f: %f}\n", s1.i, s1.f);
  puts("");


  // We can store only an int OR a float, if we update one field we modify
  // the value of the other this is because the 4 bytes in the union is either
  // used to store the int value or the float value, but there are only 4 bytes
  // total so only one may be stored
  union u u1;
  printf("sizeof(u1): %zu\n", sizeof(u1));
  u1.i = 42;
  printf("u1: {i: %d, f: %lf}\n", u1.i, u1.f);
  u1.f = 42.0;
  printf("u1: {i: %d, f: %lf}\n", u1.i, u1.f);
}
