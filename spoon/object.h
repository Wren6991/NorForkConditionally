#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include "syntaxtree.h"

#include <set>

struct object
{
    std::set<std::string> defined_funcs;
    program *tree;
};

#endif // OBJECT_H_INCLUDED
