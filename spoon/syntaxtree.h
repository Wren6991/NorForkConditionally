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
    type_pointer,
    type_label,
    type_number,        // generic number type, for compiler use only.
    n_types
} type_enum;

const int typesizes[n_types] =
{
    0,
    1,
    2,
    0,
    1
};

const std::string friendly_type_names[] =
{
    "void",
    "int",
    "pointer",
    "label",
    "number"
};

typedef enum {
    stat_call,
    stat_goto,
    stat_assignment,
    stat_label,
    stat_if,
    stat_while
} stat_type;

typedef enum {
    exp_name,
    exp_number,
    exp_funccall
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
    bool defined;       // vs. merely declared
    funcdef() {type = dt_funcdef; defined = false;}
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

struct goto_stat: public statement
{
    expression *target;
    goto_stat() {type = stat_goto;}
};

struct assignment: public statement
{
    std::string name;
    expression *expr;
    assignment() {type = stat_assignment;}
};

struct label: public statement
{
    std::string name;
    label() {type = stat_label;}
};

struct if_stat: public statement
{
    expression *expr;
    block *ifblock;
    block *elseblock;
    if_stat() {type = stat_if; elseblock = 0;}
};

struct while_stat: public statement
{
    expression *expr;
    block *blk;
    while_stat() {type = stat_while;}
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
    type_enum val_type;         // <- Not touched by the parser: the compiler sets it when it reads types, and the linker reads it later.
    std::vector<expression*> args;
};


#endif // SYNTAXTREE_H_INCLUDED
