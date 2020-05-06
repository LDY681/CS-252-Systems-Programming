CC = gcc
#CFLAGS = -std=gnu11 -Wall -Wextra -I..
CFLAGS = -std=gnu11 -I.. -g -DDEBUG
LDFLAGS = -lpthread
TEST_SRC_DIR = ./testsrc
TEST_BIN_DIR = .
MALLOC_FILES = ../myMalloc.c ../testing.c ../printing.c
MALLOC_HEADERS = ../myMalloc.h ../testing.h ../printing.h

.PHONY: all
all: simple malloc free robustness other

.PHONY: simple
simple: test_simple0 test_simple1 test_simple2 test_simple3 test_simple4 test_simple5 test_simple6

.PHONY: malloc
malloc: test_split test_exact test_multi_malloc test_coalesce_chunk_insert test_coalesce_chunk_coalesce test_insert_chunk test_malloc_large

.PHONY: free
free: test_free_insert test_free_left test_free_right test_free_both test_free_even test_free_odd 

.PHONY: robustness
robustness: test_all_lists test_large test_random test_random_sizes test_very_large

.PHONY: other
other: test_verify test_locks test_corrupted_canary test_malloc_zero test_malloc_too_large test_free_null test_double_free test_out_of_ram

# To add additional tests list the test under *all* above
#
# Fill in the test binary name, and c file name
#
# You can set the arena size by updating the variable ARENA_SIZE listed below
#
# <your_test_name>: ${TEST_SRC_DIR}/<your_test_c_file>.c ${MALLOC_FILES} ${MALLOC_HEADERS}
# 	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_simple0: ${TEST_SRC_DIR}/test_simple0.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_simple1: ${TEST_SRC_DIR}/test_simple1.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_simple2: ${TEST_SRC_DIR}/test_simple2.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_simple3: ${TEST_SRC_DIR}/test_simple3.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_simple4: ${TEST_SRC_DIR}/test_simple4.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_simple5: ${TEST_SRC_DIR}/test_simple5.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_simple6: ${TEST_SRC_DIR}/test_simple6.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_verify: ${TEST_SRC_DIR}/test_verify.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_split: ${TEST_SRC_DIR}/test_split.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_exact: ${TEST_SRC_DIR}/test_exact.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_multi_malloc: ${TEST_SRC_DIR}/test_multi_malloc.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_insert_chunk: ${TEST_SRC_DIR}/test_insert_chunk.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_coalesce_chunk_insert: ${TEST_SRC_DIR}/test_coalesce_chunk_insert.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_coalesce_chunk_coalesce: ${TEST_SRC_DIR}/test_coalesce_chunk_coalesce.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_malloc_large: ${TEST_SRC_DIR}/test_malloc_large.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=2147483648 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_free_insert: ${TEST_SRC_DIR}/test_free_insert.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_free_left: ${TEST_SRC_DIR}/test_free_insert.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_free_right: ${TEST_SRC_DIR}/test_free_right.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_free_both: ${TEST_SRC_DIR}/test_free_both.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_free_even: ${TEST_SRC_DIR}/test_free_even.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_free_odd: ${TEST_SRC_DIR}/test_free_odd.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_locks: ${TEST_SRC_DIR}/test_locks.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_corrupted_canary: ${TEST_SRC_DIR}/test_corrupted_canary.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -DTEST_ASSERT -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_malloc_zero: ${TEST_SRC_DIR}/test_malloc_zero.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_malloc_too_large: ${TEST_SRC_DIR}/test_malloc_too_large.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_free_null: ${TEST_SRC_DIR}/test_free_null.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_double_free: ${TEST_SRC_DIR}/test_double_free.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -DTEST_ASSERT -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_out_of_ram: ${TEST_SRC_DIR}/test_out_of_ram.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=1024 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_all_lists: ${TEST_SRC_DIR}/test_all_lists.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=2147483648 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_large: ${TEST_SRC_DIR}/test_large.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=2147483648 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_random: ${TEST_SRC_DIR}/test_random.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=2147483648 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_random_sizes: ${TEST_SRC_DIR}/test_random_sizes.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=2147483648 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

test_very_large: ${TEST_SRC_DIR}/test_random_sizes.c ${MALLOC_FILES} ${MALLOC_HEADERS}
	${CC} ${CFLAGS} ${LDFLAGS} -DARENA_SIZE=2147483648 -o ${TEST_BIN_DIR}/$@ ${TEST_SRC_DIR}/$@.c ${MALLOC_FILES}

.PHONY: clean
clean: 
	rm -f test_*
