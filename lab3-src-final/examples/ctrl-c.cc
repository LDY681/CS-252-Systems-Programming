
//
// Example of how to ignore ctrl-c
//

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>


extern "C" void disp( int sig )
{
	fprintf( stderr, "\nsig:%d      Ouch!\n", sig);
}

int main()
{
	printf( "Type ctrl-c or \"exit\"\n");
    
    struct sigaction sa;
    sa.sa_handler = disp;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if(sigaction(SIGINT, &sa, NULL)){
        perror("sigaction");
        exit(2);
    }

	for (;;) {
		
		char s[ 20 ];
		printf( "prompt>");
		fflush( stdout );
		fgets( s, 20, stdin );

		if ( !strcmp( s, "exit\n" ) ) {
			printf( "Bye!\n");
			exit( 1 );
		}
	}

	return 0;
}


