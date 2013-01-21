#ifndef LINKER_H_INCLUDED
#define LINKER_H_INCLUDED

#include "object.h"

#include <map>
#include <vector>


class linker
{
    std::map<std::string, definition*> defined_funcs;       // possibly: replace definition* with funcdef* ? as nothing else should be exported by compiler.
    std::vector<definition*> definitions;
public:
    linker();
    void add_object(object* obj);
    std::vector<char> link();
};

#endif // LINKER_H_INCLUDED
