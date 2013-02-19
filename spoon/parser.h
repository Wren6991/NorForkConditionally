#ifndef _PARSER_H_INCLUDED_
#define _PARSER_H_INCLUDED_

#include "syntaxtree.h"
#include "tokenizer.h"

#include <vector>

class parser
{
    std::vector<token> tokens;
    std::map <std::string, type_enum> typestrings;
    int index;
    token t;
    token lastt;
    void gettoken();
    bool accept(token_type_enum type);
    void expect(token_type_enum type);
    public:
    parser(std::vector<token> tokens_);

    program* getprogram();
    definition *getdefinition();
        constdef* getconstdef();
        funcdef* getfuncdef();
        macrodef* getmacrodef();
    block* getblock();
    vardeclaration* getvardeclaration();
    vardeclaration::varpair getvarname_and_type(type_enum basetype);
    statement* getstatement();
        assignment* getassignment();
        goto_stat* getgoto();
        funccall* getfunccall();
        label* getlabel();
        if_stat* getif();
        while_stat* getwhile();
        break_stat* getbreak();
        continue_stat* getcontinue();
        return_stat* getreturn();
    expression* getexpression();
};

#endif // _PARSER_H_INCLUDED_
