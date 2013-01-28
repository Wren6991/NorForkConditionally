#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED

#include <iostream>
#include <iostream>
#include <vector>
#include <map>

#include "error.h"

typedef enum {
    t_eof = 0,  //end of file
    t_comma,
    t_const,
    t_equals,
    t_function,
    t_goto,
    t_if,
    t_lbrace,
    t_lparen,
    t_macro,
    t_name,
    t_number,
    t_rbrace,
    t_rparen,
    t_semicolon,
    t_string,
    t_type,
    t_var,
    t_while
} token_type_enum;

class token
{
    public:
    token_type_enum type;
    std::string value;
    token();
    token(token_type_enum, std::string);
};

std::vector <token> tokenize(std::string);

#endif // TOKENIZER_H_INCLUDED
