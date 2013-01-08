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
    std::cout << "constant " << def->valtype << " " << def->name << ": " << def->value << "\n";
}

void printtree(funcdef *def, int indentation)
{
    indent(indentation);
    std::cout << "Definition of function " << def->name << ":\n";
    printtree(def->body, indentation);
}

void printtree(macrodef *def, int indentation)
{
    indent(indentation);
    std::cout << "Definition of macro " << def->name << ":\n";
    printtree(def->body, indentation);
}

void printtree(block *blk, int indentation)
{
    indent(indentation);
    std::cout << "STARTBLOCK\n";
    for (std::vector<vardeclaration*>::iterator iter = blk->declarations.begin(); iter != blk->declarations.end(); iter++)
    {
        printtree(*iter, indentation + 1);
    }
    for (std::vector<statement*>::iterator iter = blk->statements.begin(); iter != blk->statements.end(); iter++)
    {
        printtree(*iter, indentation + 1);
    }
    indent(indentation);
    std::cout << "ENDBLOCK\n";
}

void printtree(vardeclaration *decl, int indentation)
{
    indent(indentation);
    std::cout << "declared " << decl->type << decl->names[0] << "\n";
}

void printtree(statement *stat, int indentation)
{
    indent(indentation);
    if (stat->type == stat_call)
        std::cout << "call to function " << ((funccall*)stat)->name << "\n";
}
