#ifndef LINKER_H_INCLUDED
#define LINKER_H_INCLUDED

#include "object.h"

#include <map>
#include <stdint.h>
#include <vector>


class linker
{
    std::map<std::string, funcdef*> defined_funcs;
    std::vector<definition*> definitions;
    int index;
    std::vector<char> buffer;
    void write8(uint8_t);
    void write16(uint16_t);
    void link(statement*);
    void link(funccall*);
public:
    linker();
    void add_object(object* obj);
    std::vector<char> link();
};

#endif // LINKER_H_INCLUDED
