#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "tokenizer.h"

std::string friendly_type_names[] = {
    "EOF",
    "comma",
    "\"const\"",
    "\"=\"",
    "\"function\"",
    "\"goto\"",
    "\"if\"",
    "\"{\"",
    "\")\"",
    "\"macro\"",
    "name",
    "number",
    "\"}\"",
    "\")\"",
    "semicolon",
    "string",
    "type",
    "\"var\"",
    "\"while\""
};

enum state_enum
{
    s_start = 0,
    s_number,
    s_number_hex,
    s_name,
    s_string,
    s_comment
};



extern std::string token_type_names[];


token::token()
{
    token::type = t_eof;
    token::value = std::string();
}

token::token(token_type_enum type_, std::string value_)
{
    token::type = type_;
    token::value = value_;
}

inline bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

inline bool is_hex_digit(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

inline bool allowed_in_name(char c)
{
    // uppercase, lowercase, digit or underscore:
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

inline bool is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

std::vector <token> tokenize(std::string str)
{
    std::map<std::string, token_type_enum> keywords;
    keywords["const"] = t_const;
    keywords["goto" ] = t_goto;
    keywords["if" ] = t_if;
    keywords["int"] = t_type;
    keywords["function"] = t_function;
    keywords["macro"] = t_macro;
    keywords["pointer"] = t_type;
    keywords["var"] = t_var;
    keywords["while"] = t_while;

    std::map<char, token> symbols;
    symbols[','] = token(t_comma , ",");
    symbols['='] = token(t_equals, "=");
    symbols['('] = token(t_lparen, "(");
    symbols[')'] = token(t_rparen, ")");
    symbols['{'] = token(t_lbrace, "{");
    symbols['}'] = token(t_rbrace, "}");
    symbols[';'] = token(t_semicolon, ";");

    std::vector <token> tokens;

    int index = -1;
    const char *buffer = str.c_str();
    char c;
    int startindex;
    state_enum state = s_start;
    do  // while(c);  - breaks at end of loop instead of start, so we can process ids etc. that rest up against the end of the string.
    {
        c = buffer[++index];
        switch(state)
        {
            case s_start:
                startindex = index;
                if (is_digit(c))
                    state = s_number;
                else if (allowed_in_name(c))
                    state = s_name;
                else if (is_whitespace(c))   // swallow all the whitespace
                {
                    while (is_whitespace(buffer[++index]));
                    index--;        // we've now encountered a character that isn't whitespace; unget it so the next loop can pick it up.
                }
                else if (symbols.find(c) != symbols.end())  //if we find a matching symbol, push a matching token onto the list.
                    tokens.push_back(symbols.find(c)->second);
                else if (c == '#')
                    state = s_comment;
                else if (c == '"')
                    state = s_string;
                else if (c)
                {
                    std::stringstream ss;
                    ss << "Error: unrecognized character near " << c;
                    throw(error(ss.str()));
                }

                break;
            case s_string:
                if (c == '"')
                {
                    tokens.push_back(token(t_string, str.substr(startindex + 1, index - startindex - 1)));
                    state = s_start;
                }
                break;
            case s_comment:
                if (c == '\n' || c == '\r')
                    state = s_start;
                break;
            case s_number:
                if (!is_digit(c))
                {
                    std::string val = str.substr(startindex, index - startindex);
                    if (c == 'x' && val == "0")
                    {
                        state = s_number_hex;
                        startindex += 2;
                    }
                    else
                    {
                        tokens.push_back(token(t_number, val));
                        state = s_start;
                        index--;
                    }
                }
                break;
            case s_number_hex:
                if (!is_hex_digit(c))
                {
                    int value;
                    std::stringstream ss;
                    ss << std::hex << str.substr(startindex, index - startindex);
                    ss >> value;
                    std::stringstream ss2;
                    ss2 << value;
                    tokens.push_back(token(t_number, ss2.str()));
                    state = s_start;
                    index--;
                }
                break;
            case s_name:
                if (!allowed_in_name(c))
                {
                    token t(t_name, str.substr(startindex, index - startindex));
                    if (keywords.find(t.value) != keywords.end())
                        t.type = keywords.find(t.value)->second;
                    tokens.push_back(t);
                    state = s_start;
                    index--;
                }
                break;
        }
    } while (c);
    if (state == s_string)
        throw(error("Error: expected \" to close string near EOF"));
    /*for (unsigned int i = 0; i < tokens.size(); i++)
        std::cout << tokens[i].type << ":\t\"" << tokens[i].value << "\"\n";*/
    return tokens;
}



