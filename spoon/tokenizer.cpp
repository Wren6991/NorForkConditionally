#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "tokenizer.h"

std::string friendly_tokentype_names[] = {
    "EOF",
    "\"break\"",
    "colon",
    "comma",
    "\"const\"",
    "\"continue\"",
    "\"else\"",
    "\"=\"",
    "\"function\"",
    "\"goto\"",
    "\"if\"",
    "\"{\"",
    "\"(\"",
    "\"macro\"",
    "name",
    "number",
    "\"}\"",
    "\"return\"",
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
    s_slashaccepted,
    s_linecomment,
    s_streamcomment,
    s_staraccepted,
    s_whitespace
};



extern std::string token_type_names[];


token::token()
{
    type = t_eof;
    value = std::string();
}

token::token(token_type_enum type_, std::string value_, int linenumber_ = -1)
{
    type = type_;
    value = value_;
    linenumber = linenumber_;
}

token::token(const token &other, int linenumber_)
{
    type = other.type;
    value = other.value;
    linenumber = linenumber_;
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
    keywords["break"] = t_break;
    keywords["const"] = t_const;
    keywords["continue"] = t_continue;
    keywords["else"] = t_else;
    keywords["goto" ] = t_goto;
    keywords["function"] = t_function;
    keywords["if" ] = t_if;
    keywords["int"] = t_type;
    keywords["macro"] = t_macro;
    keywords["pointer"] = t_type;
    keywords["return"] = t_return;
    keywords["var"] = t_var;
    keywords["while"] = t_while;

    std::map<char, token> symbols;
    symbols[':'] = token(t_colon , ":");
    symbols[','] = token(t_comma , ",");
    symbols['='] = token(t_equals, "=");
    symbols['('] = token(t_lparen, "(");
    symbols[')'] = token(t_rparen, ")");
    symbols['{'] = token(t_lbrace, "{");
    symbols['}'] = token(t_rbrace, "}");
    symbols[';'] = token(t_semicolon, ";");

    std::vector <token> tokens;
    int linenumber = 1;

    int index = -1;
    const char *buffer = str.c_str();
    char c;
    int startindex = index;
    state_enum state = s_start;
    do  // while(c);  - breaks at end of loop instead of start, so we can process ids etc. that rest up against the end of the string.
    {
        c = buffer[++index];
        if (c == '\n')
            linenumber++;
        switch(state)
        {
            case s_start:
                startindex = index;
                if (is_digit(c))
                    state = s_number;
                else if (allowed_in_name(c))
                    state = s_name;
                else if (is_whitespace(c))   // swallow all the whitespace
                    state = s_whitespace;
               else if (c == '/')
                    state = s_slashaccepted;
                else if (symbols.find(c) != symbols.end())  //if we find a matching symbol, push a matching token onto the list.
                    tokens.push_back(token(symbols.find(c)->second, linenumber));
                else if (c == '"')
                    state = s_string;
                else if (c)
                {
                    std::stringstream ss;
                    ss << "Error: unexpected character near \"" << c << "\", on line " << linenumber;
                    throw(error(ss.str()));
                }

                break;
            case s_string:
                if (c == '"')
                {
                    tokens.push_back(token(t_string, str.substr(startindex + 1, index - startindex - 1), linenumber));
                    state = s_start;
                }
                break;
            case s_slashaccepted:
                if (c == '/')
                    state = s_linecomment;
                else if (c == '*')
                    state = s_streamcomment;
                else
                    tokens.push_back(token(symbols['/'], linenumber));
                break;
            case s_linecomment:
                if (c == '\n' || c == '\r')
                    state = s_start;
                break;
            case s_streamcomment:
                if (c == '*')
                    state = s_staraccepted;
                break;
            case s_staraccepted:
                if (c == '/')
                    state = s_start;
                else
                    state = s_streamcomment;
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
                        tokens.push_back(token(t_number, val, linenumber));
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
                    tokens.push_back(token(t_number, ss2.str(), linenumber));
                    state = s_start;
                    index--;
                }
                break;
            case s_name:
                if (!allowed_in_name(c))
                {
                    token t(t_name, str.substr(startindex, index - startindex), linenumber);
                    if (keywords.find(t.value) != keywords.end())
                        t.type = keywords.find(t.value)->second;
                    tokens.push_back(t);
                    state = s_start;
                    index--;
                }
                break;
            case s_whitespace:
                if (!is_whitespace(c))
                {
                    index--;   // we've now encountered a character that isn't whitespace; unget it so the next loop can pick it up.
                    state = s_start;
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



