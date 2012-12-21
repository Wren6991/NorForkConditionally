#ifndef _PARSER_H_INCLUDED_
#define _PARSER_H_INCLUDED_

#include "syntaxtree.h"
#include "tokenizer.h"

class parser
{
    std::vector<token> tokens;
    int index;
    token t;
    token lastt;
    void gettoken();
    bool accept(token t);
    void expect(token t);
    public:
    parser(std::vector<token> tokens_);

    program* getprogram();
    definition *getdefinition();
        constdef* getconstdef();
        funcdef* getfuncdef();
        macrodef* getmacrodef();
    block* getblock();
    statement* getstatement();
    expression* getexpression();
};

#endif // _PARSER_H_INCLUDED_
