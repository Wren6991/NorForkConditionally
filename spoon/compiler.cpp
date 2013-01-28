#include "compiler.h"
#include "error.h"

#include <sstream>
#include <iomanip>

#include <iostream>

// take a local symbol and a globally unique pointer value,
// concatenate into a guid string
std::string makeguid(std::string name, int ptr)
{
    std::stringstream ss;
    ss << name << "@" << std::hex << ptr;
    return ss.str();
}


// Scope definitions: //
scope::scope(scope *_parent)
{
    parent = _parent;
}

void scope::insert(std::string name, symbol var)
{
    variables[name] = var;
}

// If this scope has a copy then return that, else refer upwards.
symbol& scope::get(std::string name)
{
    if (variables.find(name) != variables.end())
        return variables[name];
    else if (parent)
        return parent->get(name);
    else
        throw(error("Error: undefined name _!"));
}

// If the current scope has a match then return true, else refer upwards
bool scope::exists(std::string name)
{
    if (variables.find(name) != variables.end())
        return true;
    else if (parent)
        return parent->exists(name);
    else
        return false;
}

// Compiler definitions //

// set up scopes and signatures:
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

// add a new variable to the current scope: it is saved in the current scope
// with its local name, and in the global symbol table with its global name,
// so we can do type checking and stuff in the second pass.
void compiler::addvar(std::string name, type_enum type, int ptr, bool isConstant, int constvalue)
{
    symbol var;
    var.name = makeguid(name, ptr);
    var.type = type;
    var.is_constant = isConstant;
    if (isConstant)
        var.value = constvalue;
    globalsymboltable[var.name] = var;
    currentscope->insert(name, var);
}

// Run through all the definitions that make up the program - delegate to the proper
// functions depending on the definition type.
object* compiler::compile(program *prog)
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
            compile((funcdef*)def);
        }
    }
    std::cout << "\nGlobal symbol table:\n";
    std::map<std::string, symbol>::iterator iter = globalsymboltable.begin();
    for(; iter != globalsymboltable.end(); iter++)
    {
        std::cout << iter->first << "\n";
    }

    object *obj = new object;
    obj->defined_funcs = defined_funcs;
    obj->tree = prog;
    std::vector<definition*>::iterator idef = obj->tree->defs.begin();
    while (idef != obj->tree->defs.end())
    {
        if ((*idef)->type != dt_funcdef || !((funcdef*)*idef)->defined)
        {
            idef = obj->tree->defs.erase(idef);     // strip out everything apart from function definitions. (macros have already been subbed by this point)
        }
        else
        {
            idef++;
        }
    }
    return obj;
}


// check for signature conflicts, check for definition conficts,
// and then compile the function body if there is one.
void compiler::compile(funcdef *fdef)
{
    func_signature sig;
    pushscope();
    for (unsigned int i = 0; i < fdef->args.size(); i++)
    {
        argument *arg = &(fdef->args[i]);
        addvar(arg->name, arg->type, (int)arg);
        sig.push_back(arg->type);
    }
    if (functions.find(fdef->name) != functions.end())
    {
        if (sig != functions[fdef->name])
        {
            std::stringstream ss;
            ss << "Error: conflicting type declarations for function \"" << fdef->name << "\"";
            throw(error(ss.str()));
        }
    }
    else
    {
        functions[fdef->name] = sig;
    }
    if (defined_funcs.find(fdef->name) == defined_funcs.end()) // i.e. function is currently undefined
    {
        if (fdef->defined)
        {
            compile(fdef->body);
            defined_funcs.insert(fdef->name);
        }
    }
    else    // (if function has already been defined)
    {
        if (fdef->defined)  // and we have another definition in this func def...
        {
            std::stringstream ss;
            ss << "Error: conflicting definitions of function \"" << fdef->name << "\"";
            throw(error(ss.str()));
        }
    }
    popscope();
}

// Push all the variable declarations into a new scope.
// run through all the statements and delegate compilation based
// on statement type
void compiler::compile(block *blk)
{
    pushscope();
    for (unsigned int i = 0; i < blk->declarations.size(); i++)
    {
        vardeclaration *dec = blk->declarations[i];
        for (unsigned int j = 0; j < dec->names.size(); j++)
        {
            addvar(dec->names[j], dec->type, (int)dec);
            dec->names[j] = currentscope->get(dec->names[j]).name;      // replace the declaration with the global name of the var; makes linking easier.
        }
    }
    for (unsigned int i = 0; i < blk->statements.size(); i++)
    {
        statement *stat = blk->statements[i];

        if (stat->type == stat_call)
        {
            compile((funccall*)stat);
        }
        else if (stat->type == stat_goto)
        {
            compile((goto_stat*)stat);
        }
    }
    popscope();
}


// compile ALL the arguments!
void compiler::compile(funccall *fcall)
{
    if (functions.find(fcall->name) == functions.end())
    {
        std::stringstream ss;
        ss << "Error: implicit declaration of function \"" << fcall->name << "\"";
        throw(error(ss.str()));
    }
    for (unsigned int argnum = 0; argnum < fcall->args.size(); argnum++)
    {
        compile(fcall->args[argnum]);
    }
}

// just compile the target expression...
void compiler::compile(goto_stat *sgoto)
{
    compile(sgoto->target);
}

// To compile an expression:
// - if it's a name, check that it exists in the current scope
//     - if so, replace the local name with the global one.
//     - if it refers to a constant, swap the constant value in for the name.
// - otherwise, leave it
void compiler::compile(expression *expr)
{
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
