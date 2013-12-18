#include "vardict.h"

#include <iostream>

#include "error.h"
#include "linker.h"

void vardict::find_first_available_space(int searchstart)
{
    for (int i = 0; i < HEAP_SIZE; i++)
    {
        if (!memory_in_use[i])
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
    int pos = first_available_space;
    while (pos < HEAP_SIZE)
    {
        int start = pos;
        bool enoughSpace = true;
        for (; pos < start + size && enoughSpace; pos++)
        {
            if (memory_in_use[pos])
            {
                enoughSpace = false;
                pos++;          // start checking again at the next unchecked location
            }
        }
        if (enoughSpace)
        {
            for (int i = start; i < start + size; i++)
            {
                memory_in_use[i] = true;
                has_been_used[i] = true;
            }
            if (start == first_available_space)
                find_first_available_space(first_available_space);       // if we've covered the first known free space, find a new one.
            return start;
        }
    }
    throw(error("Error: no more free space in heap!"));
}

// Note: returns an offset from the heap start, you'll have to add
// the heap bottom to this value to get a machine address.
linkval vardict::addvar(std::string name, type_t type)
{
    variable *var = new variable;
    var->type = type;
    var->offset = getspace(type.getstoragesize());
    if (vars.find(name) != vars.end())
        var->next = vars[name];  // push the stack down one...
    vars[name] = var;
    if (start_from_top)
        var->address = HEAP_TOP - var->offset - type.getstoragesize() + 1;
    else
        var->address = linkval("__program_end") + var->offset;
#ifdef EBUG
    std::cout << "adding var " << name << " (" << type.getstoragesize() << ") 0x" << std::hex << var->offset << " -> " << var->address.literal << "\n";
#endif
    return var->address;
}

void vardict::registervar(std::string name, type_t type, linkval address)
{
    variable *var = new variable;
    var->type = type;
    var->offset = -1;
    if (vars.find(name) != vars.end())
        var->next = vars[name];
    vars[name] = var;
    var->address = address;
}

void vardict::remove(std::string name)
{
    // sanity check:
    std::map<std::string, variable*>::iterator iter = vars.find(name);
    if (iter == vars.end())
        throw(error("Error: tried to free non-existent variable! (Link-time)"));
    variable *var = iter->second;
    // release the memory:
    if (var->offset >= 0)   // registervar() doesn't allocate memory, so it sets offset to -1.
    {
        for (int i = var->offset; i < var->offset + var->type.getstoragesize(); i++)
            memory_in_use[i] = 0;
        if (var->offset < first_available_space)
            first_available_space = var->offset;
    }
    // update the dictionary: pop or remove
    if (var->next)
    {
        iter->second = var->next;
        delete var;
    }
    else
    {
        delete var;
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

bool vardict::exists(std::string name)
{
    return vars.find(name) != vars.end();
}

// When we link a new function body, we mark all memory that is used by other functions as currently in use.
// This means memory is still packed as efficiently as possible within functions, but functions
// can't clobber memory used by others when they declare vars.
void vardict::push_function_scope()
{
    memory_in_use = has_been_used;
}

void vardict::push_temp_scope()
{
    tempscopes.push_back(std::vector<std::string>());
}

void vardict::pop_temp_scope()
{
    if (tempscopes.size() < 1)
        throw(error("Linker error: no tempscope to pop!"));
    std::vector<std::vector<std::string> >::iterator iter = tempscopes.end() - 1;
    std::vector<std::string> &names = *iter;
    for (unsigned int i = 0; i < names.size(); i++)
    {
#ifdef EBUG
        std::cout << "popping " << names[i] << "\n";
#endif
        remove(names[i]);
    }
    tempscopes.erase(iter);
}

void vardict::remove_on_pop(std::string name)
{
    if (tempscopes.size() < 1)
        remove(name);               // if we're not in a temp scope, assume we can just chuck the variable away.
    else
        tempscopes[tempscopes.size() - 1].push_back(name);
}

vardict::vardict()
{
    first_available_space = 0;
    start_from_top = true;
    for (int i = 0; i < HEAP_SIZE; i++)
    {
        memory_in_use.push_back(0);
        has_been_used.push_back(0);
    }
}

