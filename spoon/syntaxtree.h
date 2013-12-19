#ifndef SYNTAXTREE_H_INCLUDED
#define SYNTAXTREE_H_INCLUDED

#include <set>
#include <string>
#include <vector>

#include "type.h"

typedef enum {
    dt_constdef = 0,
    dt_funcdef,
    dt_macrodef,
    dt_vardec       // dec vs. def I know...
} def_type;

typedef enum {
    stat_call,
    stat_goto,
    stat_assignment,
    stat_label,
    stat_if,
    stat_while,
    stat_break,
    stat_continue,
    stat_return
} stat_type;

typedef enum {
    exp_name,
    exp_number,
    exp_funccall,
    exp_string,
    exp_and,
    exp_not,
    exp_or
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
    type_t valtype;
    std::string name;
    int value;                          // ought really to be an expression
    constdef() {type = dt_constdef;}
};

struct argument
{
    type_t type;
    std::string name;
    argument(type_t type_ = type_none, std::string name_ = "")
    {
        type = type_;
        name = name_;
    }
};

struct funcdef: public definition
{
    type_t return_type;
    std::string name;
    std::vector <argument> args;
    block *body;
    bool defined;       // vs. merely declared
    bool exported;
    std::vector <int> exportvectors;
    // Only used by compiler and linker:
    std::set<std::string> dependson;
    bool is_used;   // For mark and sweep of functions by linker
    funcdef() {type = dt_funcdef; defined = false; is_used = false; exported = false;}
};

struct macrodef: public definition
{
    std::string name;
    std::vector<std::string> args;
    block *body;
    macrodef() {type = dt_macrodef;}
};

struct vardeclaration: public definition
{
    struct varpair
    {
        std::string name;
        type_t type;
        bool exported;
        int exportvector;
        varpair() {exported = false;}
    };
    std::vector <varpair> vars;
    vardeclaration *getcopy()
    {
        vardeclaration *vardec = new vardeclaration;
        for (unsigned int i = 0; i < vars.size(); i++)
            vardec->vars.push_back(vars[i]);
        return vardec;
    }
    vardeclaration() {type = dt_vardec;}
};

struct statement
{
    stat_type type;
    virtual statement* getcopy() = 0;
};

struct funccall: public statement
{
    std::string name;
    std::vector <expression*> args;
    virtual statement* getcopy();
    funccall() {type = stat_call;}
};

struct goto_stat: public statement
{
    expression *target;
    virtual statement* getcopy();
    goto_stat() {type = stat_goto;}
};

struct assignment: public statement
{
    std::string name;
    bool indexed;
    int index;
    expression *expr;
    virtual statement* getcopy();
    assignment() {type = stat_assignment; indexed = false;}
};

struct label: public statement
{
    std::string name;
    virtual statement* getcopy();
    label() {type = stat_label;}
};

struct if_stat: public statement
{
    expression *expr;
    block *ifblock;
    block *elseblock;
    virtual statement* getcopy();
    if_stat() {type = stat_if; elseblock = 0;}
};

struct while_stat: public statement
{
    expression *expr;
    block *blk;
    virtual statement* getcopy();
    while_stat() {type = stat_while;}
};

struct break_stat: public statement
{
    break_stat() {type = stat_break;}
    virtual statement* getcopy() { return new break_stat; }
};

struct continue_stat: public statement
{
    continue_stat() {type = stat_continue;}
    virtual statement* getcopy() { return new continue_stat; }
};

struct return_stat: public statement
{
    return_stat() {type = stat_return;}
    virtual statement* getcopy() { return new return_stat; }
};

struct block
{
    std::vector <vardeclaration*> declarations;
    std::vector <statement*> statements;
    block* getcopy();
};

struct expression
{
    exp_type type;
    std::string name;
    int number;
    bool indexed;
    type_t val_type;         // <- Not touched by the parser: the compiler sets it when it reads types, and the linker reads it later.
    std::vector<expression*> args;
    expression* getcopy();
    expression() {indexed = false;}
    expression(std::string _name) {type = exp_name; name = _name; indexed = false;}
};

inline statement* funccall::getcopy()
{
    funccall *f = new funccall;
    f->name = name;
    for (unsigned int i = 0; i < args.size(); ++i)
        f->args.push_back(args[i]->getcopy());
    return f;
}

inline statement* goto_stat::getcopy()
{
    goto_stat *g = new goto_stat;
    g->target = target->getcopy();
    return g;
}

inline statement* assignment::getcopy()
{
    assignment *assg = new assignment;
    assg->name = name;
    assg->indexed = indexed;
    assg->index = index;
    assg->expr = expr->getcopy();
    return assg;
}

inline statement* label::getcopy()
{
    label *lbl = new label;
    lbl->name = name;
    return lbl;
}

inline statement* if_stat::getcopy()
{
    if_stat *ifs = new if_stat;
    ifs->expr = expr->getcopy();
    ifs->ifblock = ifblock->getcopy();
    ifs->elseblock = elseblock->getcopy();
    return ifs;
}

inline statement* while_stat::getcopy()
{
    while_stat *whiles = new while_stat;
    whiles->expr = expr->getcopy();
    whiles->blk = blk->getcopy();
}

inline block* block::getcopy()
{
    block *blk = new block;
    for (unsigned int i = 0; i < declarations.size(); ++i)
        blk->declarations.push_back(declarations[i]->getcopy());
    for (unsigned int i = 0; i < statements.size(); ++i)
        blk->statements.push_back(statements[i]->getcopy());
    return blk;
}

inline expression* expression::getcopy()
{
    expression *expr = new expression;
    expr->type = type;
    expr->name = name;
    expr->number = number;
    expr->val_type = val_type;
    for (unsigned int i = 0; i < args.size(); ++i)
        expr->args.push_back(args[i]->getcopy());
    return expr;
}


#endif // SYNTAXTREE_H_INCLUDED
