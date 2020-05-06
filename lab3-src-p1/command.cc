/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 * DO NOT PUT THIS PROJECT IN A PUBLIC REPOSITORY LIKE GIT. IF YOU WANT 
 * TO MAKE IT PUBLICALLY AVAILABLE YOU NEED TO REMOVE ANY SKELETON CODE 
 * AND REWRITE YOUR PROJECT SO IT IMPLEMENTS FUNCTIONALITY DIFFERENT THAN
 * WHAT IS SPECIFIED IN THE HANDOUT. WE OFTEN REUSE PART OF THE PROJECTS FROM  
 * SEMESTER TO SEMESTER AND PUTTING YOUR CODE IN A PUBLIC REPOSITORY
 * MAY FACILITATE ACADEMIC DISHONESTY.
 */

#include <cstdio>
#include <cstdlib>

#include <iostream>

#include "command.hh"
#include "shell.hh"


Command::Command() {
    // Initialize a new vector of Simple Commands
    _simpleCommands = std::vector<SimpleCommand *>();

    _outFile = NULL;
    _inFile = NULL;
    _errFile = NULL;
    _background = false;
	_append = 0;
}

void Command::insertSimpleCommand( SimpleCommand * simpleCommand ) {
    // add the simple command to the vector
    _simpleCommands.push_back(simpleCommand);
}

void Command::clear() {
    // deallocate all the simple commands in the command vector
    for (auto simpleCommand : _simpleCommands) {
        delete simpleCommand;
    }

    // remove all references to the simple commands we've deallocated
    // (basically just sets the size to 0)
    _simpleCommands.clear();

    if ( _outFile ) {
        delete _outFile;
    }
    _outFile = NULL;

    if ( _inFile ) {
        delete _inFile;
    }
    _inFile = NULL;

    if ( _errFile ) {
        delete _errFile;
    }
    _errFile = NULL;

    _background = false;
	_append = 0;
}

void Command::print() {
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    int i = 0;
    // iterate over the simple commands and print them nicely
    for ( auto & simpleCommand : _simpleCommands ) {
        printf("  %-3d ", i++ );
        simpleCommand->print();
    }

    printf( "\n\n" );
    printf( "  Output       Input        Error        Background\n" );
    printf( "  ------------ ------------ ------------ ------------\n" );
    printf( "  %-12s %-12s %-12s %-12s\n",
            _outFile?_outFile->c_str():"default",
            _inFile?_inFile->c_str():"default",
            _errFile?_errFile->c_str():"default",
            _background?"YES":"NO");
    printf( "\n\n" );
}

void Command::execute() {

	//TODO:
	//printf("_simpleCommands.size is %zu", _simpleCommands.size());
	// Don't do anything if there are no simple commands
	if (_simpleCommands.size() == 0) {
		Shell::prompt();
		return;
	}

	// Print contents of Command data structure
	print();

	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec
	int dfltin = dup(0);
	int dfltout = dup(1);
	int dflterr = dup(2);

	int fdin = 0;
	int	fdout = 0;
	int fderr = 0;

	if (_inFile) {
		fdin = open(_inFile->c_str(), O_RDONLY);
	}
	else {
		fdin = dup(dfltin);
	}

	if (_errFile) {
		if (_append) {
			fderr = open(_errFile->c_str(), O_WRONLY | O_APPEND | O_CREAT, 0600);
		}
		else {
			fderr = open(_errFile->c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
		}
	}
	else {
		fderr = dup(dflterr);
	}

	dup2(fderr, 2);
	close(fderr);

	int pid;

	for (size_t i = 0; i < _simpleCommands.size(); i++) {

		//TODO: Handle several built-in commands

		//redirect input
		dup2(fdin, 0);
		close(fdin);

		//setup output
		if (i == _simpleCommands.size() - 1) {
			//Last simple command
			if (_outFile) {
				if (_append) {
					fdout = open(_outFile->c_str(), O_WRONLY | O_APPEND | O_CREAT, 0600);
				}
				else {
					fdout = open(_outFile->c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
				}
			}
			else {
				//Use default output
				fdout = dup(dfltout);
			}
		}
		else {	//Not last simple command->create pipe
			int fdpipe[2];
			pipe(fdpipe);
			fdout = fdpipe[1];
			fdin = fdpipe[0];
		}	//if/else

		//Redirect the output
		dup2(fdout, 1);
		close(fdout);

		//create child process
		pid = fork();
		
		if (pid == -1) {
			perror("fork\n");
			exit(2);
		}

		if (pid == 0) {
			//Convert std::vector<std::string *> _arguments into char**
			size_t argsize = _simpleCommands[i]->_arguments.size();
			char ** x = new char*[argsize+1];
			for (size_t j = 0; j < argsize; j++) {
				x[j] = const_cast<char*>(_simpleCommands[i]->_arguments[j]->c_str());
				x[j][strlen(_simpleCommands[i]->_arguments[j]->c_str())] = '\0';

			}
			x[argsize] = NULL;
			execvp(x[0], x);
			//perror("execvp");
			_exit(1);	//exit immeditately without messing with buffer
		}

	}	//for

	dup2(dfltin, 0);
	dup2(dfltout, 1);
	dup2(dflterr, 2);
	close(dfltin);
	close(dfltout);
	close(dflterr);

	if (_background == false) {
		//wait for last command
		waitpid(pid, NULL, 0);
	}
	// Clear to prepare for next command
	clear();

	// Print new prompt
	Shell::prompt();
}	//execute

SimpleCommand * Command::_currentSimpleCommand;
