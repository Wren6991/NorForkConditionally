#ifndef LINKER_H_INCLUDED
#define LINKER_H_INCLUDED

#include "object.h"
#include "linkval.h"

#include <map>
#include <stdint.h>
#include <vector>

const int ROM_SIZE = 32 * 1024;
const int INCREMENT_START = ROM_SIZE - 1024;
const int DECREMENT_START = INCREMENT_START + 256;
const int LEFTSHIFT_START = DECREMENT_START + 256;
const int RIGHTSHIFT_START = LEFTSHIFT_START + 256;
const int RAM_SIZE = 16 * 1024;
const int HEAP_TOP = 0xbfff;
const int HEAP_BOTTOM = 0x8000;
const int HEAP_SIZE = HEAP_TOP - HEAP_BOTTOM + 1;

const int POINTER_READ_INSTRUCTION = HEAP_TOP - 0x2f;
const int POINTER_READ_PVECTOR = POINTER_READ_INSTRUCTION + 0x0a;       // B field of the next instruction - the one we read from.
const int JUMP_INSTRUCTION = HEAP_TOP - 0x1f;
const int JUMP_PVECTOR = JUMP_INSTRUCTION + 0x6;
const int POINTER_READ_RESULT = HEAP_TOP - 0x0f;
const int POINTER_WRITE_VALUE = POINTER_READ_RESULT;
const int POINTER_WRITE_CLEAR_INSTRUCTION = POINTER_READ_INSTRUCTION - 0x10;
const int POINTER_WRITE_COPY_INSTRUCTION = POINTER_WRITE_CLEAR_INSTRUCTION + 0x8;

class vardict;

struct variable
{
    friend vardict;
    type_t type;
    variable *next;     // for stack operations we can build a linked list, in case we get the same symbol twice. (Shadowing)
    linkval address;
    variable() {next = 0;}
private:
    int offset;     // we don't want to use this by mistake...
};

class vardict
{
    std::map<std::string, variable*> vars;
    std::vector<bool> memory_in_use;
    std::vector<bool> has_been_used;
    int first_available_space;
    int getspace(int size);
    void find_first_available_space(int searchstart = 0);
    std::vector<std::vector<std::string> > tempscopes;       // in an if/while test we may use multiple temp locations, and we don't want them to clobber each other, so we keep track of temps and clean up after test finished.
public:
    linkval addvar(std::string name, type_t type);
    void registervar(std::string name, type_t type, linkval address);    // for when we want to push an existing address as a var, and the memory is already allocated. (it's removed in the same way)
    void remove(std::string name);
    variable* getvar(std::string name);
    bool exists(std::string name);
    void push_function_scope();     // so functions can't clobber each other's memory, we mark all memory used by other functions as currently in use.
    void push_temp_scope();
    void pop_temp_scope();
    void remove_on_pop(std::string name);
    vardict();
};

class linker
{
    vardict vars;
    std::map<std::string, definition*> defined_funcs;
    std::vector<definition*> definitions;
    int index;
    std::vector<linkval> buffer;
    std::map<std::string, linkval> valtable;        // contains values for substitution
    std::vector<std::pair<std::string, std::string> > stringvalues;   // label, contents
    void savelabel(std::string, linkval);
    void write8(linkval);
    void write16(linkval);
    void padto8bytes();
    linkval evaluate_or_return_literal(expression*);
// Code generation routines:
    void emit_nfc2(linkval x, linkval y);
    void emit_branchifzero(linkval testloc, linkval dest);
    void emit_branchalways(linkval dest, bool always_emit = false);     // can pass true to disable the follow behaviour, if the jump is jumped to (e.g. end of while)
    void emit_copy(linkval src, linkval dest);
    void emit_copy_inverted(linkval src, linkval dest);
    void emit_writeconst(uint8_t val, linkval dest);
    void emit_copy_multiple(linkval src, linkval dest, int nbytes);
    void emit_writeconst_multiple(int value, linkval dest, int nbytes);
    void emit_writelabel(std::string label, linkval dest);
// Tree traversal:
    void link(funcdef*);
    void link(block*);
    void link(statement*);
    void link(funccall*);
    linkval linkfunctioncall(std::vector<expression*>&, funcdef*);
    linkval linkbuiltinfunction(std::vector<expression*>&, std::string);
    void link(goto_stat*);
    void link(if_stat*);
    void link(while_stat*);
    void link(assignment*);
    linkval evaluate(expression*);
    uint16_t evaluate(linkval);
    std::vector<char> assemble();
    void allocatefunctionstorage();
public:
    linker();
    void add_object(object* obj);
    std::vector<char> link();
};

#endif // LINKER_H_INCLUDED
