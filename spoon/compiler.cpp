#include "compiler.h"
#include "error.h"

#include <sstream>
#include <iomanip>

#include <iostream>

std::string makeguid(std::string name, int ptr)
{
    std::stringstream ss;
    ss << name << "@" << std::hex << ptr;
    return ss.str();
}

scope::scope(scope *_parent)
{
    parent = _parent;
}

void scope::insert(std::string name, variable var)
{
    variables[name] = var;
}

variable& scope::get(std::string name)
{
    if (variables.find(name) != variables.end())
        return variables[name];
    else if (parent)
        return parent->get(name);
    else
        throw(error("Error: undefined name _!"));
}

bool scope::exists(std::string name)
{
    if (variables.find(name) != variables.end())
        return true;
    else if (parent)
        return parent->exists(name);
    else
        return false;
}


compiler::compiler()
{
    globalscope = new scope();
    currentscope = globalscope;
    func_signature sig;
    sig.push_back(type_pointer);
    sig.push_back(type_pointer);
    functions["nfc"] = sig;
    sig.push_back(type_pointer);
    sig.push_back(type_pointer);
    functions["nfc4"] = sig;
}

void compiler::pushscope()
{
    currentscope = new scope(currentscope);
}

void compiler::popscope()
{
    scope *oldscope = currentscope;
    currentscope = currentscope->parent;
    delete oldscope;
}

void compiler::addvar(std::string name, type_enum type, int ptr, bool isConstant, int constvalue)
{
    variable var;
    var.name = makeguid(name, ptr);
    var.type = type;
    var.is_constant = isConstant;
    if (isConstant)
        var.value = constvalue;
    globalsymboltable[var.name] = var;
    currentscope->insert(name, var);
}

void compiler::compile(program *prog)
{
    for (unsigned int i = 0; i < prog->defs.size(); i++)
    {
        definition *def = prog->defs[i];
        if (def->type == dt_constdef)
        {
            constdef *cdef = (constdef*)def;
            addvar(cdef->name, cdef->valtype, (int)cdef, true, cdef->value);
        }
        else if (def->type == dt_macrodef)
        {
            //eh, don't really know what to do here yet
            // guess I push a new scope with the arg names in it and scan the body?
        }
        else if (def->type == dt_funcdef)
        {
            funcdef *fdef = (funcdef*)def;
            pushscope();
            for (unsigned int i = 0; i < fdef->args.size(); i++)
            {
                argument *arg = &(fdef->args[i]);
                addvar(arg->name, arg->type, (int)arg);
            }
            compile(fdef->body);
            popscope();
        }
    }
    std::cout << "\nGlobal symbol table:\n";
    std::map<std::string, variable>::iterator iter = globalsymboltable.begin();
    for(; iter != globalsymboltable.end(); iter++)
    {
        std::cout << iter->first << "\n";
    }
}

void compiler::compile(block *blk)
{
    pushscope();
    for (unsigned int i = 0; i < blk->declarations.size(); i++)
    {
        vardeclaration *dec = blk->declarations[i];
        for (unsigned int j = 0; j < dec->names.size(); j++)
        {
            addvar(dec->names[j], dec->type, (int)dec);
        }
    }
    for (unsigned int i = 0; i < blk->statements.size(); i++)
    {
        statement *stat = blk->statements[i];

        if (stat->type == stat_call)
        {
            funccall *fcall = (funccall*)stat;

            expression *expr;
            for (unsigned int argnum = 0; argnum < fcall->args.size(); argnum++)
            {
                expr = fcall->args[argnum];
                if (expr->type == exp_name)
                {
                    if (!currentscope->exists(expr->name))
                    {
                        std::stringstream ss;
                        ss << "Error: undeclared name \"" << expr->name << "\" in expression.";
                        throw(error(ss.str()));
                    }
                    expr->name = currentscope->get(expr->name).name;        // replace local name with globally unique name;
                    if (globalsymboltable[expr->name].is_constant)          // if it's a constant, fetch the value from the global symbol table and replace.
                    {
                        expr->type = exp_number;
                        expr->number = globalsymboltable[expr->name].value;
                    }
                }
            }
        }
    }
    popscope();
}
