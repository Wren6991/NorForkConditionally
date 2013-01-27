#include "error.h"
#include "linker.h"

#include <set>
#include <sstream>



void vardict::find_first_available_space(int searchstart)
{
    for (int i = searchstart; i < HEAP_SIZE; i++)
    {
        if (!memory[i])
        {
            first_available_space = i;
            return;
        }
    }
    // If it fell through:
    throw(error("Error: no more free space in heap!"));
}

// Search through memory for a block large enough
// If we find it, block it out and return the start.
int vardict::getspace(int size)
{
    int start = first_available_space;
    while (start < HEAP_SIZE)
    {
        bool enoughSpace = true;
        for (int i = start; i < start + size; i++)
        {
            if (memory[i])
            {
                enoughSpace = false;
                start = i + 1;          // start checking again at the next unchecked location
                break;
            }
        }
        if (enoughSpace)
        {
            for (int i = start; i < start + size; i++)
                memory[i] = true;
            if (start == first_available_space)
                find_first_available_space(first_available_space);       // if we've covered the first known free space, find a new one.
            return start;
        }
    }
}

// Note: returns an offset from the heap start, you'll have to add
// the heap bottom to this value to get a machine address.
int vardict::addvar(std::string name, type_enum type)
{
    variable *var = new variable;
    var->type = type;
    var->offset = getspace(typesizes[type]);
    if (vars.find(name) != vars.end())
        var->next = vars[name];  // push the stack down one...
    vars[name] = var;
}

void vardict::remove(std::string name)
{
    // sanity check:
    std::map<std::string, variable*>::iterator iter = vars.find(name);
    if (iter == vars.end())
        throw(error("Error: tried to free non-existent variable! (Link-time)"));
    // release the memory:
    for (int i = iter->second->offset; i < iter->second->offset + typesizes[iter->second->type]; i++)
        memory[i] = 0;
    if (iter->second->offset < first_available_space)
        first_available_space = iter->second->offset;
    // update the dictionary: pop or remove
    if (iter->second->next)
    {
        variable *old = iter->second;
        iter->second = iter->second->next;
        delete old;
    }
    else
    {
        delete iter->second;
        vars.erase(iter);
    }
}

variable* vardict::getvar(std::string name)
{
    std::map<std::string, variable*>::iterator iter = vars.find(name);
    if (iter == vars.end())
        return 0;
    else
        return iter->second;
}



vardict::vardict()
{
    typesizes[type_int] = 1;
    typesizes[type_pointer] = 2;
    first_available_space = 0;
    for (int i = 0; i < (HEAP_TOP - HEAP_BOTTOM); i++)
        memory.push_back(0);
}

linker::linker()
{
    defined_funcs["nfc"] = 0;
    defined_funcs["nfc4"] = 0;  // hardcoded funcs

    index = 0;
    buffer.reserve(ROM_SIZE);
}


// NB: index refers to the location _about_ to be written. (I.e. next location)
void linker::write8(uint8_t val)
{
    if (index >= ROM_SIZE)
        throw(error("Error: program too big!"));
    buffer.push_back(val);
    index++;
}

void linker::write16(uint16_t val)
{
    write8(val >> 8);           // big endian!
    write8(val & 0xff);
}

void linker::add_object(object *obj)
{
    // check for collisions:
    for (std::set<std::string>::iterator iter = obj->defined_funcs.begin(); iter != obj->defined_funcs.end(); iter++)
    {
        if (defined_funcs.find(*iter) != defined_funcs.end())
        {
            std::stringstream ss;
            ss << "Error: multiple definitions of function \"" << *iter << "\"";
            throw(error(ss.str()));
        }
    }
    // add all the definitions to linker's symbol table:
    for (unsigned int i = 0; i < obj->tree->defs.size(); i++)
    {
        definition *def = obj->tree->defs[i];
        if (def->type != dt_funcdef || !((funcdef*)def)->defined)
        {
            throw(error("Error: linker only accepts defined functions as symbols, wtf are you doing."));
        }
        funcdef *fdef = (funcdef*)def;
        defined_funcs[fdef->name] = fdef;
        definitions.push_back(def);
    }
}

// keep a dict of static vars.
// start at main, pass through the statements. Link in all the hardcoded funcs.
// increment the current machine index as you go.
std::vector<char> linker::link()
{
    if (defined_funcs.find("main") == defined_funcs.end())
    {
        throw(error("Error: no definition of function main."));
    }
    link(defined_funcs["main"]->body);
    return buffer;
}

void linker::link(block *blk)
{
    for (std::vector<vardeclaration*>::iterator iter = blk->declarations.begin(); iter != blk->declarations.end(); iter++)
    {
        for (unsigned int i = 0; i < (*iter)->names.size(); i++)
        {
            vars.addvar((*iter)->names[i], (*iter)->type);
        }
    }
    for (std::vector<statement*>::iterator iter = blk->statements.begin(); iter != blk->statements.end(); iter++)
    {
        link(*iter);
    }
    // TODO: make this a simple scope pop operation.
    for (std::vector<vardeclaration*>::iterator iter = blk->declarations.begin(); iter != blk->declarations.end(); iter++)
    {
        for (unsigned int i = 0; i < (*iter)->names.size(); i++)
        {
            vars.remove((*iter)->names[i]);
        }
    }
}

void linker::link(statement *stat)
{
    switch(stat->type)
    {
    case stat_call:
        link((funccall*)stat);
        break;
    }
}

void linker::link(funccall *call)
{
    if (defined_funcs.find(call->name) == defined_funcs.end())
    {
        std::stringstream ss;
        ss << "Error: no definition for function \"" << call->name << "\"";
        throw(error(ss.str()));
    }
    funcdef *def = defined_funcs[call->name];
    if (!def)   // it's a hardcoded function...
    {
        if (call->name == "nfc")
        {
            write16(call->args[0]->number);
            write16(call->args[1]->number);
            uint16_t next = index + 4;
            write16(next);
            write16(next);
        }
        else if (call->name == "nfc4")
        {
            write16(call->args[0]->number);
            write16(call->args[1]->number);
            write16(call->args[2]->number);
            write16(call->args[3]->number);
        }
    }
}



