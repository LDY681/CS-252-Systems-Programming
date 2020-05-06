
#include <dlfcn.h>
#include <link.h>
#include <errno.h>
#include <stdio.h>
#include<stdlib.h>
typedef void (*httprunfunc)(int ssock, const char* querystring);

int
main(int argc, char ** argv )
{

  // Example of how to use dlopen and dlsym

  // Opening 
  void * lib = dlopen( "./hello.so", RTLD_LAZY );

  if ( lib == NULL ) {
    fprintf( stderr, "./hello.so not found\n");
    perror( "dlopen");
    exit(1);
  }

  // Get function to print hello
  httprunfunc hello_httprun;

  hello_httprun = (httprunfunc) dlsym( lib, "httprun");
  if ( hello_httprun == NULL ) {
    perror( "dlsym: httprun not found:");
    exit(1);
  }

  // Call the function
  hello_httprun( 1, "a=b&c=d");
}

