#include "compiler.h"
#include "error.h"

#include <sstream>
#include <iomanip>

std::string makeguid(std::string name, int ptr)
{
    std::stringstream ss;
    ss << name << "@" << ptr;
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

void compiler::compile(program *prog)
{
    for (unsigned int i = 0; i < prog->defs.size(); i++)
    {
        definition *def = prog->defs[i];
        if (def->type == dt_constdef)
        {
            constdef *cdef = (constdef*)def;
            variable var;
            var.is_constant = true;
            var.type = cdef->valtype;
            var.name = makeguid(cdef->name, (int)cdef);
            globalsymboltable[var.name] = var;
            currentscope->insert(cdef->name, var);
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
                variable var;
                var.type = arg->type;
                var.name = makeguid(arg->name, (int)arg);
                globalsymboltable[var.name] = var;
                currentscope->insert(arg->name, var);
            }
            compile(fdef->body));
            popscope();
        }
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
            variable var;                                   // this block is repeated a few times, should factor it out
            var.type = dec->type;
            var.name = makeguid(dec->names[j], (int)dec);
            globalsymboltable[var.name] = var;
            currentscope->insert(dec->names[j], var);



            //////////////////////////////////////////////HERE!

        }
    }
    popscope();
}
