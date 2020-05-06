CC = gcc
CFLAGS = -g -std=gnu11 -Wall -Wextra
LDFLAGS =

.PHONY: examples
examples: composite_ex constructor_ex

composite_ex: composite_ex.c
	${CC} -o composite_ex composite_ex.c

constructor_ex: constructor_ex.c
	${CC} -o constructor_ex constructor_ex.c

arch_m32: arch_ex.c
	${CC} -m32 -o arch_m32 arch_ex.c

arch_m64: arch_ex.c
	${CC} -o arch_m64 arch_ex.c


.PHONY: clean
clean: 
	rm -f composite_ex constructor_ex arch_m32 arch_m64
