#include "error.h"
#include "linker.h"

#include <set>
#include <sstream>

const int MAX_MEM_SIZE = 32 * 1024;


linker::linker()
{
    defined_funcs["nfc"] = 0;
    defined_funcs["nfc4"] = 0;  // hardcoded funcs

    index = 0;
    buffer.reserve(MAX_MEM_SIZE);
}

void linker::write8(uint8_t val)
{
    ++index;
    if (index >= MAX_MEM_SIZE)
        throw(error("Error: program too big!"));
    buffer.push_back(val);
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
            throw(error("Error: linker only accepts defined functions as symbols, wtf."));
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
    block *dmain = defined_funcs["main"]->body;
    std::vector<statement*>::iterator iter = dmain->statements.begin();
    for(; iter != dmain->statements.end(); iter++)
    {
        link(*iter);
    }
    return buffer;
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



