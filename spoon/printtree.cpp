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
    std::cout << "constant " << def->valtype.getname() << " " << def->name << ": " << def->value << "\n";
}

void printtree(funcdef *def, int indentation)
{
    indent(indentation);
    std::cout << "\nDefinition of function " << def->name << ":\n";
    if (def->defined)
    {
        printtree(def->body, indentation);
    }
    else
    {
        indent(indentation + 1);
        std::cout << "(declaration only)\n";
    }
}

void printtree(macrodef *def, int indentation)
{
    indent(indentation);
    std::cout << "\nDefinition of macro " << def->name << ":\n";
    printtree(def->body, indentation);
}

void printtree(block *blk, int indentation)
{
    indent(indentation);
    std::cout << "{\n";
    for (std::vector<vardeclaration*>::iterator iter = blk->declarations.begin(); iter != blk->declarations.end(); iter++)
    {
        printtree(*iter, indentation + 1);
    }
    for (std::vector<statement*>::iterator iter = blk->statements.begin(); iter != blk->statements.end(); iter++)
    {
        printtree(*iter, indentation + 1);
    }
    indent(indentation);
    std::cout << "}\n";
}

void printtree(vardeclaration *decl, int indentation)
{
    indent(indentation);
    std::cout << "declared " << decl->type.getname() << " " << decl->names[0] << "\n";
}

void printtree(statement *stat, int indentation)
{
    indent(indentation);
    if (stat->type == stat_call)
    {
        funccall *fcall = (funccall*)stat;
        std::cout << "call to function " << fcall->name << "\n";
        for (unsigned int i = 0; i < fcall->args.size(); i++)
        {
            indent(indentation + 1);
            std::cout << "argument: ";
            printtree(fcall->args[i]);
            std::cout << "\n";
        }
    }
    else if (stat->type == stat_goto)
    {
        goto_stat *sgoto = (goto_stat*)stat;
        std::cout << "goto ";
        printtree(sgoto->target);
        std::cout << "\n";
    }
    else if (stat->type == stat_label)
    {
        std::cout << "Label: " << ((label*)stat)->name << "\n";
    }
    else if (stat->type == stat_if)
    {
        std::cout << "If ";
        printtree(((if_stat*)stat)->expr);
        std::cout << " Then\n";
        printtree(((if_stat*)stat)->ifblock, indentation);
        if (((if_stat*)stat)->elseblock)
        {
            indent(indentation);
            std::cout << "Else\n";
            printtree(((if_stat*)stat)->elseblock, indentation);
        }
    }
    else if (stat->type == stat_while)
    {
        std::cout << "While ";
        printtree(((while_stat*)stat)->expr);
        std::cout << " Do\n";
        printtree(((while_stat*)stat)->blk, indentation);
    }


}

void printtree(expression *expr)
{
    if (expr->type == exp_name)
        std::cout << expr->name;
    else
        std::cout << expr->number;
}
