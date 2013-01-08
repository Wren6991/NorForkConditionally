#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

#include "syntaxtree.h"

#include <map>

class variable
{
    std::string name;
    type_enum type;
};

class scope
{
  private:
    std::map <std::string, variable> variables;
  public:
    scope *parent;
    void insert(std::string name, variable var);
    variable& get(std::string name);
};


#endif //COMPILER_H_INCLUDED
