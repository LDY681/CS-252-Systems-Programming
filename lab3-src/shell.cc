#include <cstdio>

#include "shell.hh"

void yyrestart(FILE * file);
int yyparse(void);

void Shell::prompt() {

	//if input is from terminal
	if (isatty(0)) {
		printf("myshell>");
		fflush(stdout);
	}
	fflush(stdout);
}

//TODO: when ctrl+C on ongoing process, nextline is myshell>>myshell>>
extern "C" void ctrlC(int sig) {
	//fflush(stdin);
	printf("\n");
	Shell::prompt();
}

//TODO: Only one msg for the same PID
extern "C" void zombie(int sig) {
	int pid = wait3(0, 0, NULL);


	while (waitpid(-1, NULL, WNOHANG) > 0) {};
//	printf("[%d] exited.\n", pid);
}

int main() {
	//2.1: CtrlC handling
	struct sigaction sigCtrl;
	sigCtrl.sa_handler = ctrlC;
	sigemptyset(&sigCtrl.sa_mask);
	sigCtrl.sa_flags = SA_RESTART;

	if (sigaction(SIGINT, &sigCtrl, NULL)) {
		perror("sigaction");
		exit(2);
	}

	//2.2: Zombie sigaction
	//TODO: Only analysize signal if background flag is true
	struct sigaction sigZombie;
//	if (Shell::_currentCommand._background == true) {
		sigZombie.sa_handler = zombie;
		sigemptyset(&sigZombie.sa_mask);
		sigZombie.sa_flags = SA_RESTART;

		if (sigaction(SIGCHLD, &sigZombie, NULL)) {
			perror("sigaction");
			exit(-1);
		}
//		}

	//2.7: Create .shellrc
	FILE*fd = fopen(".shellrc", "r");
	if (fd) {
		yyrestart(fd);
		yyparse();
		yyrestart(stdin);
		fclose(fd);
	}
	else {
		Shell::prompt();
	}

	yyparse();
}

Command Shell::_currentCommand;
