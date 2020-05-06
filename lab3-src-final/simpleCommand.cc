#include <cstdio>
#include <cstdlib>

#include <iostream>

#include "simpleCommand.hh"

SimpleCommand::SimpleCommand() {
  _arguments = std::vector<std::string *>();
  envtrue = false;
  tildtrue = false;
}

SimpleCommand::~SimpleCommand() {
  // iterate over all the arguments and delete them
  for (auto & arg : _arguments) {
    delete arg;
  }
}

//Environement Variable Expansion
std::string * SimpleCommand::envexpansion(std::string * argument) {
	//char *arg = const_cast<char*> (argument->c_str());

	//if ( arg[0]== '$') {
	//	//Get rid of ${ and }
	//	char * realarg = (char*)malloc(200);
	//	char *realargptr = realarg;
	//	arg = arg + 2;
	//	while (*arg != '}') {
	//		*realargptr = *arg;
	//		realargptr++;
	//		arg++;
	//	}
	//	*realargptr = '\0';
	//	printf("the argument is : %s\n", realarg);

	//	//Get environment variable
	//	char *arg = const_cast<char*> (argument->c_str());
	//	char * env = getenv(arg);
	//	printf("the environment variable is : %s\n", env);

	//	//Append character by character? How to do?
	//	std::string *str;

	//	//Append char by char? Not working
	//	while (*env) {
	//		//no matching function for call to ¡®std::__cxx11::basic_string<char>::append(char&)
	//		//invalid conversion from ¡®char¡¯ to ¡®const char*¡¯
	//		str->append(*env);			
	//	}
	//	
	//	return str;

	//	//str = argument->substr(2);
	//	//std::size_t	pos = str->find(")");
	//	//str = str->substr(0, pos);
	//	//char *temp = const_cast<char*> (str->c_str());
	//	//printf("the argument is : %s\n", temp);
	//}
	//envtrue = true;
	return argument;
}

void SimpleCommand::insertArgument( std::string * argument ) {
	//char *arg = const_cast<char*> (argument->c_str());
	//printf("the argument to be inserted is: %s\n", arg);

	//3.1: environment variable expansion
	//std::string * envexp = envexpansion(argument);

	//if (envtrue == true) {
	//	argument = envexp;
	//}

	//3.2: tilde expansion


	envtrue = false;
	tildtrue = false;
	_arguments.push_back(argument);
}

// Print out the simple command
void SimpleCommand::print() {
  for (auto & arg : _arguments) {
    std::cout << "\"" << arg << "\" \t";
  }
  // effectively the same as printf("\n\n");
  std::cout << std::endl;
}
