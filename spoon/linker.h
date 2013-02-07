#ifndef LINKER_H_INCLUDED
#define LINKER_H_INCLUDED

#include "object.h"

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
const int POINTER_READ_PVECTOR = POINTER_READ_INSTRUCTION;
const int JUMP_INSTRUCTION = HEAP_TOP - 0x1f;
const int JUMP_PVECTOR = JUMP_INSTRUCTION + 0x6;


// TODO: might be better to just have a stack of maps for scope, rather
// than this multiple-linked-list juggling bullshit.
struct variable
{
    type_enum type;
    variable *next;     // for stack operations we can build a linked list, in case we get the same symbol twice. (Shadowing)
    int offset;
    variable() {next = 0;}
};

class vardict
{
    std::map<std::string, variable*> vars;
    std::vector<bool> memory;
    int first_available_space;
    int getspace(int size);
    void find_first_available_space(int searchstart = 0);
public:
    int addvar(std::string name, type_enum type);
    void remove(std::string name);
    variable* getvar(std::string name);
    bool exists(std::string name);
    vardict();
};

struct substitution
{
    std::string name;
    int nbytes;
    substitution(std::string _name = "", int _nbytes = 0) {name = _name; nbytes = _nbytes;}
};

typedef enum
{
    lv_literal = 0,
    lv_symbol,
    lv_expression
} lv_type;

// linkvals are our "Assembly language" - they let us pass symbols and expressions for machine code
// instead of just the literal addresses, e.g. with labels where we don't know the location til we reach it.
// They get evaluated in the final "assemble" step.
struct linkval
{
    enum op_type {op_add};
    lv_type type;
    uint16_t literal;
    std::string sym;
    linkval *next;
    op_type operation;
    linkval(uint16_t lit) {type = lv_literal; literal = lit;}
    linkval(std::string s) {type = lv_symbol; sym = s; literal = 0;}
    linkval& operator+(uint16_t rhs);
};

class linker
{
    vardict vars;
    std::map<std::string, definition*> defined_funcs;
    std::vector<definition*> definitions;
    int index;
    std::vector<linkval> buffer;
    std::map<std::string, int> valtable;        // contains values for substitution
    void savelabel(std::string, uint16_t);
    void write8(linkval);
    void write16(linkval);
    void padto8bytes();
    void emit_nfc2(linkval x, linkval y);
    void emit_branchifzero(linkval testloc, linkval dest);
    void emit_branchalways(linkval dest);
    void emit_copy(linkval src, linkval dest);
    void emit_writeconst(uint8_t val, linkval dest);
    void emit_copy_multiple(linkval src, linkval dest, int nbytes);
    void emit_writeconst_multiple(int value, linkval dest, int nbytes);         //TODO: fill these out
    void link(funcdef*);
    void link(block*);
    void link(statement*);
    void link(funccall*);
    uint16_t linkfunctioncall(funccall*, funcdef*);
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
