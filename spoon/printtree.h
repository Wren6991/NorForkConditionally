#ifndef PRINTTREE_H_INCLUDED
#define PRINTTREE_H_INCLUDED

#include "syntaxtree.h"

void printtree(program *prog, int indentation = 0);
void printtree(definition *def, int indentation);
void printtree(funcdef *def, int indentation);
void printtree(macrodef *def, int indentation);
void printtree(constdef *def, int indentation);
void printtree(block *blk, int indentation);
void printtree(vardeclaration *decl, int indentation);
void printtree(statement *stat, int indentation);
void printtree(expression *expr);

#endif // PRINTTREE_H_INCLUDED
