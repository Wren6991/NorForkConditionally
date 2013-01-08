#include "compiler.h"
#include "error.h"


void scope::insert(std::string name, variable var)
{
    variables[name] = var;
}

variable& scope::get(std::string name)
{
    if (variables.find(name) != variables.end())
        return variables[name];
    else if (parent)
        return parent->get(name);
    else
        throw(error("Error: undefined name _!"));
}
