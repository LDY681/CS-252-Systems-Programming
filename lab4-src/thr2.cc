
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void printC( const char * s )
{
	while (1) {
		printf("%s", s );
	}
}

int main( int argc, char ** argv )
{
	pthread_t t1, t2;
	pthread_attr_t attr;

	pthread_attr_init( &attr );
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	printC("C");
	pthread_create( &t1, &attr, (void * (*)(void *)) printC, (void *) "A" );
	pthread_create( &t2, &attr, (void * (*)(void *)) printC, (void *) "B" );

}
