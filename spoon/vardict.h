#ifndef _VARDICT_H_
#define _VARDICT_H_

#include <map>
#include <string>
#include <vector>

#include "object.h"
#include "linkval.h"

class vardict;

struct variable
{
    friend class vardict;
    type_t type;
    variable *next;     // for stack operations we can build a linked list, in case we get the same symbol twice. (Shadowing)
    linkval address;
    variable() {next = 0;}
private:
    int offset;     // we don't want to use this by mistake...
};

class vardict
{
    std::map<std::string, variable*> vars;
    std::vector<bool> memory_in_use;
    std::vector<bool> has_been_used;
    int first_available_space;
    int getspace(int size);
    void find_first_available_space(int searchstart = 0);
    std::vector<std::vector<std::string> > tempscopes;       // in an if/while test we may use multiple temp locations, and we don't want them to clobber each other, so we keep track of temps and clean up after test finished.
public:
    bool start_from_top;
    linkval addvar(std::string name, type_t type);
    void registervar(std::string name, type_t type, linkval address);    // for when we want to push an existing address as a var, and the memory is already allocated. (it's removed in the same way)
    void remove(std::string name);
    variable* getvar(std::string name);
    bool exists(std::string name);
    void push_function_scope();     // so functions can't clobber each other's memory, we mark all memory used by other functions as currently in use.
    void push_temp_scope();
    void pop_temp_scope();
    void remove_on_pop(std::string name);
    vardict();
};

#endif // _VARDICT_H_

