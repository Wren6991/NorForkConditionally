#ifndef _PARSER_H_INCLUDED_
#define _PARSER_H_INCLUDED_

#include "syntaxtree.h"
#include "tokenizer.h"

#include <map>
#include <set>
#include <vector>

class parser
{
    std::string filename;
    std::string filedirectory;
    std::vector<token> tokens;
    std::map <std::string, type_enum> typestrings;
    std::set <std::string> includedfiles;
    int index;
    token t;
    token lastt;
    void gettoken();
    bool accept(token_type_enum type);
    void expect(token_type_enum type);
    public:
    parser(std::vector<token> tokens_, std::string _filename = "file", std::string _filedirectory = "");

    program* getprogram();
    void do_preprocessor(program *prog);
    void throw_unexpected(std::string value, int linenumber = 0, token_type_enum expected = t_eof, token_type_enum got = t_eof);
    definition *getdefinition();
        constdef* getconstdef();
        funcdef* getfuncdef(bool exported = false);
        macrodef* getmacrodef();
    block* getblock();
    vardeclaration* getvardeclaration(std::vector<statement*> *statlist = 0, bool exported = false);   // we tell the function where the statements are, so it's allowed to push assignments. (See implementation for more notes)
    vardeclaration::varpair getvarname_and_type(type_enum basetype, std::vector<statement*> *statlist);
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
    expression* getsinglevalue();
};

#endif // _PARSER_H_INCLUDED_
