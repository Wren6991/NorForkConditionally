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
        printtree(prog->defs[i], 0);
    }
}

void printtree(definition *def, int indentation)
{
    switch(def->type)
    {
        case dt_constdef:
            printtree((constdef*)def, indentation);
            break;
        case dt_funcdef:
            printtree((funcdef*)def, indentation);
            break;
        case dt_macrodef:
            printtree((macrodef*)def, indentation);
            break;
        case dt_vardec:
            printtree((vardeclaration*)def, indentation);
            break;
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
    std::cout << "\nDefinition of function " << def->name << ": (";
    if (def->args.size() == 0)
        std::cout << "void";
    else
        for (unsigned int i = 0; i < def->args.size(); i++)
        {
            std::cout << def->args[i].type.getname();
            if (i < def->args.size() - 1)
                std::cout << ", ";
        }
    std::cout << ") => " << def->return_type.getname() << ":\n";
    indent(indentation);
    std::cout << " Depends on:\n";
    for (std::set<std::string>::iterator iter = def->dependson.begin(); iter != def->dependson.end(); iter++)
    {
        indent(indentation);
        std::cout << "  " << (*iter) << "\n";
    }
    if (def->exported)
    {
        indent(indentation + 1);
        std::cout << "(exported)\n";
    }
    else if (def->defined)
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
    std::cout << "\nDefinition of macro " << def->name << ": (";
    if (def->args.size() == 0)
    {
        std::cout << "<none>";
    }
    else
    {
        for (unsigned int i = 0; i < def->args.size(); i++)
        {
            std::cout << def->args[i];
            if (i < def->args.size() - 1)
                std::cout << ", ";
        }
    }
    std::cout << "):\n";
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
    for (unsigned int i = 0; i < decl->vars.size(); i++)
    {
        indent(indentation);
        std::cout << "Declared " << decl->vars[i].type.getname() << " " << decl->vars[i].name << "\n";
    }

}

void printtree(statement *stat, int indentation)
{
    indent(indentation);
    if (stat->type == stat_block)
        printtree((block*)stat, indentation);
    if (stat->type == stat_call)
    {
        funccall *fcall = (funccall*)stat;
        std::cout << "Call to function " << fcall->name << "\n";
        for (unsigned int i = 0; i < fcall->args.size(); i++)
        {
            indent(indentation + 1);
            std::cout << "Argument: ";
            printtree(fcall->args[i]);
            std::cout << "\n";
        }
    }
    else if (stat->type == stat_goto)
    {
        goto_stat *sgoto = (goto_stat*)stat;
        std::cout << "Goto ";
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
    else if (stat->type == stat_assignment)
    {
        assignment *assg = (assignment*)stat;
        std::cout << "Setting " << assg->name << " to ";
        printtree(assg->expr);
        std::cout << "\n";
    }
    else if (stat->type == stat_break)
    {
        std::cout << "Break\n";
    }
    else if (stat->type == stat_continue)
    {
        std::cout << "Continue\n";
    }
    else if (stat->type == stat_return)
    {
        std::cout << "Return\n";
    }


}

void printtree(expression *expr)
{
    if (expr->type == exp_name)
        std::cout << expr->name;
    else if (expr->type == exp_number)
        std::cout << expr->number;
    else if (expr->type == exp_not)
    {
        std::cout << "!(";
        printtree(expr->args[0]);
        std::cout << ")";
    }
    else if (expr->type == exp_or)
    {
        std::cout << "(";
        printtree(expr->args[0]);
        std::cout << " || ";
        printtree(expr->args[1]);
        std::cout << ")";
    }
    else if (expr->type == exp_and)
    {
        std::cout << "(";
        printtree(expr->args[0]);
        std::cout << " && ";
        printtree(expr->args[1]);
        std::cout << ")";
    }
    else if (expr->type == exp_funccall)
    {
        std::cout << expr->name << "(";
        for (unsigned i = 0; i < expr->args.size(); i++)
        {
            printtree(expr->args[i]);
            if (i < expr->args.size() - 1)
                std::cout << ", ";
        }
        std::cout << ")";
    }
    else if (expr->type == exp_string)
    {
        std::cout << "\"" << expr->name << "\"";
    }

}
