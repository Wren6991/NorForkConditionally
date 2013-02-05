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

void throw_type_error(std::string context, type_enum expected, type_enum got)
{
    if (expected < 0 or expected >= n_types)
        expected = type_none;
    if (got < 0 or got >= n_types)
        got = type_none;
    throw (error("Error: Type mismatch in " + context + ": was expecting " + friendly_type_names[expected] +
                 " but got " + friendly_type_names[got]));
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
    sig.args_must_match = false;
    sig.arg_types.push_back(type_number);
    sig.arg_types.push_back(type_number);
    functions["nfc"] = sig;
    sig.arg_types.push_back(type_number);
    sig.arg_types.push_back(type_number);
    functions["nfc4"] = sig;
    func_signature val_sig;
    val_sig.return_type = type_pointer;
    val_sig.arg_types.push_back(type_int);
    functions["val"] = val_sig;
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
            compile((macrodef*)def);
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
        if (((*idef)->type != dt_funcdef || !((funcdef*)*idef)->defined) && (*idef)->type != dt_macrodef)
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

void compiler::compile(macrodef *mdef)
{
    //std::string guid = makeguid(mdef->name, (int)mdef);
    pushscope();
    for (unsigned int i = 0; i < mdef->args.size(); i++)
    {
        addvar(mdef->args[i], type_label, (int)&(mdef->args[i]));
        mdef->args[i] = currentscope->get(mdef->args[i]).name;
    }
    if (defined_funcs.find(mdef->name) != defined_funcs.end())
    {
        std::stringstream ss;
        ss << "Error: conflicting definitions of macro \"" << mdef->name << "\"";
        throw(error(ss.str()));
    }
    compile(mdef->body);
    popscope();
    defined_funcs.insert(mdef->name);
}


// check for signature conflicts, check for definition conficts,
// and then compile the function body if there is one.
void compiler::compile(funcdef *fdef)
{
    func_signature sig;
    sig.return_type = fdef->return_type;
    pushscope();
    for (unsigned int i = 0; i < fdef->args.size(); i++)
    {
        argument *arg = &(fdef->args[i]);
        addvar(arg->name, arg->type, (int)arg);
        arg->name = currentscope->get(arg->name).name;
        sig.arg_types.push_back(arg->type);
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
    // scan through for labels: (because they break the forward-view scoping rule)
    for (unsigned int i = 0; i < blk->statements.size(); i++)
    {
        // NOTE: we put a label in the declarations, but put a pointer variable in the current scope. Compiler thinks pointer, linker knows label.
        if (blk->statements[i]->type == stat_label)
        {
            addvar(((label*)blk->statements[i])->name, type_pointer, (int)(blk->statements[i]));
            vardeclaration *dec = new vardeclaration;
            dec->type = type_label;
            dec->names.push_back(currentscope->get(((label*)blk->statements[i])->name).name);
            blk->declarations.push_back(dec);
        }
    }

    // compile the remaining statements:
    for (unsigned int i = 0; i < blk->statements.size(); i++)
    {
        statement *stat = blk->statements[i];

        switch (stat->type)
        {
        case stat_call:
            compile((funccall*)stat);
            break;
        case stat_goto:
            compile((goto_stat*)stat);
            break;
        case stat_label:
            ((label*)stat)->name = currentscope->get(((label*)stat)->name).name;        // replace local label with globally unique one
            break;
        case stat_if:
            compile((if_stat*)stat);
            break;
        case stat_while:
            compile((while_stat*)stat);
            break;
        case stat_assignment:
            compile((assignment*)stat);
            break;
        default:
            throw(error("Error: unrecognised statement type"));
        }
    }
    popscope();
}

// compile ALL the arguments!
void compiler::compile(funccall *fcall)
{
    if (functions.find(fcall->name) == functions.end() &&
        defined_funcs.find(fcall->name) == defined_funcs.end())
    {
        std::stringstream ss;
        ss << "Error: implicit declaration of function \"" << fcall->name << "\"";
        throw(error(ss.str()));
    }
    func_signature &sig = functions[fcall->name];
    if (fcall->args.size() < sig.arg_types.size())
        throw(error("Error: not enough arguments to function " + fcall->name));
    else if (fcall->args.size() > sig.arg_types.size())
        throw(error("Error: too many arguments to function " + fcall->name));

    for (unsigned int argnum = 0; argnum < fcall->args.size(); argnum++)
    {
        compile(fcall->args[argnum]);
        if (sig.args_must_match && !match_types(sig.arg_types[argnum], fcall->args[argnum]->val_type))
            throw_type_error(std::string("argument ") + "" + " to function " + fcall->name, sig.arg_types[argnum], fcall->args[argnum]->val_type);
    }
}

// just compile the target expression...
void compiler::compile(goto_stat *sgoto)
{
    compile(sgoto->target);
}

void compiler::compile(label *lbl)
{
    //... do we really need this?
}

// we only need to compile the expression and if bodies:
// the actual code generation and labelling happens at link time.
void compiler::compile(if_stat *ifs)
{
    compile(ifs->expr);
    compile(ifs->ifblock);
    if (ifs->elseblock)
        compile(ifs->elseblock);
}

void compiler::compile(while_stat *whiles)
{
    compile(whiles->expr);
    compile(whiles->blk);
}

void compiler::compile(assignment *assg)
{
    if (!currentscope->exists(assg->name))
    {
        throw(error("Error: undeclared variable " + assg->name));
    }
    assg->name = currentscope->get(assg->name).name;
    compile(assg->expr);
    type_enum assg_type = globalsymboltable[assg->name].type;
    // Check for type mismatch:
    if (!match_types(assg_type, assg->expr->val_type))
        throw_type_error("assignment of variable " + assg->name.substr(0, assg->name.find("@")), assg_type, assg->expr->val_type);
}

// To compile an expression:
// - if it's a name, check that it exists in the current scope
//     - if so, replace the local name with the global one.
//     - if it refers to a constant, swap the constant value in for the name.
// - otherwise, leave it
// - get its type once we've compiled it
void compiler::compile(expression *expr)
{
    bool typeAlreadyDetermined = false;
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
            expr->val_type = globalsymboltable[expr->name].type;
            typeAlreadyDetermined = true;
        }
    }
    else if (expr->type == exp_funccall)
    {

        func_signature &sig = functions[expr->name];
        if (expr->args.size() < sig.arg_types.size())
            throw(error("Error: not enough arguments to function " + expr->name));
        else if (expr->args.size() > sig.arg_types.size())
            throw(error("Error: too many arguments to function " + expr->name));
        for (unsigned int i = 0; i < expr->args.size(); i++)
        {
            compile(expr->args[i]);
            if (sig.args_must_match && !match_types(sig.arg_types[i], expr->args[i]->val_type))
                throw_type_error(std::string("argument ") + "" + " to function " + expr->name, sig.arg_types[i], expr->args[i]->val_type);
        }
    }
    else if (expr->type != exp_number)
    {
        throw(error("Error: attempted to compile unknown expression type"));
    }
    // tell the compiler to make note of the type: (if we haven't already gotten it from the symbol table)
    if (!typeAlreadyDetermined)
        gettype(expr);
}

// Find the type of the expression (possibly by looking at subexpressions), and
// then note it in the type field.
// As this is called at the end of the end of expression compilation, it is
// guaranteed that subexpressions will already be compiled, so their types
// will be known.
void compiler::gettype(expression *expr)
{
    if (expr->type == exp_name)
    {
        expr->val_type = globalsymboltable[expr->name].type;
    }
    else if (expr->type == exp_number)
    {
        expr->val_type = type_number;
    }
    else if (expr->type == exp_funccall)
    {
        expr->val_type = functions[expr->name].return_type;
    }
}


// check that received matches accepted, and refine the generic "number" type.
// if there is no possible match, return false.
bool compiler::match_types(type_enum expected, type_enum &received)
{
    if (expected == received)
        return true;

    if ((expected == type_int || expected == type_pointer) && received == type_number)
    {
        received = expected;   // replace generic number with int/pointer.
        return true;
    }
    else
    {
        return false;
    }
}
