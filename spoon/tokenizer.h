#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED

#include <iostream>
#include <iostream>
#include <vector>
#include <map>

#include "error.h"

typedef enum {
    t_eof = 0,  //end of file
    t_and,
    t_break,
    t_colon,
    t_comma,
    t_const,
    t_continue,
    t_else,
    t_equals,
    t_function,
    t_goto,
    t_hash,
    t_if,
    t_lbrace,
    t_lparen,
    t_lsquareb,
    t_macro,
    t_name,
    t_not,
    t_number,
    t_or,
    t_rbrace,
    t_return,
    t_rparen,
    t_rsquareb,
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
    int linenumber;
    token();
    token(token_type_enum, std::string, int);
    token(const token&, int);   // for different line number instances of token
};

std::vector <token> tokenize(std::string);

#endif // TOKENIZER_H_INCLUDED







