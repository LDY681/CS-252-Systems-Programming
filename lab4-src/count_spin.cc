
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

volatile unsigned long lock = 0;

unsigned long 
test_and_set(volatile unsigned long * lock)
{
    unsigned long oldval = 1;
    asm volatile("xchgq %1,%0":"=r"(oldval):"m"(*lock),"0"(oldval):"memory");
    return oldval;
}

void
my_spin_lock( volatile unsigned long * lock )
{
	while (test_and_set(lock) != 0 ){
		pthread_yield();
	}
}

void
my_spin_unlock( volatile unsigned long * lock )
{
	*lock = 0;
}

int count;

void increment( int ntimes )
{
	for ( int i = 0; i < ntimes; i++ ) {
		my_spin_lock(&lock);
		int c;
		c = count;
		c = c + 1;
		count = c;
		my_spin_unlock(&lock);
	}
}

int main( int argc, char ** argv )
{
	int n = 10000000;
	pthread_t t1, t2;
        pthread_attr_t attr;

        pthread_attr_init( &attr );
        pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	printf("Start Test. Final count should be %d\n", 2 * n );

	// Create threads
	pthread_create( &t1, &attr, (void * (*)(void *)) increment, 
			(void *) n);

	pthread_create( &t2, &attr, (void * (*)(void *)) increment, 
			(void *) n);

	// Wait until threads are done
	pthread_join( t1, NULL );
	pthread_join( t2, NULL );

	if ( count != 2 * n ) {
		printf("\n****** Error. Final count is %d\n", count );
		printf("****** It should be %d\n", 2 * n );
	}
	else {
		printf("\n>>>>>> O.K. Final count is %d\n", count );
	}
}


