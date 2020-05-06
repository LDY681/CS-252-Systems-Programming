#ifndef TESTING_H
#define TESTING_H

#include "myMalloc.h"
#include "printing.h"

void ** mallocing_loop(void ** array, size_t size, size_t n, printFormatter pf, bool silent);
void * mallocing(size_t size, printFormatter pf, bool silent);
void freeing_loop(void ** array, size_t size, size_t n, printFormatter pf, bool silent);
void freeing(void * p, size_t size, printFormatter pf, bool silent);
void initialize_test();
void finalize_test();

#endif // TESTING_H
