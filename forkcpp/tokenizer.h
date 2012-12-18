#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED

#include <iostream>
#include <iostream>
#include <vector>
#include <map>

#include "error.h"

typedef enum {
    t_eof = 0,  //end of file
    t_value,
    t_plus,
    t_minus,
    t_lparen,
    t_rparen,
    t_string,
    t_comma,
    t_quote,
    t_end,
    t_def,
    t_var,
    t_dat,
    t_statementsep
    } token_type_enum;

class arglist_member;
class tagged_value;

typedef tagged_value (*dfuncd)(arglist_member*);

class token
{
    public:
    token_type_enum type;
    std::string value;
    token();
    token(token_type_enum, std::string);
};

std::vector <token> tokenize(std::string);
std::vector <token> tokenize(std::string, std::map <std::string, dfuncd>);

#endif // TOKENIZER_H_INCLUDED
