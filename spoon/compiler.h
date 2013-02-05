#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

#include "syntaxtree.h"
#include "object.h"

#include <map>
#include <vector>
#include <set>

struct func_signature
{
    type_enum return_type;
    std::vector<type_enum> arg_types;
    bool operator==(func_signature &rhs) const {return rhs.return_type == return_type && rhs.arg_types == arg_types;}
    bool operator!=(func_signature &rhs) const {return !operator==(rhs);}
};

struct symbol
{
    std::string name;
    type_enum type;
    bool is_constant;
    int value;
    symbol(){is_constant = false;}
};

class scope
{
  private:
    std::map <std::string, symbol> variables;
  public:
    scope(scope *_parent = 0);
    scope *parent;
    void insert(std::string name, symbol var);
    symbol& get(std::string name);
    bool exists(std::string name);
};


// The scopes map a local name to a global symbol (name@ptr)
// The global symbol table gets exported as part of the object, also makes it easy to look up types in type checking.
class compiler
{
    scope *globalscope;
    scope *currentscope;
    std::map<std::string, symbol> globalsymboltable;
    std::set<std::string> defined_funcs;
    std::map<std::string, func_signature> functions;
    void pushscope();
    void popscope();
    public:
    compiler();
    object* compile(program*);
    void compile(macrodef*);
    void compile(funcdef*);
    void compile(block*);
    void compile(funccall*);
    void compile(goto_stat*);
    void compile(label*);
    void compile(if_stat*);
    void compile(while_stat*);
    void compile(expression*);
    void gettype(expression*);
    void compile(assignment*);
    bool match_types(type_enum expected, type_enum &received);
    void addvar(std::string name, type_enum type, int ptr, bool isConstant = false, int constvalue = 0);
};


#endif //COMPILER_H_INCLUDED
