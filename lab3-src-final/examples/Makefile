#
# CS252 - Shell Project Example Files
#
#Use GNU compiler
cc= gcc
CC= g++
ccFLAGS= -g -std=c11
CCFLAGS= -g -std=c++17
WARNFLAGS= -Wall -Wextra -pedantic

all: cat_grep ctrl-c regular keyboard-example read-line-example

cat_grep: cat_grep.cc
	$(CC) $(CCFLAGS) $(WARNFLAGS) -o cat_grep cat_grep.cc

ctrl-c: ctrl-c.cc
	$(CC) $(CCFLAGS) $(WARNFLAGS) -o ctrl-c ctrl-c.cc

regular: regular.cc
	$(CC) $(CCFLAGS) $(WARNFLAGS) -o regular regular.cc 

tty-raw-mode.o: ../tty-raw-mode.c
	$(cc) $(ccFLAGS) $(WARNFLAGS) -c ../tty-raw-mode.c

read-line.o: read-line.c
	$(cc) $(ccFLAGS) $(WARNFLAGS) -c read-line.c

keyboard-example: keyboard-example.c tty-raw-mode.o
	$(cc) $(ccFLAGS) $(WARNFLAGS) -o keyboard-example keyboard-example.c tty-raw-mode.o

read-line-example: read-line-example.c tty-raw-mode.o read-line.o
	$(cc) $(ccFLAGS) $(WARNFLAGS) -o read-line-example read-line-example.c tty-raw-mode.o read-line.o

.PHONY: clean
clean:
	rm -f ctrl-c regular cat_grep keyboard-example read-line-example *.o

