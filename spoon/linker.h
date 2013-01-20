#ifndef LINKER_H_INCLUDED
#define LINKER_H_INCLUDED

#include "object.h"

#include <set>
#include <vector>


class linker
{
    std::set<std::string> defined_funcs;
    std::vector<definition*> definitions;
public:
    void add_object(object* obj);
    vector<char> link();
};

#endif // LINKER_H_INCLUDED
