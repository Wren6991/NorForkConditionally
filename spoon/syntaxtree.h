#ifndef SYNTAXTREE_H_INCLUDED
#define SYNTAXTREE_H_INCLUDED

#include <vector>
#include <string>

typedef enum {
    dt_constdef = 0,
    dt_funcdef,
    dt_macrodef
} def_type;

typedef enum {
    type_none = 0,
    type_int,
    type_pointer
} type_enum;

typedef enum {
    stat_call
} stat_type;

typedef enum {
    exp_name,
    exp_number
} exp_type;


struct definition;
struct block;
struct expression;


struct program
{
    std::vector <definition*> defs;
};

struct definition
{
    def_type type;
};

struct constdef: public definition
{
    type_enum valtype;
    std::string name;
    int value;                          // ought really to be an expression
    constdef() {type = dt_constdef;}
};

struct argument
{
    type_enum type;
    std::string name;
    argument(type_enum type_ = type_none, std::string name_ = "")
    {
        type = type_;
        name = name_;
    }
};

struct funcdef: public definition
{
    type_enum return_type;
    std::string name;
    std::vector <argument> args;
    block *body;
    funcdef() {type = dt_funcdef;}
};

struct macrodef: public definition
{
    std::string name;
    std::vector<std::string> args;
    block *body;
    macrodef() {type = dt_macrodef;}
};

struct vardeclaration
{
    type_enum type;
    std::vector <std::string> names;
};

struct statement
{
    stat_type type;
};

struct funccall: public statement
{
    std::string name;
    std::vector <expression*> args;
    funccall() {type = stat_call;}
};

struct block
{
    std::vector <vardeclaration*> declarations;
    std::vector <statement*> statements;
};

struct expression
{
    exp_type type;
    std::string name;
    int number;
};


#endif // SYNTAXTREE_H_INCLUDED
