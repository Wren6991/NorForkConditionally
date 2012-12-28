#include "printtree.h"

#include <iostream>

void indent(int n)
{
    for (int i = 0; i < n; i++)
    {
        std::cout << "  ";
    }
}

std::string typenames[] =
{
    "void",
    "int",
    "pointer"
};

void printtree(program *prog, int indentation)
{
    for (unsigned int i = 0; i < prog->defs.size(); i++)
    {
        switch(prog->defs[i]->type)
        {
            case dt_constdef:
                printtree((constdef*)(prog->defs[i]), indentation);
                break;
            case dt_funcdef:
                printtree((funcdef*)(prog->defs[i]), indentation);
                break;
            case dt_macrodef:
                printtree((macrodef*)(prog->defs[i]), indentation);
                break;
        }
    }
}

void printtree(constdef *def, int indentation)
{
    indent(indentation);
    std::cout << "const " << def->valtype << " " << def->name << " = " << def->value << ";\n";
}

void printtree(funcdef *def, int indentation)
{
    indent(indentation);
    std::cout << "Definition of " << def->name << "\n";
}

void printtree(macrodef *def, int indentation)
{
    indent(indentation);
    std::cout << "Definition of " << def->name << "\n";
}
