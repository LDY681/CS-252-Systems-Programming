#ifndef simplcommand_hh
#define simplecommand_hh

#include <string>
#include <vector>

struct SimpleCommand {

  // Simple command is simply a vector of strings
  std::vector<std::string *> _arguments;
  bool envtrue;
  bool tildtrue;

  SimpleCommand();
  ~SimpleCommand();
  void insertArgument( std::string * argument );
  std::string* envexpansion(std::string * argument);
  void print();
};

#endif
