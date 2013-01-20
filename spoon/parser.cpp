#include "parser.h"
#include "resourcep.h"

#include <sstream>

// Throw a preformatted error that we've received an unexpected token.
// TODO: make it show the expected token type;
void throw_unexpected(std::string value)
{
    std::stringstream ss;
    ss << "Error: unexpected token near \"" << value << "\"";
    throw(error(ss.str()));
}


// Set up the type dicts and the state vars
parser::parser(std::vector<token> tokens_)
{
    types["int"] = type_int;
    types["pointer"] = type_pointer;
    tokens = tokens_;
    index = 0;
    if (tokens.size() > 0)
        t = tokens[0];
}

// read the next token, put it in t; make t a blank token if no more tokens.
void parser::gettoken()
{
    lastt = t;
    index++;
    if ((unsigned)index < tokens.size())
    {
        t = tokens[index];
    }
    else
    {
        t = token();
    }
}

// optionally gobble up a token, and return whether or not we have gobbled.
bool parser::accept(token_type_enum type)
{
    if (t.type == type)
    {
        gettoken();
        return true;
    }
    else
    {
        return false;
    }
}

// gobble up a token, or raise an error if it doesn't match
void parser::expect(token_type_enum type)
{
    if (!accept(type))
    {
        throw_unexpected(t.value);
    }
}

// top level function: delegates to the function, macro and const
// definition functions.
program* parser::getprogram()
{
    resourcep <program> prog;
    while (t.type != t_eof)
    {
        if (accept(t_function))
        {
            prog.obj->defs.push_back(getfuncdef());
        }
        else if (accept(t_macro))
        {
            prog.obj->defs.push_back(getmacrodef());
        }
        else if (accept(t_const))
        {
            prog.obj->defs.push_back(getconstdef());
        }
        else
        {
            throw_unexpected(t.value);
        }
    }
    return prog.release();
}

// sort out all the syntax stuff, return the type, name and
// value of the constant.
constdef* parser::getconstdef()
{
    resourcep <constdef> def;
    expect(t_type);
    def.obj->valtype = types[lastt.value];
    expect(t_name);
    def.obj->name = lastt.value;
    expect(t_equals);
    expect(t_number);
    std::stringstream ss;
    ss << lastt.value;
    ss >> def.obj->value;  // convert string to int
    expect(t_semicolon);
    return def.release();
}

// read in the list of args and parse the macro body
macrodef* parser::getmacrodef()
{
    resourcep <macrodef> def;
    expect(t_name);
    def.obj->name = lastt.value;
    expect(t_lparen);
    if (accept(t_name))
    {
        def.obj->args.push_back(lastt.value);
        while (accept(t_comma))
        {
            expect(t_name);
            def.obj->args.push_back(lastt.value);
        }
    }
    expect(t_rparen);
    def.obj->body = getblock();
    return def.release();
}


funcdef* parser::getfuncdef()
{
    resourcep <funcdef> def;
    if (accept(t_type))
    {
        def.obj->return_type = types[lastt.value];
    }
    else
    {
        def.obj->return_type = type_none;
    }
    expect(t_name);
    def.obj->name = lastt.value;
    expect(t_lparen);
    if (accept(t_type))
    {
        type_enum type = types[lastt.value];
        expect(t_name);
        def.obj->args.push_back(argument(type, lastt.value));
        while (accept(t_comma))
        {
            expect(t_type);
            type_enum type = types[lastt.value];
            expect(t_name);
            def.obj->args.push_back(argument(type, lastt.value));
        }
    }
    expect(t_rparen);
    if (accept(t_semicolon))
    {
        def.obj->defined = false;
    }
    else
    {
        def.obj->defined = true;        // there is a function definition body, so it's not just a declaration.
        def.obj->body = getblock();
    }
    return def.release();
}

// block is either a single statement, or a bunch of statements
// and definitions between two braces.
block* parser::getblock()
{
    resourcep <block> blk;
    if (accept(t_lbrace))
    {
        while(t.type != t_rbrace)
        {
            if (accept(t_var))
            {
                blk.obj->declarations.push_back(getvardeclaration());
            }
            else
            {
                blk.obj->statements.push_back(getstatement());
            }
        }
        accept(t_rbrace);
    }
    else
        blk.obj->statements.push_back(getstatement());
    return blk.release();
}

// returns the type and a list of names
vardeclaration* parser::getvardeclaration()
{
    resourcep <vardeclaration> vardec;
    expect(t_type);
    vardec.obj->type = types[lastt.value];
    expect(t_name);
    vardec.obj->names.push_back(lastt.value);
    while (accept(t_comma))
    {
        expect(t_name);
        vardec.obj->names.push_back(lastt.value);
    }
    expect(t_semicolon);
    return vardec.release();
}


statement* parser::getstatement()
{
    resourcep <funccall> fcall;
    expect(t_name);
    fcall.obj->name = lastt.value;
    expect(t_lparen);
    if (t.type != t_rparen)
    {
        fcall.obj->args.push_back(getexpression());
        while (accept(t_comma))
        {
            fcall.obj->args.push_back(getexpression());
        }
    }
    expect(t_rparen);
    expect(t_semicolon);
    return fcall.release();
}

expression* parser::getexpression()
{
    resourcep <expression> expr;
    if (accept(t_name))
    {
        expr.obj->type = exp_name;
        expr.obj->name = lastt.value;
    }
    else
    {
        expr.obj->type = exp_number;
        expect(t_number);
        std::stringstream ss;
        ss << lastt.value;
        ss >> expr.obj->number;
    }
    return expr.release();
}
