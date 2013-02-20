#include "parser.h"
#include "resourcep.h"

#include <fstream>
#include <sstream>

extern std::string friendly_tokentype_names[];

// Throw a preformatted error that we've received an unexpected token.
void throw_unexpected(std::string value, int linenumber = 0, token_type_enum expected = t_eof, token_type_enum got = t_eof)
{
    std::stringstream ss;
    ss << "Error: unexpected token near \"" << value << "\"";
    if (linenumber)
        ss << " on line " << linenumber;
    if (expected)
        ss << ": expected " << friendly_tokentype_names[expected] << ", got " << friendly_tokentype_names[got];
    throw(error(ss.str()));
}


// Set up the type dicts and the state vars
parser::parser(std::vector<token> tokens_)
{
    typestrings["char"] = type_int;
    typestrings["int"] = type_int;
    typestrings["pointer"] = type_pointer;
    typestrings["void"] = type_none;
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
        throw_unexpected(t.value, t.linenumber, type, t.type);
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
        else if (accept(t_var))
        {
            prog.obj->defs.push_back(getvardeclaration());
        }
        else if (accept(t_hash))
        {
            expect(t_name);
            do_preprocessor(prog.obj);
        }
        else
        {
            throw_unexpected(t.value, t.linenumber, t_function, t.type);
        }
    }
    return prog.release();
}

void parser::do_preprocessor(program *prog)
{
    if (lastt.value == "include")
    {
        expect(t_string);
        std::fstream includefile(lastt.value, std::ios::in | std::ios::binary);
        if (!includefile.is_open())
        {
            throw(error(std::string("Error: could not open include file ") + lastt.value));
        }
        includefile.seekg(0, std::ios::end);
        int sourcelength = includefile.tellg();
        includefile.seekg(0, std::ios::beg);
        std::vector<char> source(sourcelength);
        includefile.read(&source[0], sourcelength);
        source.push_back(0);
        includefile.close();
        std::vector<token> includetokens = tokenize(&source[0]);
        parser p(includetokens);
        program *includedefs = p.getprogram();
        for (unsigned int i = 0; i < includedefs->defs.size(); i++)
            prog->defs.push_back(includedefs->defs[i]);
    }
    else
    {
        throw(error("Error: unrecognised preprocessor directive \"" + lastt.value + "\""));
    }
}

// sort out all the syntax stuff, return the type, name and
// value of the constant.
constdef* parser::getconstdef()
{
    resourcep <constdef> def;
    expect(t_type);
    def.obj->valtype = typestrings[lastt.value];
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
        def.obj->return_type = typestrings[lastt.value];
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
        type_t type = typestrings[lastt.value];
        expect(t_name);
        def.obj->args.push_back(argument(type, lastt.value));
        while (accept(t_comma))
        {
            expect(t_type);
            type_t type = typestrings[lastt.value];
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
    type_enum basetype = typestrings[lastt.value];
    expect(t_name);
    vardec.obj->vars.push_back(getvarname_and_type(basetype));  // handles all of the array length arguments and stuff for us too :)
    while (accept(t_comma))
    {
        expect(t_name);
        vardec.obj->vars.push_back(getvarname_and_type(basetype));
    }
    expect(t_semicolon);
    return vardec.release();
}

vardeclaration::varpair parser::getvarname_and_type(type_enum basetype)
{
    vardeclaration::varpair var;
    var.name = lastt.value;
    if (accept(t_lsquareb))
    {
        var.type.type = type_array;
        var.type.second = basetype;
        expect(t_number);
        std::stringstream ss;
        ss << lastt.value;
        ss >> var.type.count;
        expect(t_rsquareb);
    }
    else
    {
        var.type.type = basetype;
    }
    if (accept(t_equals))
        var.initializer = getexpression();
    return var;
}


statement* parser::getstatement()
{
    if (accept(t_goto))
    {
        return getgoto();
    }
    else if (accept(t_if))
    {
        return getif();
    }
    else if (accept(t_while))
    {
        return getwhile();
    }
    else if (accept(t_break))
    {
        resourcep <break_stat> breaks;
        expect(t_semicolon);
        return breaks.release();
    }
    else if (accept(t_continue))
    {
        resourcep <continue_stat> continues;
        expect(t_semicolon);
        return continues.release();
    }
    else if (accept(t_return))
    {
        resourcep <return_stat> returns;
        expect(t_semicolon);
        return returns.release();
    }
    else
    {
        expect(t_name);
        if (t.type == t_equals)
        {
            return getassignment();
        }
        else if (t.type == t_lparen)
        {
            return getfunccall();
        }
        else if (t.type == t_colon)
        {
            return getlabel();
        }
        else
        {
            expect(t_lparen);
            return new statement;
        }
    }
}

assignment* parser::getassignment()
{
    resourcep <assignment> assg;
    assg.obj->name = lastt.value;
    expect(t_equals);
    assg.obj->expr = getexpression();
    expect(t_semicolon);
    return assg.release();
}

goto_stat* parser::getgoto()
{
    resourcep <goto_stat> sgoto;
    sgoto.obj->target = getexpression();
    expect(t_semicolon);
    return sgoto.release();
}

funccall* parser::getfunccall()
{
    resourcep <funccall> fcall;
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

label* parser::getlabel()
{
    resourcep <label> lbl;
    lbl.obj->name = lastt.value;
    expect(t_colon);
    return lbl.release();
}

if_stat* parser::getif()
{
    resourcep <if_stat> ifs;
    expect(t_lparen);
    ifs.obj->expr = getexpression();
    expect(t_rparen);
    ifs.obj->ifblock = getblock();
    if (accept(t_else))
        ifs.obj->elseblock = getblock();
    return ifs.release();
}

while_stat* parser::getwhile()
{
    resourcep <while_stat> whiles;
    expect(t_lparen);
    whiles.obj->expr = getexpression();
    expect(t_rparen);
    whiles.obj->blk = getblock();
    return whiles.release();
}

expression* parser::getexpression()
{
    resourcep <expression> expr;
    if (accept(t_name))
    {
        expr.obj->name = lastt.value;
        if (accept(t_lparen))
        {
            expr.obj->type = exp_funccall;
            if (t.type != t_rparen)
                expr.obj->args.push_back(getexpression());
            while (accept(t_comma))
                expr.obj->args.push_back(getexpression());
            expect(t_rparen);
        }
        else
        {
            expr.obj->type = exp_name;
        }
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
