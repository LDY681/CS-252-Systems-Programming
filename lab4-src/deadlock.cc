
#include <pthread.h>
#include <stdio.h>

int balance1 = 100000000;
int balance2 = 100000000;

pthread_mutex_t m1;
pthread_mutex_t m2;

void transfer1to2(int amount)
{
	pthread_mutex_lock(&m1);	
	balance1 -= amount;
	pthread_mutex_unlock(&m1);

	pthread_mutex_lock(&m2);
	balance2 += amount;
	pthread_mutex_unlock(&m2);
}

void transfer2to1( int amount )
{
	pthread_mutex_lock(&m2);
	balance2 -= amount;
	pthread_mutex_unlock(&m2);

	pthread_mutex_lock(&m1);
	balance1 += amount;
	pthread_mutex_unlock(&m1);
}

void thr1(int number) {
	for (int i = 0; i < number; i++) {
	        if ( i % 1000000 == 0  ) printf("transfer1to2 %d times\n", i);
		transfer1to2(100);	
	}
}

void thr2(int number) {
	for (int i = 0; i < number; i++) {
	        if ( i % 1000000 == 0  ) printf("transfer2to1 %d times\n", i);
		transfer2to1(100);	
	}
}

int main( int argc, char ** argv )
{
	int n = 10000000;
	pthread_t t1, t2;
        pthread_attr_t attr;

        pthread_attr_init( &attr );
        pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	pthread_mutex_init(&m1, NULL);
	pthread_mutex_init(&m2, NULL);


	// Create threads
	pthread_create( &t1, &attr, (void * (*)(void *)) thr1, 
			(void *) 10000000);

	pthread_create( &t2, &attr, (void * (*)(void *)) thr2, 
			(void *) 10000000);

	// Wait until threads are done
	pthread_join( t1, NULL );
	pthread_join( t2, NULL );

	if ( balance1 + balance2 !=  200000000) {
		printf("\n****** Error. Final total is %d\n", balance1 + balance2 );
		printf("****** It should be %d\n", 200000000 );
	}
	else {
		printf("\n>>>>>> O.K. Final total is %d\n", balance1 + balance2);
	}
}


