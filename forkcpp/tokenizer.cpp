#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "tokenizer.h"

enum state_enum
{
    s_start = 0,
    s_value,
    s_comment,
    s_string
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

std::vector <token> tokenize(std::string str)
{
    std::map<std::string, token_type_enum> keywords;
    keywords["def"] = t_def;
    keywords["end"] = t_end;
    keywords["var"] = t_def;
    keywords["dat"] = t_def;


    std::map<char, token> symbols;
    symbols['+'] = token(t_plus, "+");
    symbols['-'] = token(t_minus, "-");
    symbols['('] = token(t_lparen, "(");
    symbols[')'] = token(t_rparen, ")");
    symbols[','] = token(t_comma, ",");
    symbols['\'']= token(t_quote, "'");
    symbols[';'] = token(t_statementsep, ";");

    std::vector <token> tokens;

    std::string v;
    int index = -1;
    const char *buffer = str.c_str();
    char c;
    int startindex;
    state_enum state = s_start;
    do  // while(c);  - breaks at end of loop so we can process ids etc. that rest up against the end of the string.
    {
        c = buffer[++index];
        switch(state)
        {
            case s_start:
                startindex = index;
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')
                    state = s_value;
                else if (c == ' ' || c == '\t' || c == '\n' || c == '\r')   // swallow all the whitespace
                {
                    bool stat_sep_added = false;
                    do  //while we're looking at whitespace:
                    {
                        if ((!stat_sep_added) && (c == '\n' || c == '\r'))
                        {
                            stat_sep_added = true;
                            tokens.push_back(token(t_statementsep, ";"));
                        }
                        if (!(c == ' ' || c == '\t' || c == '\n' || c == '\r'))
                            break;
                    }
                    while ((c = buffer[++index]));
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
            case s_value:
                if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9') && c != '_')
                {
                    token t(t_value, str.substr(startindex, index - startindex));
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
    for (unsigned int i = 0; i < tokens.size(); i++)
        std::cout << tokens[i].type << ":\t\"" << tokens[i].value << "\"\n";
    return tokens;
}



