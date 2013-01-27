#ifndef LINKER_H_INCLUDED
#define LINKER_H_INCLUDED

#include "object.h"

#include <map>
#include <stdint.h>
#include <vector>

const int ROM_SIZE = 32 * 1024;
const int RAM_SIZE = 16 * 1024;
const int HEAP_TOP = 0xbfff;
const int HEAP_BOTTOM = 0x8000;
const int HEAP_SIZE = HEAP_TOP - HEAP_BOTTOM + 1;


// TODO: might be better to just have a stack of maps for scope, rather
// than this multiple-linked-list juggling bullshit.
struct variable
{
    type_enum type;
    variable *next;     // for stack operations we can build a linked list, in case we get the same symbol twice. (Shadowing)
    int offset;
    variable() {next = 0;}
};

class vardict
{
    std::map<type_enum, int> typesizes;
    std::map<std::string, variable*> vars;
    std::vector<bool> memory;
    int first_available_space;
    int getspace(int size);
    void find_first_available_space(int searchstart = 0);
public:
    int addvar(std::string name, type_enum type);
    void remove(std::string name);
    variable* getvar(std::string name);
    bool exists(std::string name);
    vardict();
};

class linker
{
    vardict vars;
    std::map<std::string, funcdef*> defined_funcs;
    std::vector<definition*> definitions;
    int index;
    std::vector<char> buffer;
    void write8(uint8_t);
    void write16(uint16_t);
    void link(block*);
    void link(statement*);
    void link(funccall*);
    uint16_t evaluate(expression*);
public:
    linker();
    void add_object(object* obj);
    std::vector<char> link();
};

#endif // LINKER_H_INCLUDED
