#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

#include "syntaxtree.h"

#include <map>
#include <vector>
#include <set>

// TODO: turn this into a struct that includes return type etc.
typedef std::vector<type_enum> func_signature;

struct variable
{
    std::string name;
    type_enum type;
    bool is_constant;
    int value;
    variable(){is_constant = false;}
};

class scope
{
  private:
    std::map <std::string, variable> variables;
  public:
    scope(scope *_parent = 0);
    scope *parent;
    void insert(std::string name, variable var);
    variable& get(std::string name);
    bool exists(std::string name);
};


// The scopes map a local name to a global symbol (name@ptr)
// The global symbol table gets exported as part of the object, also makes it easy to look up types in type checking.
class compiler
{
    scope *globalscope;
    scope *currentscope;
    std::map<std::string, variable> globalsymboltable;
    std::set<std::string> defined_funcs;
    std::map<std::string, func_signature> functions;
    void pushscope();
    void popscope();
    public:
    compiler();
    void compile(program *prog);
    void compile(block *blk);
    void compile(funccall *fcall);
    void compile(expression *expr);
    void addvar(std::string name, type_enum type, int ptr, bool isConstant = false, int constvalue = 0);
};


#endif //COMPILER_H_INCLUDED
