#ifndef BUILTINS_H_INCLUDED
#define BUILTINS_H_INCLUDED

#include "linker.h"
#include "syntaxtree.h"
#include <vector>

int (linker::*emit_func_val)(std::vector<expression*>&) = 0;


#endif // BUILTINS_H_INCLUDED
